#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <future>
#include<Windows.h>
#include <thread>
//#include "ChunkStoreMDS.cpp"
using  namespace std ;



// class A{

//    public:

//      int i =0;

//      void sayhello(){
//             // auto incre
        
//             while(true)
//                 {
//                     std::cout<<"hello  ,world"<<i++<<std::endl;

//                     Sleep(2000);
//                 }
//    }
// };

class ChunkStoreMDSManager
{

public:


     // static ChunkStoreMDS csMDS;
   // static ChunkStoreMDS   MDS;

 static  A a ;
    void autoAskFiles();



    //
    // bool Caa::init()
    // {
    //     // 设置回调函数
    //     // 需要将成员函数的this指针传入
    //     // 需要将类成员函数地址强制转换为回调函数类型
    //     setCBTest((ptestCB)&Caa::onmyevent,this);
    //     return true;
    // }
    // // 回调函数
    // int Caa::onmyevent(testMsgType *  p,void* aa)
    // {
    //     // 将回调回来的指针强制转换为类指针，然后调用类的成员函数
    //     ((CEventMgmt*)aa)->dealCB(p);
    //     return 0;
    // }
    // // 业务处理函数
    // int Caa::dealCB(testMsgType* p)
    // {
    //       return 0;
};


A ChunkStoreMDSManager::a;


// using windowsAPI timer 
VOID   CALLBACK   TimerProc(HWND   hwnd,UINT   uMsg,UINT   idEvent,DWORD   dwTime); 

VOID   CALLBACK   TimerProc(HWND   hwnd,UINT   uMsg,UINT   idEvent,DWORD   dwTime) 

{ 
      ChunkStoreMDSManager m;
      m.a.sayhello();
} 


// new threa autoAskFiles ;

void ChunkStoreMDSManager::autoAskFiles(){
    
    int timer1 = 1;
    HWND hwndTimer;
    MSG msg;

    // 调用另一个对象的方法
    
    // SetTimer(NULL, timer1, 2000, TimerProc);

    // int itemp;

    // while ((itemp = GetMessage(&msg, NULL, NULL, NULL)) && (itemp != 0) && (-1 != itemp))
    // {
    //     if (msg.message == WM_TIMER)
    //     {
    //         std::cout << "i  got  the  message " << std::endl;
    //         TranslateMessage(&msg);
    //         DispatchMessage(&msg);
    //     }
    // }
   
  
}

void print(){

while(true){
    cout<<"hello  exhcange"<<std::endl;
    Sleep(4000);
}
}


int main()
{
   
     A  a;

     thread   t(&A::sayhello,a);
     
     std::thread  t3(print);
     t.join();

  
   // std::thread  t(&ChunkStoreMDSManager::autoAskFiles,m);

  //  t.join();
  

    t3.join();


    
    return 0;
}
