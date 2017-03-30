//
// Created by Ruhollah Shemirani on 3/29/17.
//

#ifndef IBD_MINHASHER_H
#define IBD_MINHASHER_H

#include <vector>
#include <set>
#include "context.h"

class Minhasher{
    Context *context;
    unsigned permutation_count;
    unsigned long ** minhashed;
    std::set<uint32_t> *corpus;
    unsigned slice_num;
public:
    uint32_t ** lsh_mat;


    Minhasher(Context *);
    void insert(uint32_t * );
    void permute(uint32_t , unsigned);
    void set_slice_num(unsigned);
    uint32_t ** calculate_lsh();
};


#endif //IBD_MINHASHER_H
