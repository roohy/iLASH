//
// Created by Ruhollah Shemirani on 3/29/17.
//

#ifndef IBD_CORPUS_H
#define IBD_CORPUS_H

#include "context.h"
#include <unordered_map>


class Corpus{
public:
    std::mutex *LSH_Lock;
    std::unordered_map<uint32_t,std::vector<std::string> > **LSH_MAT;

    std::mutex agg_poiter;
    std::unordered_map<std::string,std::unordered_map<std::string, std::vector<unsigned> > > * agg_ptr;


    std::mutex dna_lock;
    std::vector<dnabit *> dna_data;
    std::unordered_set<std::string,uint32_t> dna_id;
    uint32_t person_counter;

    Context * context;

    void initializer();
    uint32_t register_corpus(dnabit *, std::string);
};


#endif //IBD_CORPUS_H
