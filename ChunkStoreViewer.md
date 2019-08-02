

### chunkstore create

1. ```c++
   pre:
   
   1. BlobInitializeStubs():  initial api stubs
   2. BlobTraceGlobal::StaticInitialize()
   3. MultiByteToWideChar()   ()
   4. SrmTraceInitialize()  
   5. RtlAdjustPrivilege()  
   
   pro:
   //  store Initialize
   IBlbBlockStore::StaticInitialize(
           volumePath,
           BLOB_PARTITION_DIR_NAME,
           threadPool,
           FALSE,
           store);
   
   
   
   CBlbDedupBlockStore : public  IBlbBlockStore
   
   // initialize
    DedupBlockStore = new CBlbDedupBlockStore( IsSecondaryPartition );
   
       status = DedupBlockStore->Initialize( pszVolumeRoot,
                                             pszPartitionDir,
                                             ThreadPool,
                                             IsMigrationSource );
   
   
   // real impl
   CBlbDedupBlockStore::Initialize(
       _In_ PCWSTR         pszVolumeRoot,
       _In_ PCWSTR         pszPartitionDir,
       _In_ IThreadPool    *ThreadPool,
       _In_opt_ BOOL       IsMigrationSource /* = FALSE */
       )
    
       
       
    //  Initialize the chunkstore 
   CStreamStore    
   
   // aop   m_DedupSessionManager = new CBlbDedupSessionManager( m_StreamStore, ThreadPool );
   
   // This class exposes various functionality of chunk store to other modules
   // in Dedup feature.
   CStreamStore::Create( pszVolumeRoot, volumePath.c_str(), m_IsSecondaryPartition, FALSE )
       
    // volume path like drive letter e:\, mount
        // point p:\mount\, or volume guid path
                                        // \\?\Volume{25837862-4c28-11df-b2cd-806e6f6e6963}\.
                                           // trailing backslash is optional.
       
       
       CStreamStore::  HRESULT CreateSession(
           __out CStreamStoreSession*& Session // receive a pointer to the new session
                                               // the pointer remains valid until
                                               // CommitSession, AbortSession or
                                               // Uninitialize is called.
           ) throw();
       //we only support one outstanding session at a time.
    
       
    // Create a new stream within the context of a session.
   CStreamStore::CreateNewStream(
       __in CStreamStoreSession* Session,
       __in ULONG MaxChunkCount,
       __in LONGLONG FileOffset,
       __out CStreamStoreStream*& Stream) 
   
     
      // ssesion 
     class CStreamStoreSession 
        void  AttachData(CStreamStoreSession* s);
        void  detachData(CStreamStoreSession* s);
   	 void  getDATA()
         RETURN   m_activeDataCSSession ;
         
         
       
         
         
     // The design spec states that ChunkStore is an abstract class and
   // LocalFSChunkStore is an implementation of i
         class CChunkStore ()
            
             //ft   function tracer class  log
        
                      
       // Use an internal class so that client code doesn't have to
       // include all the internal classes in chunk store.
       class CStreamStoreInternal ()
           
        CInternal()::Initialize()   
          
       if (ft.HrSucceeded())
       {
           // The store is almost ready except hotspot. Set things up so that
           // it can be used by hotspot initialization (in rebuild case) after
           // this point. Alternatively, we can have hotspot table references
           // stream map/data/hotspot chunk store directly by passing in them
           // vs through the container class (this class).
           NT_ASSERT(tempStoreId != GUID_NULL);
           C_ASSERT(sizeof(tempStoreId) == sizeof(m_storeId));
           m_storeId = tempStoreId;
           m_storeRootDir = streamStoreRootDir;
   
           m_streamMapCS.SetStoreId(m_storeId);
           m_dataCS.SetStoreId(m_storeId);
       }
           
   
   create_chunkstore()
      
   
   
   /// Details:
   (1)create chunkstore: call inherachy
   
   MultiByteToWideChar(CP_ACP, 0, volumepath, 2, (LPWSTR)volumePath, 3);
   
   threadPool->InitializeThreadPool();
   
   Blobstub::
   RtlAdjustPrivilege(
           SE_MANAGE_VOLUME_PRIVILEGE,
           TRUE,
           FALSE,
           &oldPrivilegeState);
   
   //配置
   SettingsMemPool::SetEffectiveSetting()
   
       //初始化chunkstore
       IBlbBlockStore::StaticInitialize(
           volumePath,
           BLOB_PARTITION_DIR_NAME,
           threadPool,
           FALSE,
           store);
   
   
   
   insertChunk: call
   
   CBlbDedupBlockStore::InsertChunk()
   m_StreamStore->InsertNewChunk()
   CStreamStore::InsertChunk()
   m_innerStore->InsertNewChunk(）
   CStreamStoreInternal*m_innerStore::InsertNewChunk()
   CDataChunkStore  m_dataCS.InsertNewChunk();      // This class is a thin specialization of CChunkStore for data chunks.
   // For the most part, it provides a layer to introduce data chunk store specific
   // defaults such as container capacity, occupancy rates, etc.                 
    CChunkStore::InsertNewChunk()
                             CChunkStore::FindContainerForTransaction
    
           
   ```

2  Key  Class diagram 

```c++
//初始化chunkstore 
  IBlbBlockStore::StaticInitialize
  
 // Initialize a blob block store object.
 static DWORD
    StaticInitialize(
        _In_ PCWSTR             pszVolumePath,
        _In_ PCWSTR             pszPartitionDir,
        _In_ IThreadPool        *ThreadPool,
        _In_ BOOL               IsSecondaryPartition,
        _Out_ IBlbBlockStore    *&BlockStore,
        _In_opt_ BOOL           IsMigrationSource = FALSE
        );


Blockstore   :IBlbBlockStore class implementation.


IBlbBlockStore::StaticInitialize(
    _In_ PCWSTR             pszVolumeRoot,
    _In_ PCWSTR             pszPartitionDir,
    _In_ IThreadPool        *ThreadPool,
    _In_ BOOL               IsSecondaryPartition,
    _Out_ IBlbBlockStore    *&BlockStore,
    _In_opt_ BOOL           IsMigrationSource /* = FALSE */
    )

  CBlbDedupBlockStore DedupBlockStore = new CBlbDedupBlockStore( IsSecondaryPartition );
 //初始化属性
 DedupBlockStore->Initialize( pszVolumeRoot,
                                          pszPartitionDir,
                                          ThreadPool,
                                          IsMigrationSource );
    //      return value    
    BlockStore =   DedupBlockStore    ;            
                  
     // 构造             
   CBlbDedupBlockStore(_In_ BOOL IsSecondaryPartition)
            :
            m_BlockStoreLock(),
            m_StreamStore( NULL ),
            m_DedupSessionManager( NULL ),
            m_IsSecondaryPartition( IsSecondaryPartition )
        {}
    
    // This class exposes various functionality of chunk store to other modules         

CStreamStore :: create()
     ft.hr = CStreamStoreInternal::Create(volumeRoot, volumePath, IsSecondaryPartition, bSkipRootDirectoryVerification);

// call   CStreamStoreInternal

CStreamStoreInternal::create(){
    
    seeting state blobstore
}

//  call   cchunkstore
//  data  stream hotspot  fouces: data 
CChunkStore::Create(
            streamStoreRootDir,
            DDP_DATA_CHUNK_STORE_DIRECTORY_NAME,
            IsSecondaryPartition
            );
   
// cchunkstore implements the ChunkStore, LocalFSChunkStore and ContainerManager
 cchunkstore::ensureDirectory     BlbCreateDirectory()


    
     
     
     
// all chunkstore operations  controlled b y chunkstorsession 
     
     
     
     
     
     
     
    
    //  
 CChunkStore::InsertNewChunk(
    __in CChunkStoreSession* Session,
    __in_bcount(DataSize) PBYTE Data,
    __in ULONG DataSize,
    __in const DDP_CHUNK_COMPRESSION_INFO& CompressionInfo,
    __in const DDP_CHUNK_HASH_INFO& HashInfo,
    __out DDP_CHUNK_ID& NewChunkId)


  
    //cchunk    数据实体抽象， CDataChunk  CStreamMapChunk  CHotspotChunk
     CChunk( 
        __in USHORT PropertySetType,
        __in_ecount(DataSize) PBYTE Data,
        __in ULONG DataSize,
        __in bool DataOwned,
        __in ULONG CorruptionType
        );

class CDataChunk : public CChunk
    CDataChunk(
        __in_ecount(DataSize) PBYTE Data,
        __in ULONG DataSize,
        __in const DDP_CHUNK_COMPRESSION_INFO& CompressionInfo,
        __in const DDP_CHUNK_HASH_INFO& HashInfo        
        );


CChunkStore::InsertNewChunk(
    __in CChunkStoreSession* Session,
    __inout CChunk* Chunk)
    
    // call CChunkStoreContainer
   Getchunksize()
   CChunkStore::FindContainerForTransaction(Session, tempChunkSize, container)
    
    
    //迭代找m_container 
     void StartReverseEnumeration(
        __out CChunkStoreContainerVector::reverse_iterator& Iter,
        __out CChunkStoreContainerVector::reverse_iterator& EndIter) throw()
    {
        Iter = m_containers.rbegin();
        EndIter = m_containers.rend();
    }
    
// session 设置
Session->SetLastUsedContainer(tempContainer);

// find container   
  //  Make sure only one session can perform the container lookup at
  // a time as the container namespace is shared among all sessions.
  // CSrmAutoExclusiveSRWLock containersLock(m_containersLock);
CChunkStore::FindNewContainerForTransaction(
    __in_range(>, 0) ULONGLONG NewChunkSize,
    __out CChunkStoreContainer*& Container)
    
    
    //  Generic auto-object class
    //class CSrmAuto  : public Storage

   
    //.ccc
     CChunkStore::FindContainerForTransaction(
    __in CChunkStoreSession* Session,
    __in_range(>, 0) ULONGLONG NewChunkSize,
    __out CChunkStoreContainer*& Container)
    
    
    
    // 管理session 
    CStreamStoreSession :
{
    内部维护session集合  控制并发？
        
    ChunkStoreSession* m_activeDataCSSession;
    CChunkStoreSessionSet m_inactiveDataCSSessions;
   
}

// chunkstore内部维护一张chunkIDmap
CBlbDedupBlockStoreSession::AddChunkToStream(
    _In_ ULONG                  ChunkDataSize,
    _In_ BLOB_CHUNK_ID&         ChunkId,
    _In_ ULONG                  StreamMapElements,
    _Inout_updates_(StreamMapElements) PBLOB_STREAM_MAP_ENTRY
                                &StreamMap,
    _Inout_ ULONG               &ChunkSequence
    )
/**
Routine Description:

    Insert a chunk id into a stream map in the chunkstore.
  */
   //保证session并发执行  
   //读写都是靠session控制
   // 代码实现过程中就是进行的session控制，可供多线程同时访问
    BlbDedupBlockStoreSession::Flush()
    
    BlbDedupBlockStoreSession 
    {
    // // Create a chunk store on the specified volume. If a chunk store already
    // exists, this API ensures the chunk store is properly secured.
    // 持有一个cstreamstoe引用
     CStreamStore
    }
    
    
/**
Routine Description:
    Flush a block store session. Use Flush() when you want to keep the session open to be reused.
   */

// 绑定store和对应session
    HRESULT CChunkStore::CreateSession(
    __in const DDP_STORE_ID& StoreId,
    __out CChunkStoreSession*& Session)
    
   
        
          CChunkStoreContainer(
        __in ULONG ContainerId,
        __in ULONG Generation,
        __in bool Writable,
        __in_opt bool IsNew,                /* false */
        __in_opt ULONG PhysicalSectorSize,  /* 0 */
        __in_opt PVOID CreateFileContext,   /* NULL */
        __in_opt BOOL IsSecondaryPartition  /* false */
        );


//
// Commit the session when this number of bytes have been written.
// Currently set it to 1GB to match the dedup service behavior.
//

// Get a session from session manager. The session must not be shared by multiple threads.

  CBlbDedupSessionManager(
            _In_ CStreamStore* StreamStore,
            _In_ IThreadPool *ThreadPool
            );



// focused 
CChunkStore::FindContainerForTransaction(
    __in CChunkStoreSession* Session,
    __in_range(>, 0) ULONGLONG NewChunkSize,
    __out CChunkStoreContainer*& Container)
    
    
    // cchunkstoresession
    //维护store的文件列表
       CChunkStoreContainerVector m_containers;
    CChunkStoreContainerVector m_ownedContainers;
    CChunkStoreContainer* m_lastUsedContainer;
    bool m_committed;
    DDP_STORE_ID m_storeId;



//    CStreamStoreSession
//    维护datachunkstore的session列表
void AttachDataChunkStoreSession(__in CChunkStoreSession* Session)
    {
        NT_ASSERT(Session != NULL);

        if (m_activeDataCSSession != NULL)
        {
            m_inactiveDataCSSessions.insert(m_activeDataCSSession);
        }
        m_activeDataCSSession = Session;
    }


//组合代替继承
class CBlbDedupBlockStoreSession::
           CStreamStoreSession*
        GetDedupSession()
        {
            return m_StreamStoreSession;
        }
```



![1564730945465](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1564730945465.png)

![1564731004086](C:\Users\t-zhfu\AppData\Roaming\Typora\typora-user-images\1564731004086.png)