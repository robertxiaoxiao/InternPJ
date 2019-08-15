### ChunkStoreMDS 

```c++
/* 主要职责：维护一个chunkstore文件表
  实现的功能：
   (1) 	 初始化 folder信息，扫描物理磁盘获得chunkstore信息
   （2）   提供两个基本操作: 
    	1  markOwned() ;
   	    2  根据一定策略 分配Filelist findFileList() ;
    (3) 接受client读写过程信息
   （4）内部表状态恢复（client crash恢复请求）
   	(5) 内部表磁盘恢复(自己crash掉从磁盘重新初始化)
   （5）实现对表的基本操作,实现类似os folder中的一些操作
*/


}

ChunkStoreMDS Manager  设置管理ChunkStoreMDS并保证单例
```

### ClientSvc

```c++
/*
   主要职责 ： 提供chunkStore File信息的供应支持
   实现的功能：
   （1）初始化  文件列表动态增加
   （2）insertchunkstore()   文件标记
        内部采用多线程写，单元测试模拟多个节点访问情况
   （3）sizeSensor(),态感知状态 需要请求的文件数  定期执行
   (4) 强制请求文件列表，当负载过大的时候自动进行执行，兼容sensor()
   (5) 请求文件 to MDS ,
   （6）clear（） 清除已写文件
   (7)  留接口集成现有的直接写入数据  
     if(false) {
     
     //     primary code
     }else {
     
        //  new code 
           
     }
*/
```



### FileContainerTable 

```c++
/*
使用vector来存储
table parameters 
*/

//filapath
	  long 	    pathname ,
// using client id 
	  long      owener ,
// check-in and check out flag 
      boolean   using  ,

// file info 
Struct fileinfo{

    File size ;
    
    //可以开始写
    long offset;
}
```

###  Test Case

```c++
class ChunkStoreMDS{
 
} ;

class  ClientSvc{
    
} ;

int main(){
    
    singleton ChunkStoreMDS();
    
    void unittestCall();
    
    ClientSvc[]  clients;
    void thread(task ,&ChunkStoreMDS){
          runtask();
    }
    
    threads.join();
    
    assert(client) ;
    
    assert(ChunkStoreMDS);
    
    killClient();
    recoverClient();
    
    killChunkStoreMDS():
    recoverChunkStoreMDS();

}

```

当前算法 ：
      1. 要文件是根据rate要的，可以配置
      2. 增加文件都是从filesize末尾增加的，askfiles更新时直接从上一次的末尾更新
      3. restfiles  usingfiles分离读写，更倾向于restfiles分配文件，更新的时候更倾向于usingfiles 还可以更优化、


加锁并发分析:
      当前并发资源     restfilelist    usingfilelist  



      askingflie()


      mds   sensecapicilty ;











