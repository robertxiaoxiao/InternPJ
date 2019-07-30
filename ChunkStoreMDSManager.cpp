
#include <windows.h>
#include <threadpoolapiset.h>
#include <iostream>

class ChunkStoreMDSManager
{

public:


 //  ChunkStoreMDS csMDS;

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



VOID   CALLBACK   TimerProc(HWND   hwnd,UINT   uMsg,UINT   idEvent,DWORD   dwTime); 
VOID   CALLBACK   TimerProc(HWND   hwnd,UINT   uMsg,UINT   idEvent,DWORD   dwTime) 

{ 
std::cout   <<   "hello "   <<   std::endl; 
} 


int main()
{
   
         int timer1 = 1;
         HWND hwndTimer;
         MSG msg;


    // 调用另一个对象的方法
    
     SetTimer(NULL, timer1, 5000, TimerProc);

      int itemp;

    while ((itemp = GetMessage(&msg, NULL, NULL, NULL)) && (itemp != 0) && (-1 != itemp))
    {
        if (msg.message == WM_TIMER)
        {
            std::cout << "i  got  the  message " << std::endl;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
  


    return 0;
}
