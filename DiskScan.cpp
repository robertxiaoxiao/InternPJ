#include <iostream>
#include <vector>
#include <stdio.h>
#include <io.h>
#include <iomanip>
#include <math.h>
#include "stdlib.h"
#include "Windows.h"
using  namespace  std  ;

void GetAllFiles( string path, std::vector<string>& files)  ;

//获取特定格式的文件名
void GetAllFormatFiles( string path, vector<string>& files,string format) ;

int main(){

    
	string filePath = "D:\\BlobServiceData\\TestPartition\\BlockBlob\\ChunkStore\\{E1B5B274-9511-4832-B8AF-4A8D4B826C8C}.ddp\\Data";  

	vector<string> files;  

 
	//读取所有的文件，包括子文件的文件
	//GetAllFiles(filePath, files);
 
	//读取所有格式为ccc的文件
	string format = ".ccc";
	GetAllFormatFiles(filePath, files,format);

    for(string s: files)
        cout<<s<<std::endl;

	return 0;

}

//获取所有的文件名
void GetAllFiles( string path, vector<string>& files)  
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
 
 
//获取特定格式的文件名
void GetAllFormatFiles( string path, vector<string>& files,string format)  
{  
	//文件句柄  
	long   hFile   =   0;  
	//文件信息  
	struct _finddata_t fileinfo;  

	string p;  

	if((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(),&fileinfo)) !=  -1)  
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
				files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
			}  
		}while(_findnext(hFile, &fileinfo)  == 0);  
 
		_findclose(hFile); 
	} 
} 
 
// 该函数有两个参数，第一个为路径字符串(string类型，最好为绝对路径)；
// 第二个参数为文件夹与文件名称存储变量(vector类型,引用传递)。



