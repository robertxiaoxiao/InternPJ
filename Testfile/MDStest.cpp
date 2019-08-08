#include <iostream>           
#include <queue>
#include <thread>             
#include <mutex>              
#include <condition_variable> 
#include <functional>
using namespace std;

mutex mtx;
condition_variable produce, consume;  // 条件变量是一种同步机制，要和mutex以及lock一起使用

queue<int> q;     // shared value by producers and consumers, which is the critical section
int i = 0;

std:: mutex l;
int  autotimes =100 ;
class Test
{
    public:
        Test(int m)
        {
		
        }

	  void Enableincre(Test* p){
		
		while (true)
			{
				
				/* code */
				l.lock();
         	   	p->n=p->n+10;	
				i++;
				l.unlock();
				_sleep(1000);
				
			}
	  }
		 	

      void print(Test* p)
        {
			while (true)
			{
				
         	    std::cout<<p->n<<std::endl;
			
				_sleep(1000);	/* code */
			}
        }

		static void  phello(){

			cout << "HELLO WORLD!!" << std::endl;

		}

    private:
        static volatile int n;
};

void print(int i){
	cout << "HELLO WORLD!!" <<i<< std::endl;
}

int volatile Test::n=1;

int main(){


		for(int i=0 ;i<10;i++)
			{
				thread t(print,i);
				t.detach();

			}

		// Test test(1);
	
		// thread  t1(&Test::Enableincre,&test);

		
		// thread  t3(&Test::print,test);
		
		// thread  t2(&Test::print,&test);


		
		// t1.join();
		// t2.join();
		// t3.join();

  	// 
	// test.Enableincre();
	// test.print();
	// test.phello();
	
}
