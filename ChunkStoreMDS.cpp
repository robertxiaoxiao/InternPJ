/*
 ChunkStoreMDS

 maintain a file set

*/
#include <vector>
#include "FileFolder.cpp"

#include <algorIThm>
#include <functional>
#include <string>
#include <list>
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

        vector<fileInfo> writenFilesinfo;
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

class ChunkStoreMDS
{

private:
        //to control the os file operations
        fileFolder fholder;

        // rest file size
        int restFileSize;

        // using file size   to simplfy the balance judge   and only change by deliverFiles()  and  updateFiles() ;
        int usingFileSize;

        // to store the filelist  and must support concurrent read and write in some specific position
        std::vector<fileInfo> restfilelist;

        // askFiles and updateFile  sepeartedly
        // using list for  eraseFile in vector is  expensive
        std::vector<fileInfo> usingfilelist;

        // fholder and filelist sync flag
        //0 : not syncing    1 : syncing
        int sync = 0;

public:
        // construct
        ChunkStoreMDS();

        // desturct
        ~ChunkStoreMDS();

        //init
        void staticInit(fileFolder &holder);

        // print MDS info
        void printState();

        // mark file at one's own
        void markOwned(string filename, char *owner);

        //to deliver files based on specific policy  and must support concurrent ops
        void deliverFiles(Request_MSG msg, vector<fileInfo> &ownerFileList, BALANCE_POLICY policy);

        // to sense the capacity of rest files and prepare to ask for more files
        void senseCapcity(double restrate);

        // add files in filelist and fholder nonsyc update ;
        void AskMoreFilesTimely();

        // fileholder update file info accorrding to filelist;
        void HolderAutoUpdate();

        //fileholder recovers from crash ,and test using a specific files.txt ;
        void fileHolderRecover();

        // ask for more files forcibly in senerior which  rest files cannot meet the needs and timely-askFiles has not started
        void AskMoreFilesForcibly(int rate);

        // update fileinfo based on client writing-complete callback ;
        void updateFilelist(Return_MSG msg);

        //  to serach and modify file info  and markowned call this func
        // if write-file succeeds and modify the offset;
        fileInfo *ModifyFileinfo(string filename);

        void resetFileinfo(string filename);

        // write info succeed only in accepting return value and otherwise reset the  offset;
        void AotomicOPs(string filename);

        // migrate  fromlist to to list  automatically  based on the userName ;  use vector random access to mark it . and then
        void migrateFilesToList(vector<fileInfo> &fromlist, vector<fileInfo> &tolist);

        bool checkIfMeetNeed(size_t restsize, int askFiles, int rate);

        // just for test
        client_Recovery_MSG Client_Revocer_Help(char *owner);
};

ChunkStoreMDS::~ChunkStoreMDS()
{
        //singleton memory recycle
        fholder.~fileFolder();

        // manual destruct
        restfilelist.clear();
        usingfilelist.clear();
}

// init ffolder to restFilelist
//  ffolder  settting complete
void ChunkStoreMDS::staticInit(fileFolder &ffolder)
{

        fholder = ffolder;

        for (int i = 0; i < fholder.files.size(); i++)
        {
                fileInfo temp = {fholder.files.at(i), false, "", 0};

                restfilelist.push_back(temp);
        }

        //
        printf("restfilelist  init  completed......... \r\n");
}

// mark the file  owned
void ChunkStoreMDS::markOwned(string filename, char *owner)
{

        //get the reference of specific file
        fileInfo *temp = ModifyFileinfo(filename);

        if (temp == NULL)
        {
                printf("file path not exist \r\n");
                goto exit;
        }

        else
        {
                temp->owner = owner;

                printf("mark owner successfully \r\n");
        }

// error handler
exit:
        cout << "Error in program.\n";
        return;
}

// to redivide , beacause we just mark fileinfo in using and not migrate them to the restfilelist  on time , so
// we all know is the restfilelists prefers the rest fileinfo and the usingfilelist prefers the  using files;
// that operation is expensive so we will call it less time than other ops
void ChunkStoreMDS::migrateFilesToList(vector<fileInfo> &restlist, vector<fileInfo> &usinglist)
{

        // to record current state
        int usingsize = usinglist.size();

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
}

// get specific file addr to modify  and  called by updateFileinfo()
fileInfo *ChunkStoreMDS::ModifyFileinfo(string filename)
{

        for (fileInfo f : usingfilelist)
        {
                if (f.filepath.compare(filename) == 0)
                        return &f;
        }

        for (fileInfo f : restfilelist)
        {
                if (f.filepath.compare(filename) == 0)
                        return &f;
        }

        // not found
        return NULL;
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
                printf("file_%d:  \r\n", num++);
                printf("         state:      %s\r\n", f.Beusing ? "busy" : "free");
                printf("         filepath:   %s\r\n", f.filepath);
                printf("         owner:      %s\r\n", f.Beusing ? f.owner : "");
                printf("         offset:     %I64d\r\n", f.offset);
        }

        for (fileInfo f : restfilelist)
        {
                printf("file_%d:  \r\n", cnt++);
                printf("         state:      %s\r\n", f.Beusing ? "busy" : "free");
                printf("         filepath:   %s\r\n", f.filepath);
                printf("         owner:      vacant\r\n", f.Beusing ? f.owner : "");
                printf("         offset:     %I64d\r\n", f.offset);
        }

        printf("Total state:  \r\n");
        printf("             all   files: %d   \r\n", num + cnt);
        printf("             Using files: %d   \r\n", num);
        printf("             free  files: %d   \r\n", cnt);
}

/*
filelist sort func
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
bool ChunkStoreMDS::checkIfMeetNeed(size_t restsize, int askFiles, int rate)
{

        //enough
        if (restFileSize > askFiles)
                return true;
        else
        {

                //fholder.createMoreFile();
                AskMoreFilesForcibly(rate);

                // recursive askForfiles  until its true
                if (checkIfMeetNeed(restFileSize, askFiles, 2 * rate))
                        return true;
        }
}

//asking file rate
#define default_rate 1

// deliver files according to policy
// must support  concurrent
void ChunkStoreMDS::deliverFiles(Request_MSG msg, vector<fileInfo> &ownerFileList, BALANCE_POLICY policy)
{

        char *owner = msg.owner;

        //control  rest free fileset sync

        switch (policy)
        {
                // according to the storage size and less is better
        case STORAGE_SIZE:
                /* code */

                //descending
                sort(restfilelist.begin(), restfilelist.end(), a_greater_b);

                size_t restSize = restfilelist.size;

                // if enough  just do it ;
                // else  ask for more files and then do it

                checkIfMeetNeed(restSize, msg.fileNum, default_rate);

                for (size_t i = 0; i < msg.fileNum; i++)
                {
                        //deliver files to others according to the writen storage
                        fileInfo f = restfilelist.back();
                        restfilelist.pop_back();
                        f.owner = owner;
                        usingfilelist.push_back(f);
                }
                break;

                //case  otherPolicy to be added

        default:

                printf("unkonwn police  \r\n");
                break;
        }
};

#define deafult_Asking_File_Size 10

// called by other function explicitly  and rate to optimize the asking file times
void ChunkStoreMDS::AskMoreFilesForcibly(int rate)
{

        // get more file in folder
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
        
        
         printf("Asking %d files successfully  \r\n",addsize);
      
}

// timely check for more files

// for windows timer call
void ChunkStoreMDS::AskMoreFilesTimely()
{
        fholder.createMoreFile(deafult_Asking_File_Size);
        vector<string> &tempList = fholder.files;
        for (int i = 0; i < deafult_Asking_File_Size; i++)
        {
                // init free file
                fileInfo temp{tempList.back(), false, "", 0};
                tempList.pop_back();
                restfilelist.push_back(temp);
        }
}

// self-sense and ask for files
void ChunkStoreMDS::senseCapcity(double rest_rate)
{
        if (usingfilelist.size == 0)
                return;

        if ((double)(restfilelist.size / usingfilelist.size) < rest_rate)
        {
                printf("current state  : need to balance  \r\n");
                printf("balance proceeding.....  \r\n");
                AskMoreFilesForcibly(default_rate * 10);

                printf("balance completed.....  \r\n");
        }

        else
        {
                printf("current state  :  well  \r\n");
                return;
        }
}

//  static init fholder for the first call
// entire filehoder sync with  restfilelist
//   1   HolderAutoUpdate   copy all file into restfilelist
//   2   AskMoreFilesForcibly  or AskMoreFilestimely  push_back into the restfilelist ;

void ChunkStoreMDS::HolderAutoUpdate()
{
        vector<string> &tempList = fholder.files;

        int size = tempList.size();

        for (size_t i = 0; i < size; i++)
        { //update restfilelist
                restfilelist.push_back(fileInfo{tempList.at(i), false, "", 0});
        }

        printf("first init completed.....  \r\n");
}

void ChunkStoreMDS::fileHolderRecover()
{

        // to do
}

// to receive client info and update
void ChunkStoreMDS::updateFilelist(Return_MSG msg)
{

        char *owner = msg.owner;
        //  client inits the writen file info  such as offset
        vector<fileInfo> &writenFiles = msg.files;

        for (fileInfo finfo : writenFiles)
        {
                /* code */
                fileInfo *temp = ModifyFileinfo(finfo.filepath);

                temp->owner = "";

                // file.offset set by client

                restfilelist.push_back(*temp);

                // to optimize
                resetFileinfo(temp->filepath);
        }
}

// to recover client fileinfo
client_Recovery_MSG ChunkStoreMDS::Client_Revocer_Help(char *owner)
{

        client_Recovery_MSG m = {};
        m.owner = owner;

        for (fileInfo finfo : usingfilelist)
        {

                if (std::strcmp(owner, finfo.owner) == 0)
                        // the last call state and may casue  inconsistency
                        m.ownedFileList.push_back(finfo);
        }

        printf("client: %s   recovery  completed.....  \r\n", owner);

        return m;
}