#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "DiskScanner.h"
#include "FileInfoScan.cpp"
using namespace std;
class fileFolder
{

private:
    // cache path
    string cacheTxtPath;

public:
    DiskScanner filescanner;

    // file name space
    std::vector<std::string> files;

    // last modified position
    int lastModifiedsize;

    fileFolder(DiskScanner fileScanner)
    {

        filescanner = fileScanner;
        filescanner.GetAllFormatFiles(files,".ccc");

        // mark filelast size ;
        lastModifiedsize = filescanner.curAddedFilenum;
    };

    ~fileFolder()
    {
        // manual destruct
        files.clear();
    };

    // print ctrl files
    void listFiles()
    {
        for (auto i = 0; i < files.size(); i++)
            cout << "file_" << i << ":   " << files.at(i) << std::endl;
    };

    // create file ;
    void createMoreFile(int filenum)
    {
        filescanner.createFiles(filenum, files);

        // ensure file id incre
       // filescanner.curAddedFilenum+=filenum;

        // mark modify size
        lastModifiedsize += filenum;
        cout << "successfully add files :" << filenum << std::endl;
    };

    // just for test recovery
    void clearFiles()
    {
        files.clear();
    }


    // add file ;
    void addFile(std::string file)
    {
        files.push_back(file);
        cout << "successfully add file :" << file << std::endl;
    };

    // delete file;
    void deleteFile(std::string filename)
    {
        int i = checkFileContained(filename);

        if (i != -1)
        {
            files.erase(files.begin() + i);
            cout << "successfully delete file :" << filename << std::endl;
            return;
        }

        cout << " file  not exist :" << filename << std::endl;
    };

    //  check if file in fileFolder
    int checkFileContained(string filename)
    {

        for (auto i = 0; i < files.size(); i++)
            if (files.at(i).compare(filename) == 0)
                return i;

        return -1;
    };


    // 配置恢复路径
    void EnsureRecovery(string s)
    {
        cacheTxtPath = s; 
    }


    // "write catch"
    void cache(){
        
        std::ofstream location_out;
		location_out.open(cacheTxtPath, std::ios::out | std::ios::app);

        if(location_out.is_open())
        {

            for(string s:files)
                location_out<<s<<endl;

	        cout<<"cache builds in  :"<<cacheTxtPath<<std::endl;
            return;
        }

        cout<<"cache file cannot be opened  :"<<cacheTxtPath<<std::endl;
	
    }
 
};

//set fileAdded begin idx
int DiskScanner::curAddedFilenum=0;

int main(){
     DiskScanner  diskscannaer;

     string filePath[1]= {"D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore"};  

    for(string s :filePath)
        diskscannaer.setPath(s);

        // logical folder 
        fileFolder ffolder(diskscannaer);
 
        //cache
        ffolder.EnsureRecovery("D:\\BlobServiceData\\TestPartition\\cache.txt");


        ffolder.listFiles();

        ffolder.createMoreFile(5);

         cout<<"current file  addID  :"<<ffolder.filescanner.curAddedFilenum<<std::endl;
          cout<<"last modified position  :"<<ffolder.lastModifiedsize<<std::endl;


        ffolder.cache();

        ffolder.createMoreFile(5);


        cout<<"current file  addID  :"<<ffolder.filescanner.curAddedFilenum<<std::endl;
        cout<<"last modified position  :"<<ffolder.lastModifiedsize<<std::endl;

        ffolder.listFiles();
        

return 0;

}