#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <math.h>
#include "stdlib.h"

int times=1;
int pt=1;
using namespace std;
void  coin_flip(int size){


        cout<<"    coin flip test_"<<times<<" starting"<<std::endl;

        int cnt=0;
        for (int i =0; i<size ;i++)
             {  
                 int v1=rand()%100 ;
                 
                 if(v1<50)
                     cnt++;

             }


         cout<<"    coin flip test_"<<times<<" ending"<<std::endl;

         cout<<"    result_"<<times<<" :"<<std::endl;

         cout<<"       positive : "<<setiosflags(ios::fixed)<<setprecision(3)<<(double)cnt/size<<std::endl;
    
         cout<<std::endl ;

         times++;

         if((double)cnt/size > 0.500)
                pt++;

}


int main(){

    for(int i =0;i<10;i++)
     {
         if(i==0)
              cout<<"Test Func :sin(): "<<std::endl;

         if(i==5)
              cout<<"Test Func :cos(): "<<std::endl;

         coin_flip(10000);


        // goto stop;
     }
     cout<<"Total Positive : "<<setiosflags(ios::fixed)<<setprecision(3)<<(double)pt/10<<std::endl;



// stop:     
//      cout<<"error occured : "<<std::endl;

}


