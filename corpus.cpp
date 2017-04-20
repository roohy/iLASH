//
// Created by Ruhollah Shemirani on 3/29/17.
//

#include "headers/context.h"
#include "headers/corpus.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <unordered_map>

using namespace std;

void Corpus::initializer(Context *context) {
    this->context = context;
    this->person_counter = 0;
    this->LSH_MAT = new unordered_map<uint32_t, vector<uint32_t > > *[this->context->slice_count];
    this->LSH_Lock = new mutex[this->context->slice_size];
    for(unsigned i = 0 ; i < this->context->slice_count; i++){
        this->LSH_MAT[i] = new unordered_map<uint32_t ,vector<uint32_t> >[this->context->bucket_count];
    }
    this->agg_ptr
            = new std::unordered_map<uint32_t ,unordered_map<uint32_t , vector<pair<unsigned,bool> > > >();
}

uint32_t Corpus::register_corpus(dnabit * person_dna_data, std::string string_id) {
    this->dna_lock.lock();
    uint32_t id = this->person_counter;
    this->dna_data.push_back(person_dna_data);
    this->dna_id[id] = string_id;
    this->person_counter++;
    this->dna_lock.unlock();
    //cout<<"ID:"<<id<<"\n";
    return id;
}

uint32_t Corpus::register_corpus(uint32_t * person_hash, std::string string_id) {
    this->dna_lock.lock();
    uint32_t id = this->person_counter;
    this->dna_hashes.push_back(person_hash);
    this->dna_id[id] = string_id;
    this->person_counter++;
    this->dna_lock.unlock();

    return id;
}

void Corpus::add_to_corpus(uint32_t *hash_val, uint32_t id, unsigned slice_num
        , std::unordered_map<uint32_t, unsigned short> * relatives) {

    this->LSH_Lock[slice_num].lock();
    for(unsigned k = 0 ; k < this->context->bucket_count; k++){
        if(this->LSH_MAT[slice_num][k].find(hash_val[k]) == this->LSH_MAT[slice_num][k].end()){
            this->LSH_MAT[slice_num][k][hash_val[k]] = vector<uint32_t>();
            this->LSH_MAT[slice_num][k][hash_val[k]].push_back(id);
        }
        else{
            for(unsigned i = 0 ; i < this->LSH_MAT[slice_num][k][hash_val[k]].size(); i++){
                if(relatives[0].find(this->LSH_MAT[slice_num][k][hash_val[k]][i]) == relatives[0].end()){
                    relatives[0][this->LSH_MAT[slice_num][k][hash_val[k]][i]] = 1;
                }else{
                    relatives[0][this->LSH_MAT[slice_num][k][hash_val[k]][i]] += 1;
                }
            }
            this->LSH_MAT[slice_num][k][hash_val[k]].push_back(id);
        }

    }
    this->LSH_Lock[slice_num].unlock();

}

void Corpus::integrate(std::unordered_map<uint32_t, unsigned short> *relatives, uint32_t id, unsigned slice_number) {
    this->agg_poiter.lock();
    uint32_t mini;
    uint32_t maxi;
    for(auto it = relatives->begin(); it!=relatives->end(); ++it){
        if(it->second >= this->context->minimum_interest){
            mini = min(id,it->first);
            maxi = max(id,it->first);
            if(this->agg_ptr->find(mini) == this->agg_ptr->end()){
                this->agg_ptr[0][mini] = unordered_map<uint32_t ,vector<pair<unsigned,bool> > >();
                this->agg_ptr[0][mini][maxi] = vector<pair<unsigned , bool> >();
                this->agg_ptr[0][mini][maxi].push_back(make_pair(slice_number,(it->second >= this->context->minimum_match)));

            }else{
                if(this->agg_ptr[0][mini].find(maxi) == this->agg_ptr[0][mini].end()){
                    this->agg_ptr[0][mini][maxi] = vector<pair<unsigned,bool> >();
                    this->agg_ptr[0][mini][maxi].push_back(make_pair(slice_number,  (it->second >= this->context->minimum_match) ));
                    //this->agg_ptr[0][mini][maxi].push_back(make_pair(slice_number,((it->second >= this->context->minimum_match) && this->are_the_same(mini,maxi,slice_number))   ));
                }
                else{
                    this->agg_ptr[0][mini][maxi].push_back(make_pair(slice_number,(it->second >= this->context->minimum_match)));
                }
            }
        }
    }

    this->agg_poiter.unlock();
}

bool Corpus::are_the_same(uint32_t id1, uint32_t id2, unsigned slice_num) {

    for(unsigned long i = this->context->slice_idx[slice_num].first; i < this->context->slice_idx[slice_num].second ; i++){
        if(this->dna_data[id1][i] != this->dna_data[id2][i]){
            //cout<<id1<<"  "<<id2<<"--"<<slice_num<<"NOTTTTT\n";
            return false;
        }

    }
    return true;
}