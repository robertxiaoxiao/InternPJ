// #include <thread>
// #include <mutex>
// #include <iostream>
// #include <vector>
// #include <queue>
// using namespace  std;

// vector<int>  nums;
// template<typename T>
// class threadsafe_queue
// {
// private:
//   mutable std::mutex mut;
//   std::queue<T> data_queue;
//   std::condition_variable data_cond;

// public:
//   threadsafe_queue()
//   {}

//   void push(T new_value)
//   {
//     std::lock_guard<std::mutex> lk(mut);
//     data_queue.push(std::move(data));
//     data_cond.notify_one();  // 1
//   }

//   void wait_and_pop(T& value)  // 2
//   {
//     std::unique_lock<std::mutex> lk(mut);

//     //  捕获当前对象指针  lambda表达式
//     data_cond.wait(lk,[this]{return !data_queue.empty();});
//     value=std::move(data_queue.front());
//     data_queue.pop();
//   }

//   std::shared_ptr<T> wait_and_pop()  // 3
//   {
//     std::unique_lock<std::mutex> lk(mut);
//     data_cond.wait(lk,[this]{return !data_queue.empty();});  // 4
//     std::shared_ptr<T> res(
//       std::make_shared<T>(std::move(data_queue.front())));
//     data_queue.pop();
//     return res;
//   }

//   bool try_pop(T& value)
//   {
//     std::lock_guard<std::mutex> lk(mut);
//     if(data_queue.empty())
//       return false;
//     value=std::move(data_queue.front());
//     data_queue.pop();
//     return true;
//   }

//   std::shared_ptr<T> try_pop()
//   {
//     std::lock_guard<std::mutex> lk(mut);

//     if(data_queue.empty())
//       return std::shared_ptr<T>();  // 5
//     std::shared_ptr<T> res(
//       std::make_shared<T>(std::move(data_queue.front())));
//     data_queue.pop();
//     return res;
//   }

//   bool empty() const
//   {
//     std::lock_guard<std::mutex> lk(mut);
//     return data_queue.empty();
//   }
// };
//   使用互斥量

// std::mutex m;

// std::condition_variable cv;

// // 并发处理
// std::string data;

// bool ready = false;
// bool processed = false;

// void worker_thread()
// {
//     // Wait until main() sends data
//     std::unique_lock<std::mutex> lk(m);
//     cv.wait(lk, []{return ready;});

//     // after the wait, we own the lock.
//     std::cout << "Worker thread is processing data\n";
//     data += " after processing";

//     // Send data back to main()
//     processed = true;
//     std::cout << "Worker thread signals data processing completed\n";

//     // Manual unlocking is done before notifying, to avoid waking up
//     // the waiting thread only to block again (see notify_one for details)
//     lk.unlock();
//     cv.notify_one();
// }

// void test(){
//  std::thread worker(worker_thread);

//     std::thread worker1(worker_thread);

//     data = "Example data";
//     // send data to the worker thread
//     {
//         std::lock_guard<std::mutex> lk(m);
//         ready = true;
//         std::cout << "main() signals data ready for processing\n";
//     }

//     cv.notify_all();

//     // wait for the worker
//     {
//         std::unique_lock<std::mutex> lk(m);
//         cv.wait(lk, []{return processed;});
//     }

//     std::cout << "Back in main(), data = " << data << '\n';

//         worker.join();
//         worker1.join();
// }


// std::mutex  l;
// class  demo{

//     static int i;
//     public :

//         void incre();

// };

// std::mutex mtx;
// std::condition_variable cvar;
// char g_ch = 0;

// void demo::incre(){

//         cout<<"test cur"<<std::endl;

//          bool  flag=false;

//         while(true)
// {
//         std::unique_lock<std::mutex>ulk(mtx);
//         Sleep(1000);

//         flag=true;

// 		cvar.wait(ulk, flag);

//         i++ ;
//         cout<<"i="<<i<<std::endl;

//         ulk.unlock();

// 		cvar.notify_all();
// }

// }

// int demo::i=0;

// class Test{

//   public :
//         demo   a;
//         Test(demo A){

//             a=A;
//         }

//         void autoincre(){
//              cout<<"test autocre"<<std::endl;
//              thread t(&demo::incre,a);
//              t.join();
//         }

// };

// void print(){

//      cout<<"hello world"<<std::endl;
// }

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <Windows.h>
#include <fstream>


using namespace std;

struct fileInfo
{

    // filename unique
    std::string filepath;

    //  check free and ctrl file access
    bool Beusing;

    // help to recover client's filelist info
    char *owner;

    // writen size  for balance
    long offset;
};


void parseline(string s ,fileInfo& finfo);

int main()
{


   double  a=1/2;

   double b =(double)1/(double)2;
   cout <<a<<std::endl;
    cout <<b<<std::endl;

    return 0;
    // std::string path = "D:\\BlobServiceData\\TestPartition\\testcache.txt";
    // std::ofstream location_out;
    // location_out.open(path, std::ios::out | std::ios::app);

    // fileInfo files[10];
    // fileInfo tempfiles[10];
    // string p;
    // for (int i = 0; i < 10; i++)
    //  {   files[i] = {p.assign("a_").append(std::to_string(i)), true, "fzy", 0};
    //     tempfiles[i]={p.assign("a_").append(std::to_string(i)), true, "fzy", 0};
    //  }

    // if (location_out.is_open())
    // {
    //     for (fileInfo f : files)

    //         location_out << f.filepath << " " << f.Beusing << "  " << f.owner << "  " << f.offset << endl;
    // }

    // location_out.close();

    // int i = 0;

    // string Line;
	// //float feat_onePoint;  //存储每行按空格分开的每一个float数据
    
    // string token;

    // cout << "parse file"<<std::endl;
    // while(!in.eof())
    //     {
    //             getline(in,line);
    //             stringstream stringin(line);
    //               while(line>>token)

    //             cout<<line<<std::endl;
    //           parseline(line,tempfiles[i]);
    //             i++;
    //     }
//   string s="fzy 0 111 0" ;
//         for(i=0;i<10;i++)
//             parseline(s,tempfiles[i]);

//    //  tempfiles[0].offset=111;
//     for(fileInfo  f: tempfiles)
//         cout<<f.filepath<<" "<<f.Beusing<<" "<<f.owner<<" "<<f.offset<<endl;


    // // copy  call constuct
    // A temp=A(a);

    // std::cout << "a  addr " << &a << '\n';
    // std::cout << "a  temp " << &temp << '\n';

    // std::cout << "a  info: " << a.name<<"    "<<a.b << '\n';
    // std::cout<<a.name.compare("")<<std::endl;

    // std::cout << "temp  info: " << temp.name<<"    "<<temp.b << '\n';
}



void parseline(string s ,fileInfo& finfo){
       string delimiter=" ";
       size_t pos = 0;

    int cnt=0;
    string tokens[4];
    while (cnt<4) {
    pos=s.find(delimiter);
        tokens[cnt].assign( s.substr(0, pos));
     s.erase(0, pos + delimiter.length());
     cnt++;

}
    finfo.filepath=tokens[0];
    finfo.Beusing=tokens[1].compare(0)==0?false:true;
    finfo.owner=(char*)tokens[2].data();
    finfo.offset=std::stoi(tokens[3]);

}