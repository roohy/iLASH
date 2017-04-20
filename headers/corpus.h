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
    std::unordered_map<uint32_t,std::vector<uint32_t > > **LSH_MAT;

    std::mutex agg_poiter;
    std::unordered_map<uint32_t,std::unordered_map<uint32_t, std::vector<std::pair<unsigned,bool> > > > * agg_ptr;


    std::mutex dna_lock;
    std::vector<dnabit *> dna_data;
    std::vector<uint32_t *> dna_hashes;
    std::unordered_map<uint32_t,std::string> dna_id;
    uint32_t person_counter;

    Context * context;

    void initializer(Context *);
    uint32_t register_corpus(dnabit *, std::string);
    uint32_t register_corpus(uint32_t *, std::string);

    void add_to_corpus(uint32_t *,uint32_t, unsigned, std::unordered_map<uint32_t,unsigned short> *);
    void integrate(std::unordered_map<uint32_t,unsigned short> *,uint32_t,unsigned);
    bool are_the_same(uint32_t id1,uint32_t id2,unsigned slice_num);
};


#endif //IBD_CORPUS_H
