#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "FileFolder.h"

using namespace std;

    fileFolder::fileFolder(DiskScanner fileScanner)
    {

        filescanner = fileScanner;
        filescanner.GetAllFormatFiles(files,".ccc");

        // mark filelast size ;
        lastModifiedsize = filescanner.curAddedFilenum;
    };

    fileFolder::~fileFolder()
    {
        // manual destruct
        files.clear();
    };

    // print ctrl files
    void fileFolder::listFiles()
    {
        for (auto i = 0; i < files.size(); i++)
            cout << "file_" << i << ":   " << files.at(i) << std::endl;
    };

    // create file ;
    void fileFolder::createMoreFile(int filenum)
    {
        filescanner.createFiles(filenum, files);

        // ensure file id incre
       // filescanner.curAddedFilenum+=filenum;

        // mark modify size
        lastModifiedsize += filenum;
        cout << "successfully add files :" << filenum << std::endl;
    };

    // just for test recovery
    void fileFolder::clearFiles()
    {
        files.clear();
    }


    // add file ;
    void fileFolder::addFile(std::string file)
    {
        files.push_back(file);
        cout << "successfully add file :" << file << std::endl;
    };

    // delete file;
    void fileFolder::deleteFile(std::string filename)
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
    int fileFolder::checkFileContained(string filename)
    {

        for (auto i = 0; i < files.size(); i++)
            if (files.at(i).compare(filename) == 0)
                return i;

        return -1;
    };


    // 配置恢复路径
    void fileFolder::EnsureRecovery(string s)
    {
        cacheTxtPath = s; 
    }
    
    // "write catch"
    void fileFolder::cache(string catchPath){
        
        std::ofstream location_out;

        string path =catchPath.compare("")==0?cacheTxtPath:catchPath;

		location_out.open(path, std::ios::out);

        if(location_out.is_open())
        {

            for(string s:files)
                location_out<<s<<endl;

	        cout<<"cache builds in  :"<<path<<std::endl;
            return;
        }

        else 
              cout<<"cache file cannot be opened  :"<<path<<std::endl;
        

        location_out.close();

        // cout<<"cur path  :"<<path<<std::endl;
        // cout<<"cur cachepath  :"<<cacheTxtPath<<std::endl;

    }

    void fileFolder::recover(){

    cout<<"filefholder  recover begins ...."<<std::endl;

    if(cacheTxtPath.compare("")==0)
            {
                cout<<"cache file has not been inited  :"<<std::endl;
                return;
            }
            
  //  cout<<"cur cachepath  :"<<cacheTxtPath<<std::endl;
    ifstream file(cacheTxtPath, ios::in);

	if (!file.is_open())
	{
		cout << "file open fails " << endl;
		return;
	}

 	string line;

	while (file >> line)

		files.push_back(line);
	
	file.close();

    cout<<"filefholder  recover completely ...."<<std::endl;

    }


//set fileAdded begin idx
int main(){
     DiskScanner  diskscannaer;

     string filePath[1]= {"C:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore"};  

    for(string s :filePath)
        diskscannaer.setPath(s);

        // logical folder 
        fileFolder ffolder(diskscannaer);
 
        //cache
        ffolder.EnsureRecovery("C:\\BlobServiceData\\TestPartition\\cache.txt");


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