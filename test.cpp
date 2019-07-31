#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <future>
#include <thread>
using namespace std;

struct inum
{
    int i;

    vector<int> list ;
    /* data */
};

inum geti(int i  ){

    inum temp={};

    temp.i=i;

    for(int i =0;i<5;i++)
      temp.list.push_back(i);


    return temp;

}

void print(int i){
    cout<< i<<"hello    world\r\n"<<std::endl;

}

int main(){



        std::thread  ts[10];

        for(int i=0;i<10;i++)    
                ts[i]=thread(print,i);

        auto fr0 = async([](){cout << "Welcome to async" << endl;});
        
        for(int i=0;i<10;i++)  
                ts[i].join();
        // inum t=geti(5);
        // cout<< t.i<<std::endl;
        
        // for(int i : t.list)
        //     cout<< i<<std::endl;


        // char* t1="ab" ;
        // char* t2="ab" ;
        
        // char* t3="a" ;

        
        // cout<< strcmp(t1,t2)<<std::endl;
        
        // cout<< strcmp(t1,t3)<<std::endl;

 
 fr0.get();

 return 0;
    // std::vector<int>   list;
    // for(int i=0;i<10;i++)
    //   list.push_back(i) ;


    //  vector<int>&  l=list;
    //  int& i=list.at(5);
    //  cout<< i<<std::endl;

    //  l.erase(l.begin()+5);


    // cout<< i<<std::endl;

    
     




    //   int size=l.size();
    //   for(int i=0;i<10;i++)
    //        { res.push_back(inum{l.at(size-i-1)});
    //         cout<< l.at(size-i-1)<<std::endl;
    //        }

    //     cout<< res.size()<<std::endl;
   

}