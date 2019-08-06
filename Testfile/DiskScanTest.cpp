#include <iostream>
#include <vector>
#include <stdio.h>
#include <io.h>
#include <iomanip>
#include <math.h>
#include "stdlib.h"
#include "Windows.h"
#include <fstream>
#include <string>


using  namespace  std  ;

void GetAllFiles( string path, std::vector<string>& files)  ;

//获取特定格式的文件名
void GetAllFormatFiles( string path, vector<string>& files,string format) ;


void createFile(string path);

int main(){

    
	string volume = "D:\\BlobServiceData";  


	string p;
	for(int i=0;i<5;i++)
		{
			createFile(p.assign(volume).append("\\"+std::to_string(i)+".ccc"));

		}

// 	vector<string> files;  

 
// 	//读取所有的文件，包括子文件的文件
// 	//GetAllFiles(filePath, files);
 
// 	//读取所有格式为ccc的文件
// //	string format = ".ccc";

// //	GetAllFiles(filePath,files) ;
// //	GetAllFormatFiles(filePath, files,format);

//     for(string s: files)
//         cout<<s<<std::endl;

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
 
 
 bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
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
 
// 该函数有两个参数，第一个为路径字符串(string类型，最好为绝对路径)；
// 第二个参数为文件夹与文件名称存储变量(vector类型,引用传递)。




void createFile(string path,string format){

		std::ofstream location_out;
		location_out.open(path, std::ios::out | std::ios::app);
		cout<<"cretae file :"<<path<<std::endl;
}
