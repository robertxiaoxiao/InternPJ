/*
 ChunkStoreMDS

 maintain a file set

*/
#include <vector>
#include "FileFolder.cpp"
#include <algorIThm>
#include <functional>
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
        /* data */
        char *owner;
        vector<fileInfo> ownedFileList;
};

class ChunkStoreMDS
{

private:

        //to control the os file operations
        fileFolder fholder;

        // to store the filelist  and must support concurrent read and write in some specific position
        std::vector<fileInfo> restfilelist;

        // askFiles and updateFile  sepeartedly
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
        void staticInit(fileFolder holder)
        {

                fholder = holder;
        };

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

        // migrate  fromlist to to list
        void migrateFileToList(vector<fileInfo> &fromlist, vector<fileInfo> &tolist, string filename);

        bool checkIfMeetNeed(size_t restsize, int askFiles, int rate);

        // just for test
        client_Recovery_MSG Client_Revocer_Help(char *owner);
};

ChunkStoreMDS::~ChunkStoreMDS()
{
        //singleton memory recycle
        fholder.~fileFolder();
        restfilelist.clear();
        usingfilelist.clear();
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

                migrateFileToList(restfilelist, usingfilelist, filename);

                printf("mark owner successfully \r\n");
        }

// error handler
exit:
        cout << "Error in program.\n";
        return;
}

void ChunkStoreMDS::migrateFileToList(vector<fileInfo> &fromlist, vector<fileInfo> &tolist, string filename)
{

        // to do
}

// get specific file addr to modify
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

// clear file owner
void ChunkStoreMDS::resetFileinfo(string filename)
{

        for (size_t i = 0; i < usingfilelist.size(); i++)
        {
                /* code */
                fileInfo &temp = usingfilelist.at(i);
                if (temp.filepath == filename)
                {
                        printf("reset file :%s .....  \r\n", temp.filepath);
                        usingfilelist.erase(usingfilelist.begin() + i);
                        printf("reset file  completed .....  \r\n");
                        return;
                }
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

        for (fileInfo f : usingfilelist)

        {
                printf("file_%d:  \r\n", num++);
                printf("         state:      busy\r\n");
                printf("         filepath:   %s\r\n", f.filepath);
                printf("         owner:      %s\r\n", f.owner);
                printf("         offset:     %I64d\r\n", f.offset);
                cnt++;
        }

        for (fileInfo f : restfilelist)
        {
                printf("file_%d:  \r\n", cnt++);
                printf("         state:      free\r\n");
                printf("         filepath:   %s\r\n", f.filepath);
                printf("         owner:      vacant\r\n", f.owner);
                printf("         offset:     %I64d\r\n", f.offset);
        }

        printf("Total state:  \r\n", num++);
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

bool ChunkStoreMDS::checkIfMeetNeed(size_t restsize, int askFiles, int rate)
{

        //enough
        if (restsize > askFiles)
                return true;
        else
        {

                //fholder.createMoreFile();
                AskMoreFilesForcibly(rate);

                // recursive askForfiles  until its true
                if (checkIfMeetNeed(restfilelist.size, askFiles, 2 * rate))
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

#define deafult_Asking_File_Size 100

// called by other function explicitly  and rate to optimize the asking file times
void ChunkStoreMDS::AskMoreFilesForcibly(int rate)
{

        // get more file in folder
        fholder.createMoreFile(deafult_Asking_File_Size * rate);

        // start sync
        // catch any errors
        try
        {
                sync = 1;
                vector<string> &tempList = fholder.files;

                int size = tempList.size();

                for (int i = 0; i < deafult_Asking_File_Size * rate; i++)
                {
                        // init free file
                        fileInfo temp{tempList.at(size - 1 - i), false, "", 0};

                        restfilelist.push_back(temp);
                }

                // sync reset
                sync = 0;
        }
        catch (...)
        {
                sync = 0;
        }
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

// to recover client fileinfo  just for test
client_Recovery_MSG ChunkStoreMDS::Client_Revocer_Help(char *owner)
{

        client_Recovery_MSG m = {};
        m.owner = owner;

        for (fileInfo finfo : usingfilelist)
        {

                if (strcmp(owner, finfo.owner) == 0)
                        // the last call state and may casue  inconsistency
                        m.ownedFileList.push_back(finfo);
        }

        printf("client: %s   recovery  completed.....  \r\n", owner);

        return m;
}



//
