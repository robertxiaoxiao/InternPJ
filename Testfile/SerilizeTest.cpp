#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

#include <stdlib.h>
using namespace std;

struct fileInfo
{

	// filename unique
	string filepath;

	//  check free and ctrl file access
	bool Beusing;

	// help to recover client's filelist info
	char *owner;

	// writen size  for balance
	long offset;
};


void parsefileInfo(string line,vector<string>& parseresult) ;
//写文件
void WriteFile()
{
	ofstream file("./text.txt", ios::out);
	if (!file.is_open())
	{
		cout << "文件打开失败" << endl;
		return;
	}
	for (int i = 0; i < 10; i++)
	{
	//	fileInfo temp = {"aaaaaaaaaaa", false, "fzy", 0};
	//	file << temp.filepath << "-" << temp.Beusing << "-" << temp.owner << "-" << temp.offset << "-"<<"q"<<std::endl;
				string  temp="yvetee";
	 			file<<temp<<std::endl;
	}
	file.close();
	return;
}
void ReadFile(vector<string>& parseresult)
{
	ifstream file("./text.txt", ios::in);
	if (!file.is_open())
	{
		cout << "文件打开失败" << endl;
		return;
	}

 	string line;

	//读取文件3种方式
	//1、read  file.eof() 作为判断条件 会慢一拍
	while (file >> line)
	//while (!file.eof())
	{
		//file.read(temp, 1024); //这样会读取到\n
		//cout << temp
		parsefileInfo(line,parseresult);
		// >>按行读取，不会读换行符
		//cout << temp << endl;
	}
	file.close();
}


void  stringreadFile(vector<string>& parseresult){
	ifstream file("./text.txt", ios::in);
	if (!file.is_open())
	{
		cout << "文件打开失败" << endl;
		return;
	}

 	string line;

	//读取文件3种方式
	//1、read  file.eof() 作为判断条件 会慢一拍
	while (file >> line)
	//while (!file.eof())
	{
		//file.read(temp, 1024); //这样会读取到\n
		//cout << temp
		parseresult.push_back(line);
		// >>按行读取，不会读换行符
		//cout << temp << endl;
	}
	file.close();
}

void parsefileInfo(string str,vector<string>& parseresult){
		 
		 int beginidx=0;
		 int pos=0;
		 int size=str.size();

		// if str is null retrun ;
		 for(int i =0;i<size;i++)
		 {
			
		cout<<"current parsed char : "<<str[i]<<std::endl;
         
			if(str[i]=='-')
			{	 
				pos=i;
	
		  		std::string s=str.substr(beginidx,i-beginidx);

				parseresult.push_back(s);
            	//cout<<s<<std::endl;
         
				beginidx=pos+1;
			}

			if(str[i]=='q')
					return;
		 }
}
int main()
{
	vector<string> parseresult;

	WriteFile();
	stringreadFile(parseresult);
	for(string s :parseresult)
			cout<<s<<std::endl;
	// vector<fileInfo>  result;

	// while(!parseresult.empty())
	// 	{
	// 	  // format path ,beusing ,owner ,offset
	// 	   long offset=atoi(parseresult.back().c_str());
	// 	   parseresult.pop_back();

	// 	   char* owner=const_cast<char*>(parseresult.back().c_str());
	// 	   parseresult.pop_back();

	// 	 	bool beusing=parseresult.back().compare("1")==0?true:false;
	// 		parseresult.pop_back();

	// 		string path=parseresult.back();
	// 		parseresult.pop_back();

	// 		fileInfo  temp={path,beusing,owner,offset} ;
	// 		result.push_back(temp);
	// 	}

	// for(fileInfo temp :result)
	// 		cout<<temp.filepath << "-" << temp.Beusing << "-" << temp.owner << "-" << temp.offset <<std::endl;
	// //string s="a-0-fzy-0-q";
	// 		//	 3 4  7	

	// fileInfo  temp={"fyh",true,"1111",111};
	// 		cout<<temp.filepath << "-" << temp.Beusing << "-" << temp.owner << "-" << temp.offset <<std::endl;	
	// cout<<s.size()<<std::endl;
	// cout<<s.substr(0,1)<<std::endl;

	// 	// substr (begin_idex,len) 
	// cout<<s.substr(4,3)<<std::endl;

    // cout<<s.substr(4,6)<<std::endl;	
	// cout<<s.substr(4,7)<<std::endl;

	//parse(s);
}
