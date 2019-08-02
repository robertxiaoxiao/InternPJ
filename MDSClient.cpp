#include "preIn.h"
class MDSClient
{
private:
    /* data */
public:

    
    MDSClient(/* args */);
    ~MDSClient();

    // test reference to simulate the rpc call
    
    ChunkStoreMDS   MDS;

    vector<fileInfo>  localFilelist ;


    //  bind client to specific MDSsvc
    void bindMDS(ChunkStoreMDS*  tempMDS);

    // ask MDSserver for files
    void RequestFiles(ChunkStoreMDS  MDSserver);

    // return to update MDSserver fileinfo    
    void ReturnFile(ChunkStoreMDS  MDSserver);

    // persist writen fileList in local files
    void PersistLocalFile(string localFilePath);

    // parse local file 
    void ParseLocalFile(string localFilePath);

    // recovery request
    void Request_Recovery(ChunkStoreMDS  MDSserver);

};


MDSClient::MDSClient(/* args */)
{
}

MDSClient::~MDSClient()
{
}


void MDSClient::bindMDS(ChunkStoreMDS*  MDS){
        // pointer pass  in same memory 
        this->MDS=*MDS;
}


void MDSClient::RequestFiles(ChunkStoreMDS  MDSserver ){


}