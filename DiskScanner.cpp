
#include <iostream>
#include <vector>
#include <stdio.h>
#include <io.h>
#include <iomanip>
#include <math.h>
#include "stdlib.h"
#include <string>
#include "Windows.h"
#include <fstream>

#include "FileInfoScan.cpp"
#include "DiskScanner.h"


using namespace std;
/*
   implemtns the func of os oprations  of specific folder 
 */

// 配置filefolder扫描地址  ，第一个为默认添加文件地址
void DiskScanner::setPath(string s){
        defaultPath.push_back(s);
}


DiskScanner::DiskScanner(){

}

 DiskScanner::~DiskScanner(){
     defaultPath.clear();

}


void createFileInternal(string path){
	/* 
		test just for uniplemented method  and  integrated with chunkstore.createfile(filepath)

		std::ofstream location_out;
		location_out.open(path, std::ios::out | std::ios::app);
		cout<<"cretae file :"<<path<<std::endl;
		*/
}


//构造更多的文件
void DiskScanner::createFiles(int num , std::vector<string>& files){

			string  format=".ccc" ;

	
			string volume ="D:\\addedFileFolder\\" ;
			
			// defaultPath[0]  addedfilefolder
			if(defaultPath.size()!=0)
					volume=defaultPath.at(0);

			string p;
	
			for(int i=0;i<num;i++)
				{	p.assign(volume).append("\\"+std::to_string(i+curAddedFilenum)+format);
					files.push_back(p);
					createFileInternal(p);
				}

			curAddedFilenum+=num;
			
			// call chunkStore bld ;
			//	
			// create a new container
            // std::wstring containerPath = CChunkStoreContainer::FormContainerPath(
            //     m_storePath,
            //     nextContainerId,
            //     DefaultGeneration);
			/*

			ft.hr = CDedupFileFactory::Create(
                                    tempContainerPath.c_str(),
                                    GENERIC_READ | GENERIC_WRITE | DELETE,
                                    FILE_SHARE_READ,
                                    NULL,
                                    CREATE_ALWAYS,
#ifdef WOSS
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH,
#else
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_NO_BUFFERING,
#endif
                                    NULL,
                                    m_createFileContext,
                                    &tempContainerFile,
                                    m_isSecondaryPartition);
			 */

}


//获取所有的文件名
void DiskScanner::GetAllFilesInternal( string path, vector<string>& files)  
{  
 
	long   hFile   =   0;  
	//文件信息  
	struct _finddata_t fileinfo;  

	string p;  
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  
	{  
		do  
		{   
			if((fileinfo.attrib &  _A_SUBDIR))  
			{  
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
					GetAllFilesInternal( p.assign(path).append("\\").append(fileinfo.name), files ); 
				}
			}  
			else  
			{  
					files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
			} 
 
		}while(_findnext(hFile, &fileinfo)  == 0);  
 
		_findclose(hFile); 
	} 
 
}  
 
bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

//获取特定格式的文件名
void DiskScanner::GetAllFormatFilesInternal( string path, vector<string>& files,string format)  
{  
	//文件句柄  
	long   hFile   =   0;  
	//文件信息  
	struct _finddata_t fileinfo;  

	string p;  

	if((hFile = _findfirst(p.assign(path).append("\\*" ).c_str(),&fileinfo)) !=  -1)  
	{  
		do  
		{    
			if((fileinfo.attrib &  _A_SUBDIR))  
			{  
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
					GetAllFormatFilesInternal( p.assign(path).append("\\").append(fileinfo.name), files,format); 
				}
			}  
			else  
			{  
				if(hasEnding(p.assign(fileinfo.name),format))
							files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
			}  
		}while(_findnext(hFile, &fileinfo)  == 0);  
 
		_findclose(hFile); 
	} 
} 


void DiskScanner::GetAllFiles(vector<string>& files){

	for(string path:defaultPath)
			GetAllFilesInternal(path,files);

}

void DiskScanner::GetAllFormatFiles(vector<string>& files,string format){

	for(string path:defaultPath)
			GetAllFormatFilesInternal(path,files,format);

}
 


// int DiskScanner::curAddedFilenum=0;
// // test 
// int main(){

//     // for test 
//   //  string filePath[2]= {"D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore","C:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore\\{EDD7F05F-1349-49E0-9924-14E8275D98AD}.ddp\\Data"};  

//  	string filePath[3]={"D:\\BlobServiceData","D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore","D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore\\{E1B5B274-9511-4832-B8AF-4A8D4B826C8C}.ddp\\Data"};

//     std::cout<<"test starts ......"<<std::endl;
    
//     //FileInfoScan  scanner=DiskScanner();
// 	DiskScanner  scanner;

//     // for(string s :filePath)
//    	// 	 scanner.setPath(s);


// 	vector<string> files;  


// 	scanner.createFiles(5,files);
 
// 	//读取所有的文件，包括子文件的文件
// 	//GetAllFiles(filePath, files);
 
// 	//读取所有格式为ccc的文件
// 	string format = ".ccc";


// 	// added twice
// 	scanner.createFiles(5,files);
	

// 	cout<<"added file over "<<std::endl;

//   	scanner.GetAllFormatFiles(files,format);
	
	
//     for(string s: files)
//         cout<<s<<std::endl;

// 	return 0;
     

// }
 
