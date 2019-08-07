#include <iostream>
#include <string>

using namespace std;

struct fileinfo
{	
	 int id;
	/* data */
	string name;
};


// int Serialize(const fileinfo& finfo ,char output[])
// {
// 		int count=0;
// 		memcpy(output,&finfo.id,4);
// 		count+=4;

// 	memcpy(output,finfo.name.c_str(),finfo.name.length());
// 	count+=finfo.name.length();
// 	return count;
// }

// int Deserialize(fileinfo & s,const char* input ,int count)
// {
// 		int offset=0;
// 		memcpy(&s.id,input,4);
// 		offset+=4;
// 		s.name.append(input+offset,count-offset);
// 		return 0;

// }

int main(){

	fileinfo  s1={11,"fzy"};
	fileinfo s2=fileinfo(s1);

	cout<<&s1<<endl;
	cout<<&s2<<endl;

	cout<<s1.name<<endl;
	cout<<s2.name<<endl;

}
