//
// Created by Ruhollah Shemirani on 3/29/17.
//

#include "headers/filereader.h"

#include <string>
#include <sstream>
#include "headers/fnv.h"
#include "headers/context.h"
#include <iostream>


using namespace std;
filereader::filereader(std::string * input_string,Context * context):iss(*input_string) {
    this->source = input_string;
    this->context = context;

    for(unsigned i= 0 ; i<meta_size; i++){
        this->iss>>this->meta[i];
    }
    this->ind = 0;
    this->shingle_ind = 0;
    //Initializing the memory required for the haplotype analysis in LSH steps. 
    //bits will hold the real SNP values
    this->bits = new dnabit*[2]; 
    this->bits[0] = new dnabit[this->context->map_data.size()];
    this->bits[1] = new dnabit[this->context->map_data.size()];
    //dna hash will hold tokenized SNPs that are turn into hashes
    this->dna_hash = new uint32_t*[2];
    this->dna_hash[0] = new uint32_t[this->context->shingle_map.size()];
    this->dna_hash[1] = new uint32_t[this->context->shingle_map.size()];

    this->hash_buffer = new uint32_t[2];
    dnabit temp_bits;

    for(unsigned i = 0 ; i < this->context->map_data.size(); i++){
        this->iss>>this->bits[0][i];
        this->iss>>this->bits[1][i];
        if(this->iss.tellg()==-1){
            //throw exception
            throw DimensionException();
        }
        
    }
    this->iss>>temp_bits;
    if(this->iss.tellg() != -1){
        throw DimensionException();
    }
    delete input_string;
}



uint32_t * filereader::getNextHashed() {
    //this->hash_buffer[0] = FNV::fnv1a(&(this->bits[0][head]),(size_t)(this->ind-this->head));
    //this->hash_buffer[1] = FNV::fnv1a(&(this->bits[1][head]),(size_t)(this->ind-this->head));
    this->hash_buffer[0] = FNV::fnv1a(&(this->bits[0][this->context->shingle_map[this->shingle_ind].first]),(size_t)(this->context->shingle_map[this->shingle_ind].second-this->context->shingle_map[this->shingle_ind].first));
    this->hash_buffer[1] = FNV::fnv1a(&(this->bits[1][this->context->shingle_map[this->shingle_ind].first]),(size_t)(this->context->shingle_map[this->shingle_ind].second-this->context->shingle_map[this->shingle_ind].first));

    this->dna_hash[0][this->shingle_ind] = this->hash_buffer[0];
    this->dna_hash[1][this->shingle_ind] = this->hash_buffer[1];
//    cout<<dna_hash[0][this->shingle_ind]<<"---"<<dna_hash[1][this->shingle_ind]<<'\n';
    this->shingle_ind++;
    return this->hash_buffer;

}

bool filereader::hasNext() {
    return (this->shingle_ind < this->context->shingle_idx[slice_number].second);


    //we just started so
    /*if(this->ind == this->last){
        return false;
    }
    else{
        if(this->ind == this->head){
            this->ind += this->context->shingle_size;
        }
            //else this not the first step so we take the head to ind and update the ind
        else{
            this->head = this->ind-this->context->shingle_overlap;
            unsigned long available_steps = this->last-this->head; //how long can be go on
            if(available_steps < context->shingle_size){ //this is the last step
                this->ind = this->last;
            }
            else{
                this->ind = this->head+context->shingle_size;
            }
        }
    }
    return  true;*/
}
//sets the stage for the next slice to get parsed. It uses slice_idx list to the indices for the first and the last SNPs. It uses shingle idx list from context to do the same for shingles.
void filereader::set_slice(unsigned slice_id) {

    this->ind = this->context->slice_idx[slice_id].first;
    this->head = this->context->slice_idx[slice_id].first;
    this->last = this->context->slice_idx[slice_id].second;
    //cout<<"slice "<<slice_id<<endl;
    this->shingle_ind = this->context->shingle_idx[slice_id].first;
    this->slice_number = slice_id;

}
filereader::~filereader() {
    delete[] this->bits[0];
    delete[] this->bits[1];
    delete[] this->bits;

    delete[] this->hash_buffer;
}
//records the hash values of the slice in the corpus memory so that it can come back to them later for more details analysis. 
void filereader::register_to_experiment(Corpus * corpus) {
//
//    this->ids[0] = corpus->register_corpus(this->bits[0],this->meta[1]+"_0");
//    this->ids[1] = corpus->register_corpus(this->bits[1],this->meta[1]+"_1");


    this->ids[0] = corpus->register_corpus(this->dna_hash[0],this->meta[0]+"\t"+this->meta[1]+"_0");
    this->ids[1] = corpus->register_corpus(this->dna_hash[1],this->meta[0]+"\t"+this->meta[1]+"_1");

}
