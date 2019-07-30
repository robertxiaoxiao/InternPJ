#include  <stdio.h>
#include  <iostream>
#include <string>
#include <vector>

using namespace std;
class fileFolder{
    public :

     // file name space 
    std::vector<std::string> files;

          fileFolder(int size);

          fileFolder();

          ~fileFolder();

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

};
