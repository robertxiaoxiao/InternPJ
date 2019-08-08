#include "ChunkStoreMDS.cpp"
class MDSClient
{
private:
    /* data */
public:

    
    MDSClient();
    MDSClient(char* name);
    ~MDSClient();

    // test reference to simulate the rpc call
    
    ChunkStoreMDS*   MDS;

    char* clientname;

    vector<fileInfo>  localFilelist ;

    vector<fileInfo>  unusedfile;

    //  bind client to specific MDSsvc
    void bindMDS(ChunkStoreMDS*  tempMDS);

    // ask MDSserver for files
    void RequestFiles(int filesize);

    // return to update MDSserver fileinfo    
    void ReturnFile();

    // persist writen fileList in local files
    void PersistLocalFile(string localFilePath);

    // parse local file 
    void ParseLocalFile(string localFilePath);

    // recovery request
    void Request_Recovery(ChunkStoreMDS  MDSserver);

};
MDSClient::MDSClient()
{   
    
}

MDSClient::MDSClient(char* name)
{   
    clientname=name;
}

MDSClient::~MDSClient()
{
}


void MDSClient::bindMDS(ChunkStoreMDS*  MDS){
        // pointer pass  in same memory  and it will be refactored when used in rpc
        this->MDS=MDS;
}


void MDSClient::RequestFiles(int filesize ){

            Request_MSG   request_msg={clientname,filesize};

            Return_MSG   writenfiles={clientname,localFilelist};

            if(MDS==NULL)
                {printf("client: %s   binds MDSserver  failed   \r\n", clientname );
                    return;
                }

            // it should be internal interface based on balancy policy 
            (*MDS).deliverFiles(request_msg,writenfiles);

            printf("client: %s   has received  %d  files   successfully.....  \r\n", clientname , request_msg.fileNum);

}


void   MDSClient::ReturnFile(){

         Return_MSG   updatefiles_MSG={clientname,unusedfile};

        (*MDS).updateFilelist(updatefiles_MSG);

}

// for multithread test 
void testThreadCall(int clientnum,ChunkStoreMDS* mds){

     vector<MDSClient>  clients;
     string p; 

     for(int i=0;i<clientnum ;i++)
     {
        p.assign("client").append(std::to_string(i));

        MDSClient   temp((char*)p.data());

        temp.bindMDS(mds);

        thread   t(&MDSClient::RequestFiles,temp,30) ;
        // temp.RequestFiles(20);
        t.join();         
     }
     
}

 //g++  C:\Users\t-zhfu\Documents\InternPJ\MDSClient.cpp   C:\Users\t-zhfu\Documents\InternPJ\DiskScanner.cpp C:\Users\t-zhfu\Documents\InternPJ\FileFolder.cpp -o test
int main(){


        ChunkStoreMDS  mds ;
        mds.staticInit();

        testThreadCall(2,&mds);
        // MDSClient  client("fzy");

        
        // MDSClient  client1("fyh");

        
        // MDSClient  client2("jdj");
        
        
        // client.bindMDS(&mds);
        // client1.bindMDS(&mds);
        // client2.bindMDS(&mds);

    

        // thread t1(&MDSClient::RequestFiles,client,20);
        // thread t2(&MDSClient::RequestFiles,client1,20);
        // thread t3(&MDSClient::RequestFiles,client2,30);
        // thread t4(&MDSClient::RequestFiles,client,30);

        // client1.RequestFiles(20);
        // client2.RequestFiles(20);
        // client.RequestFiles(20);
        // t1.join();
        // t2.join();
        // t3.join();
        // t4.join();


        mds.printState();

        return 0;

}




