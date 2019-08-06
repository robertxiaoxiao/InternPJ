#include "preIn.h"

class BlbClient{


        public :
                string  filepath ;

                //chunkStore    store;
                
                
                // init chunkstore using filepath
                void  initChunkStore();

                // chunkstore read
                void Read(string Chunksize ,string  filepath);

                //chunkstore insert
                void Write(string chunksize ,string filepath);


        /*
              new  chunkstore init ;
              chunkstore.staticinit;
              cstreamstore::create(volumeroot,volumepath,....)
              sessionManager.createSession(cstreamstore) ;       
         */


        


}