/*
 ChunkStoreMDS

 maintain a file set

*/
#include <vector>
#include <algorIThm>
#include <functional>
#include <string>
#include <list>
#include <thread>
#include <mutex>
#include "FileFolder.cpp"

using namespace std;


struct fileInfo
{

        // filename unique
        string filepath;

        //  check free and ctrl file access
        bool Beusing;

        // help to recover client's filelist info
        char *owner;

        // writen size  for balance
        long offset;
};

// client request message
struct Request_MSG
{
        char *owner;

        int fileNum;
};

//client return message
// just for test
struct Return_MSG
{
        char *owner;

        vector<fileInfo> files;
        /* data */
};

//  balance policy
typedef enum
{
        UNKNOWN_OPERATION = 0,
        STORAGE_SIZE = 1,
        USING_TIME = 2

        //to add more policy

} BALANCE_POLICY;

struct client_Recovery_MSG
{
        char *owner;
        vector<fileInfo> ownedFileList;
};

// file pos
struct filepos
{

        //  if in restfilelist
        bool inRest;
        // the idx
        int idx;
};

class ChunkStoreMDS
{

private:
public:
        //to control the os file operations
        fileFolder fholder;

        // rest file size
        int restFileSize;

        // using file size  to simplfy the balance judge   and only change by deliverFiles()  and  updateFiles() ;
        int usingFileSize;

        // to store the filelist  and must support concurrent read and write in some specific position
        std::vector<fileInfo> restfilelist;

        // askFiles and updateFile  sepeartedly
        // using list for  eraseFile in vector is  expensive
        std::vector<fileInfo> usingfilelist;

        static std::string cachePath;

        // fholder and filelist sync flag
        //0 : not syncing    1 : syncing
        int sync = 0;
        // construct
        ChunkStoreMDS();

        // desturct
        ~ChunkStoreMDS();

        void staticInit();

        //init
        void staticInitInternal(fileFolder &holder);

        // print MDS info
        void printState();

        // mark file at one's own  for test
        void markOwned(string filename, char *owner);

        // public interface and  hide the balance policy setting
        void deliverFiles(Request_MSG &msg, Return_MSG &return_msg);

        //to deliver files based on specific policy  and must support concurrent ops
        void deliverFilesInternal(Request_MSG &msg, Return_MSG &return_msg, BALANCE_POLICY policy);

        // to sense the capacity of rest files and prepare to ask for more files
        void senseCapcity(double restrate);

        // add files in filelist and fholder nonsyc update ;
        void AskMoreFilesTimely();

        /*  fileholder update file info accorrding to filelist;    
            now it must created files all  before it update filelists 
            to do  : balance   
        */
        void HolderAutoUpdate();

        //fileholder recovers from crash ,and test using a specific files.txt ;
        void fileHolderRecover();

        // ask for more files forcibly in senerior which  rest files cannot meet the needs and timely-askFiles has not started
        void AskMoreFilesForcibly(int rate);

        // update fileinfo based on client writing-complete callback ;
        void updateFilelist(Return_MSG msg);

        //  to serach and modify file info  and markowned call this func
        // if write-file succeeds and modify the offset;
        void ModifyFileinfo(string filename, filepos &filepos);

        // migrate  fromlist to to list  automatically  based on the userName ;  use vector random access to mark it . and then
        void migrateFilesToList(vector<fileInfo> &fromlist, vector<fileInfo> &tolist);

        bool checkIfMeetNeed(int askFiles, int rate);

        //cacheSerialize  using self-parse policy
        void cacheSerialize();

        // just for test
        void Client_Revocer_Help(client_Recovery_MSG &msg);

        //just for MDSserver Recover
        void clearForRecoverTest();
};

ChunkStoreMDS::ChunkStoreMDS()
{
}

ChunkStoreMDS::~ChunkStoreMDS()
{
        //singleton memory recycle
        fholder.~fileFolder();

        // manual destruct
        restfilelist.clear();
        usingfilelist.clear();
}

void ChunkStoreMDS::staticInit()
{
        DiskScanner diskscannaer;

        string filePath[1] = {"D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore"};

        for (string s : filePath)
                diskscannaer.setPath(s);

        // logical folder
        fileFolder ffolder(diskscannaer);

        //cache
        ffolder.EnsureRecovery("D:\\BlobServiceData\\TestPartition\\cache.txt");

        // ChunkStoreMDS mds;
        staticInitInternal(ffolder);
}
// init ffolder to restFilelist
//  ffolder  settting complete
void ChunkStoreMDS::staticInitInternal(fileFolder &ffolder)
{

        fholder = ffolder;

        for (int i = 0; i < fholder.files.size(); i++)
        {
                fileInfo temp = {fholder.files.at(i), false, "", 0};

                restfilelist.push_back(temp);
        }

        // init restfilesize
        restFileSize = ffolder.files.size();
        usingFileSize = 0;

        printf("restfilelist  init  completed......... \r\n");
}

// mark the file  owned
void ChunkStoreMDS::markOwned(string filename, char *owner)
{

        //get the reference of specific file
        filepos fpos = {false, -1};
        ModifyFileinfo(filename, fpos);

        if (fpos.idx == -1)
        {
                printf("file path not exist \r\n");
                goto exit;
        }

        else
        {
                if (fpos.inRest)
                        restfilelist.at(fpos.idx).owner = owner;
                else
                        usingfilelist.at(fpos.idx).owner = owner;

                printf("mark owner successfully \r\n");
        }

// error handler
exit:
        cout << "Error in program.\n";
        return;
}

// to redivide , beacause we just mark fileinfo in using and not migrate them to the restfilelist  on time , so
// what we all know is that restfilelists prefers the rest fileinfo and the usingfilelist prefers the  using files;
// that  migrate operation is expensive so we will call it less time than other ops
void ChunkStoreMDS::migrateFilesToList(vector<fileInfo> &restlist, vector<fileInfo> &usinglist)
{

        // to record current state
        int usingsize = usinglist.size();

        // to optimize
        for (auto i = 0; i < restlist.size(); i++)
        {
                //  more sufficient
                //  prefer not to compare string
                if (restlist[i].Beusing)
                {
                        fileInfo temp = restlist[i];
                        usinglist.push_back(temp);
                        restlist.erase(restlist.begin() + i);
                        // back
                        i--;
                }
        }

        // just iterator  the front of vector
        for (auto i = 0; i < usinglist.size() && i < usingsize; i++)
        {
                if (!usinglist[i].Beusing)
                {
                        fileInfo temp = usinglist[i];
                        restlist.push_back(temp);
                        usinglist.erase(usinglist.begin() + i);
                        // pointer back
                        i--;
                }
        }

        printf("migrate      files  complete: \r\n");
}

// get specific file addr to modify  and  called by updateFileinfo()
// freeze the main memory and ensure to  get the specific idx
void ChunkStoreMDS::ModifyFileinfo(string filename, filepos &filepos)
{

        for (int i = 0; i < usingfilelist.size(); i++)
                if (usingfilelist.at(i).filepath.compare(filename) == 0)
                {
                        filepos.inRest = false;
                        filepos.idx = i;
                        return;
                }
        for (int i = 0; i < restfilelist.size(); i++)
                if (restfilelist.at(i).filepath.compare(filename) == 0)
                {
                        filepos.inRest = true;
                        filepos.idx = i;
                        return;
                }
}

// print MDS info
void ChunkStoreMDS::printState()
{
        printf("ChunkStoreMDS current state : \r\n");

        // list all fileinfo
        // checkIfsync();

        int cnt = 0;
        int num = 0;

        //
        for (fileInfo f : usingfilelist)

        {
                printf("busy_file_%d:  \r\n", num++);
                printf("         state:      %s\r\n", f.Beusing ? "busy" : "free");
                printf("         filepath:   %s\r\n", f.filepath.c_str());
                printf("         owner:      %s\r\n", f.Beusing ? f.owner : "vacant");
                printf("         offset:     %d\r\n", f.offset);
        }

        for (fileInfo f : restfilelist)
        {
                printf("rest_file_%d:  \r\n", cnt++);
                printf("         state:      %s\r\n", f.Beusing ? "busy" : "free");
                printf("         filepath:   %s\r\n", f.filepath.c_str());
                printf("         owner:      %s\r\n", f.Beusing ? f.owner : "vacant");
                printf("         offset:     %d\r\n", f.offset);
        }

        printf("Total state:  \r\n");
        printf("             all   files: %d   \r\n", restFileSize + usingFileSize);
        printf("             Using files: %d   \r\n", usingFileSize);
        printf("             free  files: %d   \r\n", restFileSize);
}

/*
        filelist sort func
        to do : self displince based on PolicySetting
*/

//   ascending
bool a_less_b(const fileInfo &r, const fileInfo &s)
{
        return r.offset < s.offset;
}

//  descending
bool a_greater_b(const fileInfo &r, const fileInfo &s)
{
        return r.offset > s.offset;
}

// checkFiles and asking_files_size_rate
bool ChunkStoreMDS::checkIfMeetNeed(int askFiles, int rate)
{

        //enough    extrem situaition   restfilesize all in using files
        if (restFileSize > askFiles)
                return true;
        else
        {
                //fholder.createMoreFile(), createchunkstore may be expensive so we must control the call frequency
                AskMoreFilesForcibly(rate);

                printf("Preparing available fileset...........\r\n");

                // recursive askForfiles  until its true
                checkIfMeetNeed(askFiles, 2 * rate);
        }
}

#define deafult_Asking_File_Size 10

// called by other function explicitly  and rate to optimize the asking file times
void ChunkStoreMDS::AskMoreFilesForcibly(int rate)
{

        // get more files in folder
        fholder.createMoreFile(deafult_Asking_File_Size * rate);

        int addsize = deafult_Asking_File_Size * rate;

        //update restfilesize
        restFileSize += addsize;

        //update restfiles
        vector<string>::iterator iter1;

        iter1 = fholder.files.end();
        for (int i = 0; i < addsize; i++)
        {
                fileInfo temp = {*(iter1 - i - 1), false, "", 0};
                restfilelist.push_back(temp);
        }

        printf("Asking %d files successfully  \r\n", addsize);
}

// timely check for more files

// for windows timer call
void ChunkStoreMDS::AskMoreFilesTimely()
{       
        // get more files in folder
        fholder.createMoreFile(deafult_Asking_File_Size);

        //update restfilesize
        restFileSize += deafult_Asking_File_Size;

        vector<string> &tempList = fholder.files;
        for (int i = 0; i < deafult_Asking_File_Size; i++)
        {
                // init free file
                fileInfo temp{tempList.back(), false, "", 0};
                tempList.pop_back();
                restfilelist.push_back(temp);
        }
}

//   public interface for MDSclient call
void ChunkStoreMDS::deliverFiles(Request_MSG &msg, Return_MSG &return_msg)
{

        //  BALANCE_POLICY  can be setted dynamically
        deliverFilesInternal(msg, return_msg, BALANCE_POLICY::STORAGE_SIZE);
}

//asking file rate
#define default_rate 1

//std::mutex l;
// deliver files according to policy
// must support  concurrent
void ChunkStoreMDS::deliverFilesInternal(Request_MSG &msg, Return_MSG &return_msg, BALANCE_POLICY policy)
{

        char *owner = msg.owner;

        //control  rest free fileset sync

        // multiply thread call

        // to do : decrease the lock  granularity
        //l.lock();

        switch (policy)
        {
                // according to the storage size and less is better
        case STORAGE_SIZE:
                /* code */

                //descending
                sort(restfilelist.begin(), restfilelist.end(), a_greater_b);

                size_t restSize = restfilelist.size();

                // if enough  just do it ;
                // else  ask for more files and then do it

                checkIfMeetNeed(msg.fileNum, default_rate);

                size_t i = 0;
                // filenum
                int cnt = 0;

                // all files in restfile are used but not miagrated ...

                while (cnt < msg.fileNum && i < restfilelist.size())
                {

                        if (restfilelist[i].Beusing)
                        //skip using but not updated file
                        {
                                i++;
                                if (i == restfilelist.size())
                                {
                                        printf("restfile in not enough \r\n", msg.owner);

                                        AskMoreFilesForcibly((int)(msg.fileNum - cnt) / deafult_Asking_File_Size + 1);
                                        break;
                                }
                        }

                        {
                                // markusing
                                restfilelist[i].owner = msg.owner;

                                restfilelist[i].Beusing = true;

                                // struct assign  copy and new construct
                                fileInfo temp = fileInfo(restfilelist[i]);

                                return_msg.files.push_back(temp);

                                // insert a file
                                cnt++;
                                i++;

                                if (i == restfilelist.size())
                                {
                                        printf("restfile in not enough \r\n", msg.owner);
                                        AskMoreFilesForcibly(msg.fileNum - cnt);
                                        break;
                                }
                        }
                }

                //asking more files

                usingFileSize += msg.fileNum;

                printf("deliver files to %s completed  \r\n", msg.owner);

                break;
        }
        //case  otherPolicy to be added

        // default:
        //         printf("unkonwn police  \r\n");
        //         break;
        // }

        // l.unlock();
};

// self-sense and ask for files
void ChunkStoreMDS::senseCapcity(double rest_rate)
{

        if (usingFileSize == 0)
        {
                printf("current state  : full rest   \r\n");
                return;
        }
        if ((double)restFileSize / (double)(usingFileSize + restFileSize) < rest_rate)
        {
                printf("current state  : need to balance  \r\n");

                // to keep the rest/using rate above the assigned rest_rate

                printf("balance proceeding.....  \r\n");

                while ((double)restFileSize / (double)(usingFileSize + restFileSize) < rest_rate)
                        AskMoreFilesForcibly(default_rate * 10);

                printf("balance completed.....  \r\n");
        }

        else
        {
                printf("current MDS rest_rate  : %f   \r\n", (double)restFileSize / (double)(usingFileSize + restFileSize));
                printf("current MDS state  :  well  \r\n");
                return;
        }
}

//recover model
void ChunkStoreMDS::cacheSerialize()
{
        // to do
        // std::string  defalutcachePath="D:\\cache";

        // string  path=cachePath.compare("")==0?defalutcachePath:cachePath;

        // printf("current cache path:  %s  \r\n",path);

        // // serialize
        // std::ofstream location_out;
        // 	location_out.open(path, std::ios::out | std::ios::app);

        // //  fholder .serilize
        // fholder.EnsureRecovery("D:\\cache");

        // // //  fholder.recover
        // // if(fholder.files.size()==0)
        // //         fholder.recover();

        // if(location_out.is_open())
        // {
        //     for(fileInfo  f : restfilelist)
        //         //format   filepath-beusing-owner-size();
        //         location_out<<<<endl;

        //     cout<<"cache builds in  :"<<path<<std::endl;
        //     return;
        // }

        // cout<<"cache file cannot be opened  :"<<cacheTxtPath<<std::endl;
}

void ChunkStoreMDS::fileHolderRecover()
{
        // to do
}

void clearForRecoverTest()
{

        // restfileli
}

// to receive client info and update  and lock the thread
// we must  lock the  filelist vector
void ChunkStoreMDS::updateFilelist(Return_MSG msg)
{
        char *owner = msg.owner;

        printf("update writenfiles from  client: %s     beginning.....  \r\n", owner);

        //  update mds fileinfo  according to client_return_msg
        // vector<fileInfo> &writenFiles = msg.files;

        filepos fpos = filepos{false, -1};

        for (fileInfo finfo : msg.files)
        {
                /* code */
                ModifyFileinfo(finfo.filepath, fpos);

                if (fpos.idx != -1)
                {
                        // maybe sometimes the marked file has not been migrated
                        if (fpos.inRest)
                        {
                                restfilelist.at(fpos.idx).owner = "";
                                restfilelist.at(fpos.idx).offset = finfo.offset;
                                restfilelist.at(fpos.idx).Beusing = false;
                        }
                        else
                        {
                                usingfilelist.at(fpos.idx).owner = "";
                                usingfilelist.at(fpos.idx).offset = finfo.offset;
                                usingfilelist.at(fpos.idx).Beusing = false;
                        }

                        printf("return  writenfiles from  client: %s     beginning.....  \r\n", owner);
                }

                // reset
                fpos.idx = -1;
        }

        restFileSize += msg.files.size();
        usingFileSize -= msg.files.size();

        printf("update writenfiles from  client: %s     completed.....  \r\n", owner);
}

// to recover client fileinfo
void ChunkStoreMDS::Client_Revocer_Help(client_Recovery_MSG &msg)
{

        

        for (fileInfo finfo : usingfilelist)
        {
                // to decrease the string compare op times 
                if (finfo.Beusing)
                {
                      if (strcmp(msg.owner, finfo.owner) == 0)
                                // the last call state and may casue  inconsistency
                                msg.ownedFileList.push_back(fileInfo(finfo));
                }
        }

        for (fileInfo finfo : restfilelist)
        {
                if (finfo.Beusing)
                {
                        if (strcmp(msg.owner, finfo.owner) == 0)
                                // the last call state and may casue  inconsistency
                                msg.ownedFileList.push_back(fileInfo(finfo));
                }
        }

        printf("client: %s   recovery  completed.....  \r\n", msg.owner);
}

int DiskScanner::curAddedFilenum = 0;

// // unit test
// int main()
// {

//         // compile cmd g++  C:\Users\t-zhfu\Documents\InternPJ\ChunkStoreMDSManager.cpp  C:\Users\t-zhfu\Documents\InternPJ\ChunkStoreMDS.cpp C:\Users\t-zhfu\Documents\InternPJ\DiskScanner.cpp   C:\Users\t-zhfu\Documents\InternPJ\FileFolder.cpp  -o test

//         //g++  F:\InternPj\InternPJ\ChunkStoreMDS.cpp  F:\InternPj\InternPJ\DiskScanner.cpp  F:\InternPj\InternPJ\FileFolder.cpp -o test

//         // DiskScanner diskscannaer;

//         // string filePath[1] = {"D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore"};

//         // for (string s : filePath)
//         //         diskscannaer.setPath(s);

//         // // logical folder
//         // fileFolder ffolder(diskscannaer);

//         // //cache
//         // ffolder.EnsureRecovery("D:\\BlobServiceData\\TestPartition\\cache.txt");

//         ChunkStoreMDS mds;
//         // mds.staticInitInternal(ffolder);
//         mds.staticInit();

//         //  test  add files
//         mds.AskMoreFilesForcibly(3);

//         mds.printState();

//         // mds.AskMoreFilesForcibly(1);

//         // mds.printState();

        

//         vector<fileInfo> ownedfile;

//         Request_MSG requestmsg = {"fzy", 12};

//         Return_MSG returnmsg = {"fzy", ownedfile};

//         mds.deliverFiles(requestmsg, returnmsg);

//         string deligit = "  ";

//         vector<fileInfo> returnfile;

//         for (fileInfo f : returnmsg.files)
//         { //mock write
//                 f.offset = 100;
//                 //            cout << f.filepath <<deligit<< f.Beusing <<deligit<< f.offset <<deligit<< f.owner << std::endl;
//                 returnfile.push_back(fileInfo(f));
//         }

//         Return_MSG rmsg1 = {"fzy", returnfile};

//         mds.updateFilelist(rmsg1);

//         Request_MSG requestmsg2 = {"fzy", 20};
//         mds.deliverFiles(requestmsg2, returnmsg);

//         // Request_MSG requestmsg3 = {"fyh", 50};
//         // mds.deliverFiles(requestmsg3, returnmsg);

//         // Request_MSG requestmsg4 = {"fzy", 500};
//         // mds.deliverFiles(requestmsg4, returnmsg, BALANCE_POLICY::STORAGE_SIZE);

//         // Request_MSG requestmsg5 = {"fzy", 100};
//         // mds.deliverFiles(requestmsg5, returnmsg, BALANCE_POLICY::STORAGE_SIZE);

//         // for(fileInfo  f: returnmsg.files)
//         //          cout << f.filepath <<deligit<< f.Beusing <<deligit<< f.offset <<deligit<< f.owner << std::endl;

//         //   Request_MSG requestmsg3 = {"fzy", 50};
//         // mds.deliverFiles(requestmsg3, returnmsg, BALANCE_POLICY::STORAGE_SIZE);
//         double rate = 0.5;

//         //         //  using : 12
//         //         mds.printState();

//         // test file
//         mds.migrateFilesToList(mds.restfilelist, mds.usingfilelist);

       
//         mds.printState();

//         mds.senseCapcity(rate);

//         vector<fileInfo> recoverfile;

//         client_Recovery_MSG  recover={"fzy",recoverfile};


//         cout << "test recovery"<< std::endl;


//         mds.Client_Revocer_Help(recover);

//         for(fileInfo  f :recover.ownedFileList)
//                 cout << f.filepath <<deligit<< f.Beusing <<deligit<< f.offset <<deligit<< f.owner << std::endl;

//         return 0;
// }