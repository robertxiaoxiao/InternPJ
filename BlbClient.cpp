#include "preIn.h"

class BlbClient{


        public :
                string  filepath ;

                //chunkStore    store;
                
                
                // init chunkstore using filepath
                void  initChunkStore();

                // chunkstore read
                void Read(string Chunksize ,string  filepath) ;

                //
                void Write(string chunksize ,string filepath);






}