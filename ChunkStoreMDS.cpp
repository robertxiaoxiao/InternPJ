/*
 ChunkStoreMDS

 maintain a file set

*/
#include  <vector>
#include "FileFolder.cpp"


using namespace std;
// basic fileInfo  as one FileSet-table row
struct fileInfo{

     // filename unique
     string filepath ;

    //  check free and ctrl file access
    bool  Beusing ;

    // help to recover client's filelist info 
    string owner ;

    // writen size  for balance
    long  offset;
};


class ChunkStoreMDS{

        private:
                //to control the os file operations 
                fileFolder fholder ;

                // to store the filelist  and must support concurrent read and write in some specific position
                std::vector<fileInfo>  filelist;


        public:
                
                // mark file at one's own 
                void  markOwend(string filename,string owner);

                //to deliver files based on specific policy  and must support concurrent ops 
                void  deliverFiles(string owner,vector<fileInfo>& ownerList,string policy);

                // to sense the capacity of rest files and prepare to ask for more files 
                void  senseCapcity();

                // add files in filelist and fholder nonsyc update ;
                void  AskMoreFilesTimely();

                // fileholder update file info accorrding to filelist;
                void  HolderAutoUpdate();

                //fileholder recovers from crash ,and test using a specific files.txt ;
                void fileHolderRecover();

                // ask for more files forcibly in senerior which  rest files cannot meet the needs and timely-askFiles has not started 
                void  AskMoreFilesForcibly();

                // update fileinfo based on client writing-complete callback ;
                void  updateFilelist(vector<fileInfo>&  writenFiles);


                //  to serach and modify file info  and markowned call this func 
                // if write-file succeeds and modify the offset; 
                void ModifyFileinfo(string filename,fileInfo& fileinfo);

                // write info succeed only in accepting return value and otherwise reset the  offset;
                void AotomicOPs(string filename);
}；