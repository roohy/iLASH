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
#include <unordered_map>
#include <memory>


class LSH_Slave{
    Corpus * corpus;
    std::mutex *linesLock;
    std::shared_ptr<std::queue<std::unique_ptr< std::string>>> linesQ;
    std::atomic<bool> * runFlag; //This flag tells the program when to stop listening.
public:
    LSH_Slave(Corpus *corpus, std::mutex *linesLock, std::shared_ptr<std::queue<std::unique_ptr<std::string>>>, std::atomic<bool> *);
    void run(); //When the master thread is ready, it will call this function. This action initiates the process of readding from the queue and parsing it.
    inline void corpus_generator( uint32_t ** , std::unordered_map<uint32_t,unsigned short> *, unsigned ,uint32_t *);
    inline void aggregator(std::unordered_map<uint32_t,unsigned short> *, unsigned ,uint32_t *);
};

#endif //IBD_LSH_SLAVE_H
