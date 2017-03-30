//
// Created by Ruhollah Shemirani on 3/29/17.
//

#include "headers/minhasher.h"
#include <set>
#include <vector>
#include "headers/context.h"
#include "headers/fnv.h"
#include <climits>
using  namespace std;


Minhasher::Minhasher(unsigned perm_count,Context * context) {
    this->permutation_count = perm_count;
    this->corpus = new set<uint32_t>[2]();

    this->context = context;
    this->slice_num = 0;

    this->minhashed = new unsigned long *[2];
    this->minhashed[0] = new unsigned long[this->permutation_count];
    this->minhashed[1] = new unsigned long[this->permutation_count];

    this->lsh_mat = new uint32_t *[2];
    this->lsh_mat[0] = new uint32_t[this->context->bucket_count];
    this->lsh_mat[1] = new uint32_t[this->context->bucket_count];

}



