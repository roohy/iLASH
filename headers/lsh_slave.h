//
// Created by Ruhollah Shemirani on 3/29/17.
//

#ifndef IBD_LSH_SLAVE_H
#define IBD_LSH_SLAVE_H

#include "context.h"
#include "corpus.h"

#include "filereader.h"
#include "minhasher.h"
#include <queue>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <map>



class LSH_Slave{
    Corpus * corpus;
    std::mutex *linesLock;
    std::queue<std::string*> *linesQ;
    bool * runFlag;
public:
    LSH_Slave(Corpus *,std::mutex *,std::queue<std::string*> * ,bool *);
    void run();
    inline void corpus_generator( uint32_t ** , std::map<uint32_t,unsigned short> *, unsigned ,uint32_t *);
    inline void aggregator(std::map<uint32_t,unsigned short> *, unsigned ,uint32_t *);
};

#endif //IBD_LSH_SLAVE_H
