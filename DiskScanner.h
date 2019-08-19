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


//千万不要忘记 这一句！！！！！！！
using namespace std;

class DiskScanner 
{
    public :

	static int curAddedFilenum;

    DiskScanner();

    ~DiskScanner();

     //获取特定格式的文件
    void GetAllFiles( std::vector<string>& files)  ;

    //获取特定格式的文件名
    void GetAllFormatFiles(vector<string>& files,string format) ;


     //获取特定格式的文件
    void GetAllFilesInternal( string path, std::vector<string>& files)  ;

    //获取特定格式的文件名
    void GetAllFormatFilesInternal( string path, vector<string>& files,string format) ;

	// setting 

    //可配置chunkstore扫描路径
    void setPath(string s) ;


    //  在指定目录生成更多新文件
	void createFiles(int num , std::vector<string>& files);


private:

	// scan path
	std::vector<std::string>  defaultPath;

};