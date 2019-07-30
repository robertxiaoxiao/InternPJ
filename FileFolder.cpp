#include  <stdio.h>
#include  <iostream>
#include <string>
#include <vector>
#include "fileFolder.h"

using namespace std;

/*
   to implements the os operations in filefolder 
 */

/*
       to do : 并发控制
 */


  //  ask for files 
        //  defalut string for test 
string testFiles[3]={"1.ccc","2.ccc","3.ccc"};


// default construct 
   fileFolder::fileFolder(){ 
        files = vector<string>() ;
  }


// release files memory
   fileFolder::~fileFolder(){
    
        files.clear();
   };

    //  list files
    void fileFolder::listFiles(){
        printf("current folder files: \r\n") ;
        int i=0;
        for (auto  s: files)
        {
            /* code */
            printf("file_%d      %s \r\n",i++,s.c_str());
        }
        
    }

    // ask for other api to get files
    void fileFolder::createFile(){
        for(string  s: testFiles)
        {
                addFile(s);
        }

    }


    // add files into  folder 
     void fileFolder::addFile(string s){
            files.push_back(s);
     }


    /*
    if file in  files  rteurn index;
    
    else 
        return   -1;
     */

     int   fileFolder::checkFileContained(string ts){
            for(int i =0; i<files.size();i++)
                    if(ts.compare(files[i])==0)
                    {
                        printf("file has been found int idx %d\r\n",i);
                            return i;
                    }

            else 
            {       printf("file has not been found \r\n ");
                        return -1;
            }
     }


    void  fileFolder::deleteFile(string ts) {

            int idx=checkFileContained(ts);

            if(idx==-1){

                printf("the input filrpath not exist\r\n")   ;  
            }
            else
            {
                files.erase(files.begin()+idx);
                 printf("the file  %s has been deleted\r\n" ,ts) ;
            }
    
    }

// unit test
    int main(){

        fileFolder  s2;
        
        s2.createFile();
        s2.listFiles();
        s2.checkFileContained("1.ccc");
        s2.checkFileContained("4.ccc");
        s2.deleteFile("1.ccc");
        s2.listFiles();
        s2.~fileFolder();

    }