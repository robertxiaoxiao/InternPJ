/*
 ChunkStoreMDS

 maintain a file set

*/
#include <vector>
#include "FileFolder.cpp"
#include <algorIThm>
#include <functional>
#include "ChunkStoreMDS.h"
using namespace std;
struct fileInfo
{

        // filename unique
        string filepath;

        //  check free and ctrl file access
        bool  Beusing;

        // help to recover client's filelist info
        string owner;

        // writen size  for balance
        long   offset;
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
        void staticInit(fileFolder holder){

                fholder=holder;

        };

        // print MDS info
        void printState();

        // mark file at one's own
        void markOwned(char *filename, char *owner);

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
        void updateFilelist(vector<fileInfo> &writenFiles);

        //  to serach and modify file info  and markowned call this func
        // if write-file succeeds and modify the offset;
        fileInfo *ModifyFileinfo(char *filename);

        // write info succeed only in accepting return value and otherwise reset the  offset;
        void AotomicOPs(char *filename);

        bool checkIfMeetNeed(size_t restsize, int askFiles, int rate);
};

ChunkStoreMDS::~ChunkStoreMDS()
{
        //singleton memory recycle
        fholder.~fileFolder();
        restfilelist.clear();
        usingfilelist.clear();
}

// mark the file  owned
void ChunkStoreMDS::markOwned(char *filename, char *owner)
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

// get specific file addr to modify
fileInfo *ChunkStoreMDS::ModifyFileinfo(char *filename)
{

        for (fileInfo f : usingfilelist)
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

#define deafult_Asking_File_Size 10

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
                for (int i = 0; i < deafult_Asking_File_Size * rate; i++)
                {
                        // init free file
                        fileInfo temp{tempList.back(), false, "", 0};
                        tempList.pop_back();
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
        if(usingfilelist.size==0)
                return;


        if ((double) restfilelist.size / usingfilelist.size < rest_rate)
                AskMoreFilesForcibly(default_rate*10);
                 
}

