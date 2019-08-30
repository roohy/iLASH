//
// Created by Ruhollah Shemirani on 3/29/17.
//


#ifndef IBD_FILEREADER_H
#define IBD_FILEREADER_H

#include <string>
#include <sstream>
#include "context.h"

#include "corpus.h"



//the filereader class is in charge of parsing genotype lines and analysing them. 
//every instance checks 


const unsigned meta_size = 6;

class filereader {
    std::string * source;
    std::istringstream iss;

    unsigned long ind;
    unsigned long last;
    unsigned long head;
    unsigned slice_number;
    Context *context;


public:
    std::string meta[meta_size]; 
    dnabit ** bits; 
    uint32_t *hash_buffer;
    uint32_t ids[2];

    uint32_t ** dna_hash;
    unsigned shingle_ind;

    filereader(std::string *, Context *);
    uint32_t* getNextHashed();
    ~filereader();
    bool hasNext();

    void set_slice(unsigned);

    void register_to_experiment(Corpus *);
};



#endif //IBD_FILEREADER_H
