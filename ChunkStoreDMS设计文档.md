### ChunkStoreDMS设计文档 

#### 一  架构图

##### 1.1当前架构

​		如下图所示，每个Client端通过调用生成一个BlbSvc实例，每一个BlbSvc实例在初始化时已经确定了对应的ChunkStore路径，由于ChunkStore可以无限写直到磁盘被写满（在物理磁盘允许的情况下），可能导致系统管理的磁盘上，不同的ChunkStore中存储数据大小产生较大差异。

![1564561731006](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1564561731006.png)

##### 1.2 设计架构

​		每个Client先从ChunkStoreDMS中请求需要写入的ChunkStore文件路径集合，根据返回的文件路径集合初始化对应的ChunkStore，每个Blbsvc只提供基本的IRDG（INSERT,READ,DELETE,GC）接口，物理磁盘上的ChunkStore由ChunkStoreDMS模块统一管理，并且提供BalanceStore ，ManageDisk,StoreRecovery,initChunkStore功能。

![1564561792233](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1564561792233.png)

### 二 概要设计

##### 2.1 原型测试设计

​		由于ChunkStoreDMS是一个必须满足分布式架构的系统，故而在Prototype的单机测试中，采用多线程来模拟不同的节点访问，来测试在并发情况下的各项指标。ChunkStoreDMS本身维护的是一个文件路径的集合（“.ccc“格式的文件路径），系统所实现的目标就是根据对应的chunkstore信息（在系统中体现为包含文件名的文件信息结构体），进行文件路径（chunkstore初始化所必须）的统一分发和管理。

##### 2.2  类设计

| Class  name          | Function                                                     |
| -------------------- | ------------------------------------------------------------ |
| DiskScan             | 封装系统提供的扫描磁盘文件的方法，用于crash后恢复ChunkStoreDMS信息 |
| FileHolder           | 维护一个逻辑文件夹（包含不同磁盘的文件），提供类似于os的文件夹集合操作（增删改查） |
| ChunkStoreMDS        | 管理文件信息的请求和分发                                     |
| ChunkStoreMDSManager | 实现平衡策略和storeRecovery                                  |
| TestDMS              | 用于原型测试                                                 |

##### 2.3 数据设计

```c++
// 文件信息
struct fileInfo{}

// 客户端请求信息
struct Request_MSG{}

//客户端返回信息
struct Return_MSG{}

// 平衡策略
typedef enum
{
        UNKNOWN_OPERATION = 0,
        STORAGE_SIZE = 1    
        //to add more policy
} BALANCE_POLICY;

//客户端恢复信息
struct   client_Recovery_MSG{
        /* data */
        char *owner;
        vector<fileInfo> ownedFileList;
};
```

##### 2.4 关键函数设计

| API（一）                                                    | Function                             |
| ------------------------------------------------------------ | ------------------------------------ |
| DiskScan::GetAllFiles( string path, vector<string>& files)   | 获取指定磁盘的全部文件               |
| DiskScan::GetAllFormatFiles( string path, vector<string>& files,string format) | 获取指定磁盘的指定格式文件           |
| DiskScan::Recover(string path,vector<fileInfo>& files)       | DMScrash后恢复文件信息（仅用于测试） |
| fileFolder::listFiles()                                      | 列出当前文件夹中的所有文件信息       |
| fileFolder::createMoreFile()                                 | 创建更多的chunkstore文件路径信息     |
| fileFolder::addFile(string s)                                | 增加指定路径                         |
| fileFolder::checkFileContained(string ts)                    | 检测是否存在指定文件路径信息         |
| fileFolder::deleteFile(string ts)                            | 删除指定路径                         |
| ChunkStoreMDS::printState()                                  | 打印当前MDS 信息                     |

| API（二） | Function |
| --------- | -------- |
|           |          |
|           |          |
|           |          |
|           |          |
|           |          |
|           |          |
|           |          |
|           |          |
|           |          |

