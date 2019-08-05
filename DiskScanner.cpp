#include <iostream>
#include <vector>
#include <stdio.h>
#include <io.h>
#include <iomanip>
#include <math.h>
#include "stdlib.h"
#include <string>
#include "Windows.h"
#include "FileInfoScan.h"


using namespace std;
/*
   implemtns the func of os oprations  of specific folder 
 */
class DiskScanner: public FileInfoScan{


	private:

	// scan path
	std::vector<string>  defaultPath;

	
    public :

		static int filenum;


    DiskScanner();

    ~DiskScanner();

     //获取特定格式的文件
    void GetAllFiles( std::vector<string>& files)  ;

    //获取特定格式的文件名
    void GetAllFormatFiles(vector<string>& files,string format) ;


     //获取特定格式的文件
    void GetAllFiles( string path, std::vector<string>& files)  ;

    //获取特定格式的文件名
    void GetAllFormatFiles( string path, vector<string>& files,string format) ;

	// setting 

    //可配置chunkstore扫描路径
    void setPath(string s) ;


    //  在指定目录生成更多新文件
	void createFiles(int num , std::vector<string>& files);

                
};


void DiskScanner::setPath(string s){
        defaultPath.push_back(s);
}


 DiskScanner::DiskScanner(){

}

 DiskScanner::~DiskScanner(){
     defaultPath.clear();

}


//构造更多的文件
void DiskScanner::createFiles(int num , std::vector<string>& files){
				// call chunkStore bld ;

				

}



//获取所有的文件名
void DiskScanner::GetAllFiles( string path, vector<string>& files)  
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
					GetAllFiles( p.assign(path).append("\\").append(fileinfo.name), files ); 
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
void DiskScanner::GetAllFormatFiles( string path, vector<string>& files,string format)  
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
					GetAllFormatFiles( p.assign(path).append("\\").append(fileinfo.name), files,format); 
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
			GetAllFiles(path,files);

}

void DiskScanner::GetAllFormatFiles(vector<string>& files,string format){

	for(string path:defaultPath)
			GetAllFormatFiles(path,files,format);

}
 

// test 
int main(){

    // for test 
  //  string filePath[2]= {"D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore","C:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore\\{EDD7F05F-1349-49E0-9924-14E8275D98AD}.ddp\\Data"};  

 	string filePath[2]={"D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore","D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore\\{E1B5B274-9511-4832-B8AF-4A8D4B826C8C}.ddp\\Data"};

    std::cout<<"test starts ......"<<std::endl;
    
    //FileInfoScan  scanner=DiskScanner();
	DiskScanner  scanner;

    for(string s :filePath)
   		 scanner.setPath(s);


	vector<string> files;  
 
	//读取所有的文件，包括子文件的文件
	//GetAllFiles(filePath, files);
 
	//读取所有格式为ccc的文件
	string format = ".ccc";
	
  	scanner.GetAllFormatFiles(files,format);
	
	
    for(string s: files)
        cout<<s<<std::endl;

	return 0;
     

}
 
