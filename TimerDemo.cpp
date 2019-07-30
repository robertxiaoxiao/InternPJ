#include  <Windows.h>
#include <iostream>
#include <stdio.h>

using namespace std;
DWORD  WINAPI mytimernull(LPVOID args)
{
    BOOL bRet = FALSE;
    MSG msg = { 0 };
    
    //步骤一:创建定时器
    //如果hWnd为NULL，返回值为新建立的timer的ID
    UINT timerid1 = SetTimer(NULL, 0, 1000, NULL);
    UINT timerid2 = SetTimer(NULL, 0, 3000, NULL);

    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        { 
            // handle the error and possibly exit 
        }
        else
        {
             //timer消息到达
             //步骤二:定时器处理程序
            if (msg.message == WM_TIMER)
            {
                //指定的timer 到达
                if (msg.wParam == timerid1)
                {
                    printf("timerid1 reaching ...\n");
                }

                //指定的timer 到达
                if (msg.wParam == timerid2)
                {
                    printf("timerid2 reaching ...\n\n\n");
                }
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    //步骤三:定时器销毁
    KillTimer(NULL, timerid1);
    KillTimer(NULL, timerid2);

    return 0;
}


//定时器使用方式二
//步骤二:定时器处理程序
void CALLBACK Timerproc (
HWND hwnd,        // handle to window for timer messages 
UINT message,     // WM_TIMER message 
UINT idTimer,     // timer identifier 
DWORD dwTime     // current system time 
)
{
    printf("timerid3 reaching ...\n");
}


//每5秒开始执行
DWORD  WINAPI mytimerproc(LPVOID args)
{
     BOOL bRet = FALSE;
     MSG msg = { 0 };
     //步骤一:创建定时器
     //如果hWnd为NULL，返回值为新建立的timer的ID
     UINT timerid3 = SetTimer(NULL, 0, 5000, (TIMERPROC)Timerproc);

     while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
     {
         if (bRet == -1)
         {
             // handle the error and possibly exit 
         }
         else
         {
             //将定时器消息传递给so消息队列,这样才能触发Timerproc执行
             TranslateMessage(&msg);
             DispatchMessage(&msg);
         }
     }
     
     //步骤三：销毁定时器
     KillTimer(NULL, timerid3);
     return 0;
 }

int main() 
{

    try
    {
        HANDLE handle1 = (HANDLE)::CreateThread(NULL, 0, mytimernull, NULL, 0, NULL);

        HANDLE handle2 = (HANDLE)::CreateThread(NULL, 0, mytimerproc, NULL, 0, NULL);

        HANDLE hArray[2] = { handle1 ,handle2 };
        WaitForMultipleObjects(2, hArray, TRUE, INFINITE);

        CloseHandle(handle1);
        CloseHandle(handle2);
    }
    catch (...)
    {
        printf("MonitorTeachingSoftPlayer error...\n");
    }

	return 0;
}
