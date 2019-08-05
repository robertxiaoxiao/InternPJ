ChunkStore扩展设计文档 

#### 一  架构图



###    需要描述的过程     MDSclient  MDSserver ChunkStore之间的交互，对外暴露的接口，交互的过程，实现的功能。

###  描述细节 主要的模块   

架构图描述  暴露的接口跟过程

chunkStore对磁盘文件的操作 都变成对MDSclient的调用....

数据记录表（）

request_retrun 











##### 1.1当前架构

​		  当前架构如下图所示， 客户端持有Blbclient ， 通过rpc调用BlbServer的服务，每一次调用Blbserver启动一个Blbsvc进程来处理请求，每一个Blbsvc包含chunkStoreLibrary，所有的chunkstore路径被Blbsvc保存在本地进程，并实现对应的读写服务。由于ChunkStore可以无限写直到磁盘被写满（在物理磁盘允许的情况下），可能导致系统管理的磁盘上，不同的ChunkStore中存储数据大小产生较大差异。

![1564971197589](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1564971197589.png)

##### 1.2 设计架构

​	  	添加ChunkStoreMDS模块后，架构变成如下图所示。每个Client先通过MDSclient通过rpc调用MDSsvc,

请求需要数量的可写入的ChunkStore文件路径集合，根据返回的文件路径集合，通过调用Blbclient中暴露的chunkStoreLibrary相关接口来初始化对应的ChunkStore，并进行相关的insert(),read(),GC(),delete()操作。

​	     ChunkStoreMDS模块提供BalanceStore,StoreRecovery,ManageFolder支持，ManagerFolder暴露统一的FolderInfo接口来进行操作，FolderInfo接口可以被扩展为DatabaseManager（基于数据库获取文件目录信息）和DiskManager(在物理磁盘上扫描获取文件目录信息)。

![1564971753670](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1564971753670.png)

### 二 概要设计

##### 2.1系统流程说明

######	2.1.1 设计流程 ：

​		客户端持有Blbclient和MDSclient，通过rpc调用ChunkStoreMDS，得到可写的文件目录集合信息之后，通过调用Blbclient提供的chunkstore接口。

​	  （1）实现chunkstore的实例化

​	   （2）写数据，根据chunkStore实例进行写入操作（直接写入磁盘），在写入完成之后MDSclient记录写入文件相关信息（filepath,offset,filesize...），并且通过rpc调用ChunkStoreMDS进行路径信息的更新和读写记录。

​	  （3）读数据，根据chukStore实例进行读数据，当前是将实例中持有过的文件路径记录在本地文件上（存在读取的文件路径信息已经返回给ChunkStoreMDS的情况），根据对应的文件路径确定后进行文件读取。

######	2.2.2 Read 流程：

  	   当前Blbsvc本地持有chunkstore实例读写过的文件路径，当加入chunkStoreMDS之后，存在可能把文件路径返还的情况（测试可做简化处理，将读写过的文件路径写入磁盘文件上，直接在磁盘文件上查找，现有的chunkstore.read()是在当前目录下持有路径中查找，需要扩展store->ReadChunk()方法)。流程图如下图所示。

![1564972841695](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1564972841695.png)

###### 2.2.3 Write流程：

​		 MDSclient直接RPC调用 chunkstoreMDS获得可写文件地址，内部进行文件目录的管理和分配（持有文件目录集合），完成写操作之后需要返回给ChunkStoreMDS更新文件目录信息，同时需要在本地记录下写过的文件路径信息。流程图入下图所示。

![1564973052168](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1564973052168.png)

###### 2.2.4 测试的流程：

​			（1）chunkstoreMDS初始化效率测试，例：256个文件夹下的10000个文件信息初始化。

​			（2）并发测试，例： 256个线程并发call chunkStoreMDS模拟多个节点不同call ,单个线程中设置call频率和模拟读写。

​			（3）chunkStoreMDS单元测试。

​			（4）结合chunkStoreViewer集成测试读写(需要根据新逻辑扩展chunkstore部分方法);

##### 2.2 原型测试设计

​		由于ChunkStoreMDS是一个必须满足分布式架构的系统，故而在Prototype的单机测试中，采用多线程来模拟不同的节点访问，来测试在并发情况下的表现。ChunkStoreMDS本身维护的是一个文件路径的集合（“.ccc“格式的文件路径），系统所实现的目标就是根据对应的chunkstore信息（在系统中体现为包含文件路径的文件信息结构体），进行文件路径（chunkstore初始化所必须的参数）的统一分发和管理。

##### 2.3 扩展方法说明

```c++
/*
	当前chunkStore的ReadChunk(),InsertChunk()方法主要实现由			   			CChunkStore::FindContainerForTransaction(_in newchunkSize,__out 					container);
		EnsureContainersLoadedUnsafe();
		EnumerateContainers(storepath,Allcontainers)
		当前来枚举container过程中，如果碰到可写文件不够的情况是直接BlbcreateFile()调用winapi....
*/
```

​		主要涉及到cchunkStore中跟磁盘文件操作的部分，因此加入MDS模块之后只需要扩展chunkStore的方法 FindContainerForTranction()来实现文件的返还和读写由MDS模块控制。

​		a. 当前读方法：直接指定对应磁盘读写.... init  chunkstore ，FindContainer(),container->openforread(storepath,containerid,generation)然后正常读。

​		b .当前写方法 ： (1) session查找(chunkstoreSession由storeID初始化 )

​								 	(2) 找不到合适调用就创建一个container，FindnewContainerForTranction（new chunksize,new container）

​		增加MDS扩展后方法：

​		c.扩展读方法：先查看session中是否有要读取的文件（container），如果没有则需要查看本地文件(单节点中已经读写过的文件集合)，如果本地文件没有则读失败。

​		d. 扩展写方法： 先查看session中是否有可读写的文件，如果找不到合适的可写文件，则向MDSSvs请求一批可写的新文件，（并将已写文件异步返回以便MDSsvc更新），需要在本地记录该节点读写过的文件路径信息（测试采用写本地文件的形式记录）。

​		 扩展方法中需要关注的类  chunkStoresession ，cchunkStore 。

##### 2.3  类与接口设计  

| Class  name          | Function                                                     |
| -------------------- | ------------------------------------------------------------ |
| FileInfoScan         | 接口，暴露抽象的获取文件路径集合方法和recovery方法           |
| DiskScan             | 实现封装os提供的操作磁盘文件的方法，初始化和用于crash后恢复ChunkStoreMDS信息，用于chunkStore物理磁盘的扩展实现 |
| ChunkStoreMDS        | 管理文件信息的请求和分发（需要支持并发）                     |
| ChunkStoreMDSManager | 实现平衡策略和chunkstoreMDSRecovery                          |
| MDSClient            | 实现节点与ChunkStoreMDS的交互，和读写过文件的本地记录        |
| BlbClient            | 实现节点的读写过程                                           |
| FileHolder           | 维护一个逻辑文件夹（包含不同磁盘的文件），提供类似于os的文件夹集合操作（增删改查） |
| TestMDS              | 用于原型测试                                                 |

##### 2.4  数据设计

```c++
// 文件信息  
struct fileInfo{
      // filename unique
        string filepath;
    
        //  check free and ctrl file access
        bool Beusing;

        // help to recover client's filelist info
        char *owner;

        // writen size  for balance 
        long offset;
}
//客户端请求信息
struct Request_MSG{
        char *owner;
        int fileNum;
        vector<fileInfo>  writenFilesinfo;
}

//客户端返回信息
struct Return_MSG{
     char *owner;    
     vector<fileInfo> files;
}
// 平衡策略
typedef enum
{
        UNKNOWN_OPERATION = 0,
        STORAGE_SIZE = 1    
        //to add more policy
} BALANCE_POLICY;

//客户端恢复信息
struct   client_Recovery_MSG{
        char *owner;
        vector<fileInfo> ownedFileList;
};

```

##### 2.5 关键函数设计

| Public API（一）                                             | Function                           |
| ------------------------------------------------------------ | :--------------------------------- |
| DiskScan::GetAllFiles( string path, vector<string>& files)   | 获取指定磁盘的全部文件信息         |
| DiskScan::GetAllFormatFiles( string path, vector<string>& files,string format) | 获取指定磁盘的指定格式文件信息     |
| DiskScan::Recover(string path,vector<fileInfo>& files)       | MDS crash后恢复文件路径信息        |
| fileFolder::listFiles()                                      | 列出当前文件夹中的所有文件路径信息 |
| fileFolder::createMoreFile()                                 | 创建更多的chunkstore文件路径信息   |
| fileFolder::addFile(string s)                                | 增加指定路径                       |
| fileFolder::checkFileContained(string ts)                    | 检测是否存在指定文件路径信息       |
| fileFolder::deleteFile(string ts)                            | 删除指定路径                       |
| ChunkStoreMDS::printState()                                  | 打印当前MDS 信息                   |

| Public  API（二）                                            | Function                                           |
| ------------------------------------------------------------ | -------------------------------------------------- |
| ChunkStoreMDS:: markOwned(string filename, char *owner);     | 标记文件路径持有人                                 |
| ChunkStoreMDS::deliverFiles(Request_MSG msg, vector<fileInfo> &ownerFileList, BALANCE_POLICY policy); | 按照特定策略分发文件路径                           |
| ChunkStoreMDS::senseCapcity(double restrate);                | 感知文件目录使用状态                               |
| ChunkStoreMDS::AskMoreFilesTimely();                         | 定时申请更多可写文件路径                           |
| ChunkStoreMDS::AskMoreFilesForcibly(int rate)；              | 强制申请更多可写文件路径                           |
| ChunkStoreMDS::updateFilelist(Return_MSG msg);               | 根据客户端信息更新文件目录                         |
| ChunkStoreMDS:: Client_Revocer_Help(char *owner);            | 恢复客户端持有文件路径集合信息（客户端 crash重启） |
| ChunkStoreMDSManager::AutoAskingFile();                      | 定时配置更新文件目录（用于测试）                   |
| ChunkStoreMDSManager::Recovery();                            | MDS Crash后恢复文件路径信息（测试）                |

| Test  API( 三 )               | Function                                          |
| ----------------------------- | ------------------------------------------------- |
| MDSClient::RequestFile()      | 模拟请求文件路径                                  |
| MDSClient::ReturnFile()       | 模拟返回读写文件信息                              |
| MDSClient::ParseLocalFile()   | 模拟本地文件目录信息解析操作（用于测试读）        |
| MDSClient::PersistLocalFile() | 本地文件目录信息记录                              |
| MDSClient::Request_Recovery() | 恢复本地文件目录信息（用于MDSclient crash后恢复） |
| BlbClient::initChunkStore()   | 模拟节点初始化chunkStore                          |
| BlbClient::read()             | 模拟节点读取操作                                  |
| BlbClient::insert()           | 模拟节点插入操作                                  |

##### 2.5  测试过程

 		a. 单元测试

```c++
 int main(){
	 //init file path  DiskScan imp FiliInfoScan
	 DiskScan  scanner("D:/");
     // init logical folder
	 fileFolder ffolder(scanner.GetAllFormatFiles(p1,p2,p3));
     
     //inti  ChunkStoreMDS
     ChunkStoreMDS  MDS(ffolder);
     
     // internal static init
     MDS.staticInit(baancePolicy , asking rate,.....);
     
     // init ChunkStoreMDSManager
     ChunkStoreMDSManager  m(MDS);
     
     // start auto askingfiles to fulfill the needs,impled by timer(once/min)
     thread(m.autoAskFiles()).join();
     
     // test MDS functions
     m.MDS.printstate();
     
     m.MDS.AskMoreFilesForcibly(int rate)；
         
     m.MDS.AskMoreFilesTimely();
     /*
     		all other  functions;
     */
      
     //test multi-thread call to simulate the scenario that  many clients call
   	for(int i =0;i<size;i++)
    {
     	thread(MDSclient.RequestFiles(p1,p2,p3)).join();
        thread(MDSclient.ReturnFiles(p1,p2,p3)).join();
        
    }
     
     // MDSClient unit test
       MDSClient::PersistLocalFile();
       MDSClient::ParseLocalFile();
 	   MDSClient::Request_Recovery();
     
     // Blbclient  unit test
     
   		  BlbClient::initChunkStore();
   		  BlbClient::read();
   		  BlbClient::insert();
     
      //test MDS recovery
      m.Recovery();

 }
```

​		b. 集成测试

```c++
/*
	 Integration Testing with demo.cpp and chunkstoreviewer.cpp
*/
int main(){
    
    	//init demo
     //init file path
	 DiskScan  scanner("D:/");
     // init logical folder
	 fileFolder ffolder(scanner.GetAllFormatFiles(p1,p2,p3));
     
     //inti  ChunkStoreMDS
      ChunkStoreMDS  MDS(ffolder);
     
     // internal static init
      MDS.staticInit(baancePolicy , asking rate,.....);
     
     // init ChunkStoreMDSManager
      ChunkStoreMDSManager  m(MDS);
    
    // get file
    MDSclient    MDSclients[n]
      
    for(int i =0;i< n ;i++)
    {
        thread t(MDSclients[i].RequestFile());
        t1.join();
    
        thread t2(MDSclients[i].ReturnFile());
   		t2.join();
    }
     
         /*  bind chunkstore with the specific file path 
         curPath : 	"D:TEST/CHUNKSTORE/.../0000001.0000001.CCCC"  init by volume("D:")
         test Path : filepath ( "D:/chunkstore/1.ccc");
        */
    	 Blbclient.staticinit(filepath,....teststore)
    
         // test insert
          teststore.insert(p1,p2,..);
    	
    	// test  read
    	  teststore.read(p1,p2,...);
         
}
```

### 三 之后的工作

​		按照Intern Plan ，如果在此之前的ChunkStoreMDS在read() ,insert()方法上效果不错，接下来是考虑delete()和GC()的设计，并且已完成的demo的基础上进一步实现。







