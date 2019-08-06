ChunkStoreMDS设计文档 

#### 一  架构设计

##### 1.1当前架构

​		  当前架构如下图所示， 客户端持有Blbclient ， 通过rpc调用BlbServer的服务，每一次调用Blbserver启动一个Blbsvc进程来处理请求，每一个Blbsvc包含chunkStoreLibrary，所有的chunkstore路径被Blbsvc保存在本地进程，并实现对应的读写服务。由于ChunkStore可以无限写直到磁盘被写满（在物理磁盘允许的情况下），可能导致系统管理的磁盘上，不同的ChunkStore中存储数据大小产生较大差异。

![1564971197589](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1564971197589.png)

##### 1.2 设计架构

​	  	添加ChunkStoreMDS模块后，架构变成如下图所示。每个Client先通过MDSclient通过rpc调用MDSsvc,

请求需要数量的可读写的ChunkStore文件路径集合，根据返回的文件路径集合，通过调用Blbclient中暴露的chunkStoreLibrary相关接口来初始化对应的ChunkStore，并进行相关的insert(),read(),GC(),delete()操作。

​	     ChunkStoreMDS模块提供BalanceStore,StoreRecovery,ManageFolder支持，ManagerFolder暴露统一的FolderInfo接口来进行操作，FolderInfo接口可以被扩展为DatabaseManager（基于数据库获取文件目录信息）和DiskManager(在物理磁盘上扫描获取文件目录信息)。

![1565057765533](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1565057765533.png)

##### 1.3 关注点

​		设计架构主要是解决Blbsvc和shareFolder的强关联问题，即在原有架构下，当Blbsvc一旦初始化ChunkStore完成后，就与对应的shareFolder中的特定Folder绑定。解决的主要思路是添加chunkStoreMDS模块，MDSserver负责对shareFolder进行管理，并且提供必要的接口供MDSclient调用，并且扩展chunkstoreLibrary中的相关方法以满足Blbsvc需要。主要关注以下两个方面：

​		(1)MDSserver模块，主要由四部分构成，BalancePolicy,MDSsvc,Recovery,FileFolder。

​		(2)MDSclient模块。

##### 1.4  模块说明

###### 1.4.1 MDSserver模块

​		MDSserver模块主要负责组织和管理FileFolder，并且对外提供调用接口。

| Component     | Function                                                     |
| ------------- | ------------------------------------------------------------ |
| BalancePolicy | 提供组织管理Filefolder的平衡策略的配置和修改                 |
| MDSsvc        | 提供公用接口(Request_ReturnFiles)供MDSclient调用,并负责更新内部Fileinfo数据记录表 |
| Recovery      | 提供crash后恢复服务                                          |
| FileFolder    | 提供对shareFolder的操作抽象，负责统一维护内部Fileinfo数据记录表 |

###### 1.4.2 MDSclient模块

​		MDSclient模块主要是提供MDS服务请求接口，以及chunkstoreLibrary中部分方法扩展调用接口。

##### 1.5  模块接口设计

###### 1.5.1  Public Interface 设计

| Public Interface                                             | Function                                                     |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| MDSserver.Deliver_updateFiles（Req_client,fileListsize,writenFileList,returnFileList） | 主要负责MDSsvc分发更新文件信息，参数分别包括：（1）Req_client请求客户端（2）请求文件数目(3)已写文件列表，用于更新MDSserver端文件信息（4）根据策略返回请求的文件列表。 |
| MDSclient.Request_returnFiles(Req_server,fileListsize,writenFileList,returnFileList) | 主要负责MDSclient请求更新文件信息，参数分别包括：（1）Req_server请求服务端（2）请求文件数目(3)已写文件列表，用于更新MDSserver端文件信息（4）根据MDSserver策略返回请求的文件列表。 |
| MDSserver.FileFolder(FileListInfo)                           | 主要提供对shareFolder的统一抽象封装，提供一个类似于操作系统os的逻辑文件目录管理功能，参数主要是FileListInfo（chunkstore物理磁盘上的路径信息，可通过磁盘扫描，数据库查表来明确） |

###### 1.5.2 Internal Interface 设计

| Internal Interface          | Function                                                     |
| --------------------------- | ------------------------------------------------------------ |
| MDSserver.Recover()         | 发生crash后恢复本地数据记录表                                |
| MDSserver.Balance()         | 在与MDSclient和FileFolder的交互中平衡数据记录表（维持数据记录表在一个相对平衡的状态下） |
| MDSclient.getAvalibalFile() | 提供可读写的chunkstore路径文件信息（可用于扩展chunkStore现有方法） |
| MDSclient.Recover()         | 当MDSclient crash后恢复本地数据记录表                        |

##### 1.6系统流程说明

###### 1.6.1 设计流程 ：

​		客户端持有Blbclient和MDSclient，通过rpc调用MDSserver服务，得到可读写的文件路径集合之后，通过调用Blbclient提供的chunkstorelib接口实现对应操作，主要包括以下三个过程：

​	   （1）根据可读写文件路径实例化chunkstore

​	   （2）写数据，根据chunkStore实例进行写入操作（直接写入磁盘），在写入完成之后MDSclient记录写入文件相关信息（filepath,offset,filesize...），并且通过rpc调用ChunkStoreMDS进行路径信息的更新和读写记录,并在本地维护一个读写文件记录cache。

​	   （3）读数据，根据chukStore实例进行读数据，在本地维持一个读写文件cache（存在读写过的文件路径信息已经返回给ChunkStoreMDS的情况），根据对应的文件路径确定后进行文件读取。

###### 1.6.2 Read 流程：

  	   当Blbsvc本地持有chunkstore实例读写过的文件路径cache(当加入chunkStoreMDS之后，存在可能把读写文件路径返还的情况),先checkCache获得可读文件路径集合，再进行ReadChunk()操作，如果cache中找不到，即读失败（默认只能读本地已操作文件）。流程图如下图所示。

![1565059136038](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1565059136038.png)

###### 1.6.3 Write流程：

​		 MDSclient直接RPC调用 MDSsvc获得可写文件地址，内部进行文件路径的管理和分配（持有文件路径集合），完成写操作之后需要返回给ChunkStoreMDS更新文件目录信息，同时需要在本地cache中记录写过的文件路径信息。流程图入下图所示。

![1565058941740](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1565058941740.png)

### 二 原型设计

##### 2.1 原型测试设计

​		由于ChunkStoreMDS是一个必须满足分布式架构的系统，故而在Prototype的单机测试中，采用多线程来模拟不同的节点访问，来测试在并发情况下的表现。ChunkStoreMDS本身维护的是一个文件路径的集合（“.ccc“格式的文件路径），系统所实现的目标就是根据对应的chunkstore信息（在系统中体现为包含文件路径的文件信息结构体），进行文件路径（chunkstore初始化所必须的参数）的统一分发和管理。

​		测试的流程：

​		（1）chunkstoreMDS初始化效率测试，例：256个文件夹下的10000个文件信息初始化。

​		（2）并发测试，例： 256个线程并发call chunkStoreMDS模拟多个节点不同call ,单个线程中设置call频率和模拟读写。

​		（3）chunkStoreMDS单元测试。

​		（4）结合chunkStoreViewer集成测试读写(需要根据新逻辑扩展chunkstore部分方法);

##### 2.2  扩展方法说明

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

​								  	(2) 找不到合适调用就创建一个container，FindnewContainerForTranction（new chunksize,new container）

​		增加MDS扩展后方法：

​		c. 扩展读方法：先查看session中是否有要读取的文件（container），如果没有则需要查看本地cache(单节点中已经读写过的文件集合)，如果cache中没有则读失败。

​		d. 扩展写方法： 先查看session中是否有可读写的文件，如果找不到合适的可写文件，则向MDSSvs请求一批可写的新文件，（并将已写文件异步返回以便MDSsvc更新），需要在本地cache中记录该节点读写过的文件路径信息（测试采用写本地文件的形式记录）。

​		 扩展方法中需要关注的类  chunkStoresession ，cchunkStore 。

##### 2.3  原型中类与接口设计  

| Class  name          | Function                                                     |
| -------------------- | ------------------------------------------------------------ |
| FileInfoScan         | 接口，抽象获取文件路径集合的方法                             |
| DiskScan             | 物理磁盘实现FileInfoScan，获取指定磁盘上的可读写文件路径集合 |
| ChunkStoreMDS        | 管理文件路径信息的请求和分发（需要支持并发）                 |
| ChunkStoreMDSManager | 实现平衡策略和chunkstoreMDSRecovery                          |
| MDSClient            | 实现节点与ChunkStoreMDS的交互，维护本地cache,提供可读写文件路径 |
| BlbClient            | 实现节点的读写过程                                           |
| FileHolder           | 维护一个逻辑文件夹（包含不同物理磁盘的文件），提供类似于os的文件夹集合操作（增删改查），提供Folder Recover功能 |
| TestMDS              | 用于原型测试                                                 |

##### 2.4  数据设计

```c++
// fileInfo maintained by chunkStoreMDS  
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
//MDSclient request_msg
struct Request_MSG{
        char *owner;
        int fileNum;
        vector<fileInfo>  writenFilesinfo;
}

//MDSserver return_msg
struct Return_MSG{
     char *owner;    
     vector<fileInfo> files;
}
// Balance_Policy
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

##### 2.5 关键方法设计

| Public API（一）                                             | Function                           |
| ------------------------------------------------------------ | :--------------------------------- |
| DiskScan::GetAllFiles( string path, vector<string>& files)   | 获取指定磁盘的全部文件信息         |
| DiskScan::GetAllFormatFiles( string path, vector<string>& files,string format) | 获取指定磁盘的指定格式文件信息     |
| fileFolder::Recover(string path,vector<fileInfo>& files)     | MDS crash后恢复文件路径信息        |
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
| ChunkStoreMDSManager::Recovery();                            | MDS Crash后恢复文件路径信息                        |

| Test  API( 三 )               | Function                                          |
| ----------------------------- | ------------------------------------------------- |
| MDSClient::RequestFile()      | 请求文件路径                                      |
| MDSClient::ReturnFile()       | 返回读写文件信息                                  |
| MDSClient::ParseLocalFile()   | 本地文件路径信息解析操作（用于测试读）            |
| MDSClient::PersistLocalFile() | 本地cache维护                                     |
| MDSClient::Request_Recovery() | 恢复本地文件路径信息（用于MDSclient crash后恢复） |
| BlbClient::initChunkStore()   | 节点初始化chunkStore                              |
| BlbClient::read()             | 节点读取操作                                      |
| BlbClient::insert()           | 节点插入操作                                      |

##### 2.6  测试过程

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

​		按照Intern Plan ，如果在此之前的ChunkStoreMDS在当前架构扩展下的read() ,insert()方法上效果不错，接下来是考虑delete()和GC()的设计，并且已完成的demo的基础上进一步实现。





