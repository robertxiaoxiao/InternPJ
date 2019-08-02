

#include "fileFolder.cpp"
#include "ChunkStoreMDS.cpp"
#include <vector>

/*

abstract  interface  fileinfoscan  impl  by diskScan and databaseScan   

 */
class FiliInfoScan{


    public :

        vector<fileInfo>  initFileList;


            //  scan for fileinfolist ;
            
        //scan for all files 
        void    GetAllFiles( string path, std::vector<string>& files)  ;

        //scan for format files 
        void    GetAllFormatFiles( string path, vector<string>& files,string format) ;

            // crash scenerio recovery
        virtual    void Recovery();


};




