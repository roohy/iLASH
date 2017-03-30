//
// Created by Ruhollah Shemirani on 3/29/17.
//

#ifndef IBD_LSH_SLAVE_H
#define IBD_LSH_SLAVE_H

#include "context.h"

#include "filereader.h"
#include "minhasher.h"
#include <queue>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <unordered_map>



class LSH_Slave{
    Context * context;
    std::mutex *linesLock;
    std::queue<std::string*> *linesQ;
    std::unordered_map<uint32_t ,std::vector<std::string> > ** lsh_reverse_corpus;
    std::mutex *lsh_locks;

    std::unordered_map<std::string,std::unordered_map<std::string,std::vector<unsigned> > > * aggregate_map;
    std::mutex *aggLock;
    bool * run_flag;
public:
    LSH_Slave(Context *,std::mutex *,std::queue<std::string*> *,std::unordered_map<uint32_t ,std::vector<std::string> > **
            ,std::mutex *,std::unordered_map<std::string,std::unordered_map<std::string,std::vector<unsigned> > >*
            ,std::mutex * ,bool *);
    void run();
    inline void corpus_generator(std::unordered_map<uint32_t ,std::vector<std::string> > **
            ,uint32_t ** , std::unordered_map<std::string,unsigned short> *, unsigned ,std::string*);
    inline void aggregator(std::unordered_map<std::string,std::unordered_map<std::string,std::vector<unsigned> > >*
            ,std::unordered_map<std::string,unsigned short> *, unsigned ,std::string);
};

#endif //IBD_LSH_SLAVE_H
