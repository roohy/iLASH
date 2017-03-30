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

//initializing

Minhasher::Minhasher(Context * context) {

    //redundant
    this->permutation_count = context->perm_count;
    this->corpus = new set<uint32_t>[2]();

    this->context = context;
    this->slice_num = 0;

    //allocating memory
    this->minhashed = new unsigned long *[2];
    this->minhashed[0] = new unsigned long[this->permutation_count];
    this->minhashed[1] = new unsigned long[this->permutation_count];

    this->lsh_mat = new uint32_t *[2];
    this->lsh_mat[0] = new uint32_t[this->context->bucket_count];
    this->lsh_mat[1] = new uint32_t[this->context->bucket_count];

}



//this function inserts a new hashed shingle into the minhash corpus. If it is the first time it will call permute
void Minhasher::insert(uint32_t *hashbuffer) {
    bool is_not_in;
    is_not_in = (this->corpus[0].find(hashbuffer[0])==this->corpus[0].end());
    if(is_not_in){
        this->permute(hashbuffer[0],0);
        this->corpus[0].insert(hashbuffer[0]);
    }
    is_not_in = (this->corpus[1].find(hashbuffer[1])==this->corpus[1].end());
    if(is_not_in){
        this->permute(hashbuffer[1],1);
        this->corpus[1].insert(hashbuffer[1]);
    }
}

//going to a new slice number, we reset corpus and reset minhashed matrix to maximum
void Minhasher::set_slice_num(unsigned slice_num) {
    this->slice_num = slice_num;
    this->corpus[0].clear();
    this->corpus[1].clear();

    for(unsigned i = 0 ; i < 2; i++){
        for(unsigned j = 0 ; j < permutation_count; j++){
            this->minhashed[i][j] = ULONG_MAX;
        }
    }

}

//when get a new word we permute it to see if it will add a new value to minhashed corpus
void Minhasher::permute(uint32_t hash_val, unsigned ind) {
    unsigned long permutation_val;
    for(unsigned i = 0 ; i < this->context->perm_count; i++){
        permutation_val = ((context->perm_matrix[slice_num][i].first*(unsigned long)hash_val) +
                           context->perm_matrix[slice_num][i].second)%context->word_count;
        if(permutation_val < this->minhashed[ind][i]){
            this->minhashed[ind][i] = permutation_val;
        }
    }
}


//After a minhash matrix is completely calculated, you can call this function to calculate LSH for the slice
uint32_t ** Minhasher::calculate_lsh() {
    for(unsigned i = 0 ; i < this->context->bucket_count; i++){
        this->lsh_mat[0][i] = FNV::fnv1a(this->minhashed[0]+(i*this->context->bucket_size)
                ,(size_t)(this->context->bucket_size* sizeof(unsigned long)));

        this->lsh_mat[1][i] = FNV::fnv1a(this->minhashed[1]+(i*this->context->bucket_size)
                ,(size_t)(this->context->bucket_size* sizeof(unsigned long)));
    }
    return this->lsh_mat;
}