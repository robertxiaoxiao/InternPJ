#include  <stdio.h>
#include  <iostream>
#include <string>
#include <vector>
#include "FileInfoScan.cpp"
using namespace std;
class fileFolder{


    private:

        std::string  recordTxtPath;

    public :
    
    FileInfoScan filescanner;
    

     // file name space 
    std::vector<std::string> files;


    fileFolder(FileInfoScan fileScanner );

    ~fileFolder();


    static void  initFiles(){
          filescanner.getFiles
    };

    // print ctrl files
    void listFiles();

    // create file ;
    void createMoreFile(int filenum);

    // add file ;
      void addFile(std::string file);

    // delete file;
      void deleteFile(std::string filename);

    //  check file
      int checkFileContained(string filename);
    
   	// to recover 
	  void Recovery(vector<string>& files) ;


    void setPath(string s) {

         recordTxtParh=s ;
    }

	    // 配置恢复路径
    void EnsureRecovery(string s) ;
	{	
		    setPath(s);
        
	}


};
