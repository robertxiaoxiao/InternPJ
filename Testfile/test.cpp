#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <future>
#include<Windows.h>
#include <thread>
#include <functional>
#include <direct.h>
#include <io.h>

using namespace std;


int i=0;
struct inum
{
    int i;

    vector<int> list ;
    /* data */
};

inum geti(int i  ){

    inum temp={};

    temp.i=i;

    for(int i =0;i<5;i++)
      temp.list.push_back(i);


    return temp;

}

void print(){
    while(true)
       {  cout<< i++<<"hello    world\r\n"<<std::endl;
                    Sleep(50);
       }

}

void printI(){
    while(true)
            {  cout<< i++<<"thread exchange\r\n"<<std::endl;
                    Sleep(50);
            }

}

//线程池的回调函数
VOID WINAPI ThreadPoolCallBack(PTP_CALLBACK_INSTANCE instance, PVOID param)
{
    cout << "param:" << (int)param << "\tThread id = " << GetCurrentThreadId() << endl;
    Sleep(200); // 模拟一个任务时间为100毫秒的执行
    return;
}

DWORD GetNumOfProcess()// 获取CPU的核心数
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);                    // 获取操作系统信息
    return sysinfo.dwNumberOfProcessors;
}


#include<mutex>

std::mutex mtx;           

void print_thread_id (int id) {
  // critical section (exclusive access to std::cout signaled by locking mtx):
  mtx.lock();

  std::cout << "thread #" << id << '\n';
  mtx.unlock();
}



class A{

public :
    static int num ;

    static void staticinit(){
         cout<<num++<<std::endl;
    }

    void incr(){
        cout<<num++<<std::endl;
    }

};


LPCWSTR stringToLPCWSTR(std::string orig)
{
size_t origsize = orig.length() + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length()-1));
mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

return wcstring;
}


//static variety init
int A::num=1;

int main(){
     
    string dir="./hello";

if (access(dir.c_str(), 0) == -1)
	{
		cout<<dir<<" is not existing"<<endl;
		cout<<"now make it"<<endl;

		int flag=mkdir(dir.c_str()); 


		if (flag == 0)
		{
			cout<<"make successfully"<<endl;
		} else {
			cout<<"make errorly"<<endl;
		}

    }



 //string to LPCWSTR
    string filepath1="C:\\Users\\Administrator\\Desktop\\file_demo_test1.txt";


   
    DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    DWORD dwShareMode = 0;
    LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0;
    DWORD dwCreationDisposition = CREATE_NEW;
    DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    HANDLE hTemplateFile = 0;
    //(p.assign(dir).append("1").append(".ccc")
    HANDLE handle = CreateFile(stringToLPCWSTR(filepath1), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    printf("CreateFile -> HANDLE=0x%x\n", (unsigned int)handle);



//           std::thread threads[10];
//   // spawn 10 threads:
//   for (int i=0; i<10; ++i)
//         threads[i] = std::thread(print_thread_id,i+1);

//     for (auto& th : threads) th.join();
      
          
//  PTP_POOL tPool;
//     tPool = CreateThreadpool(NULL);             // 创建一个线程池

//     DWORD dwMaxThread = 3;                      // GetNumOfProcess() * 2 + 1;
//     //设置线程池参数（线程池中的线程数)
//     SetThreadpoolThreadMaximum(tPool, dwMaxThread); // 线程池中最多线程数
//     SetThreadpoolThreadMinimum(tPool, 1);       // 线程池中最少线程数

//     TP_CALLBACK_ENVIRON tcEnv;
//     InitializeThreadpoolEnvironment(&tcEnv);    // 初始化线程池的回调环境
//     SetThreadpoolCallbackPool(&tcEnv, tPool);   // 给线程池分配回调环境

//     cout << "线程池中的线程数为：" << dwMaxThread << endl << endl;
//     //测试例子
//     for (int i = 1;i < 20;i++)
//     {
//         // 向线程池中投递一个任务
//         TrySubmitThreadpoolCallback(ThreadPoolCallBack, (PVOID)i, &tcEnv);
//     }

        // std::thread  ts[10];

        // for(int i=0;i<10;i++)    
        //         ts[i]=thread(print,i);

        // auto fr0 = async([](){cout << "Welcome to async" << endl;});
        
        // for(int i=0;i<10;i++)  
        //         ts[i].join();
        // fr0.get();
        //  thread   t1(print);

        //  thread   t2(printI);
        //  t1.join();
        //  t2.join();
       
        // inum t=geti(5);
        // cout<< t.i<<std::endl;
        
        // for(int i : t.list)
        //     cout<< i<<std::endl;


        // char* t1="ab" ;
        // char* t2="ab" ;
        
        // char* t3="a" ;

        
        // cout<< strcmp(t1,t2)<<std::endl;
        
        // cout<< strcmp(t1,t3)<<std::endl;

 
 
 return 0;
    // std::vector<int>   list;
    // for(int i=0;i<10;i++)
    //   list.push_back(i) ;


    //  vector<int>&  l=list;
    //  int& i=list.at(5);
    //  cout<< i<<std::endl;

    //  l.erase(l.begin()+5);


    // cout<< i<<std::endl;

    
     




    //   int size=l.size();
    //   for(int i=0;i<10;i++)
    //        { res.push_back(inum{l.at(size-i-1)});
    //         cout<< l.at(size-i-1)<<std::endl;
    //        }

    //     cout<< res.size()<<std::endl;
   

}