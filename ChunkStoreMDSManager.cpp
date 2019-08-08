#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <future>
#include <Windows.h>
#include <thread>
#include "ChunkStoreMDS.cpp"
//#include "ChunkStoreMDS.cpp"
using namespace std;

class ChunkStoreMDSManager
{

public:
  // static ChunkStoreMDS csMDS;
  // static ChunkStoreMDS   MDS;

  ChunkStoreMDS *MDS;

  void bind(ChunkStoreMDS *mds);

  // add files times
  void autoAskFiles(ChunkStoreMDS* MDS ,int duration );
};

void ChunkStoreMDSManager::bind(ChunkStoreMDS *mds)
{

  MDS = mds;

  cout << "binding mds ..." << std::endl;
};


int i=5;
// enable  new thread to  call autoAskFiles ;
void ChunkStoreMDSManager::autoAskFiles(ChunkStoreMDS* MDS ,int duration)
{

  while (true)
  {
     cout << "hello world ..." << std::endl;  

     _sleep(3000);
   

    // this_thread::sleep_for(chrono::milliseconds(1000));
    
    if (MDS == nullptr)
    {
      cout << "binding mds failed..." << std::endl;
      return;
    }

    (*MDS).AskMoreFilesTimely();

    cout << "auto adding files ..." << std::endl;

    
  }
}

// compile cmd  g++  C:\Users\t-zhfu\Documents\InternPJ\ChunkStoreMDSManager.cpp    C:\Users\t-zhfu\Documents\InternPJ\DiskScanner.cpp  -o test

int main()
{
  ChunkStoreMDS MDS;

  MDS.staticInit();

  ChunkStoreMDSManager manager;

  manager.bind(&MDS);

  (*manager.MDS).AskMoreFilesTimely();

  // cannot  ensure automatic   and it must  be called by a new thread  but not main thread 
  thread  t1(&ChunkStoreMDSManager::autoAskFiles,manager,&MDS,2);
 
  thread  t2(&ChunkStoreMDS::printState,manager.MDS);


  // mock the data ops time 
  Sleep(10000);
  //std::thread t1(&ChunkStoreMDSManager::autoAskFiles, manager, 1);

 thread  t3(&ChunkStoreMDS::printState,manager.MDS);
 
  t1.join();
  t2.join();
  t3.join();
  // t1.detach();

  //(*manager.MDS).printState();

  // std::thread  t(&ChunkStoreMDSManager::autoAskFiles,m);

  //  t.join();

  return 0;
}
