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

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
 

using namespace std;
//   使用互斥量

std::mutex m;

std::condition_variable cv;
// 并发处理
std::string data;


bool ready = false;
bool processed = false;
 
void worker_thread()
{
    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{return ready;});
 
    // after the wait, we own the lock.
    std::cout << "Worker thread is processing data\n";
    data += " after processing";
 
    // Send data back to main()
    processed = true;
    std::cout << "Worker thread signals data processing completed\n";
 
    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
}
 

int main()
{
    std::thread worker(worker_thread);

    std::thread worker1(worker_thread);



 
    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }

    cv.notify_all();
 
    // wait for the worker
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return processed;});
    }


    std::cout << "Back in main(), data = " << data << '\n';
 
        worker.join();
        worker1.join();
}


    

