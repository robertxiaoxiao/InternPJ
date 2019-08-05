#include <vector>
#include <string>

/*

abstract  interface  fileinfoscan  impl  by diskScan and databaseScan   

 */
using namespace std;
class FileInfoScan{


    public :
          
        //scan for all files 
       virtual   void    GetAllFiles( std::vector<std::string>& files) =0 ;

        //scan for format files 
        virtual void    GetAllFormatFiles(std::vector<std::string>& files,std::string format)=0;


};




