//
// Created by Ruhollah Shemirani on 3/30/17.
//

#include "headers/writer.h"
#include <iostream>
#include <fstream>

using namespace std;

Writer::Writer(const char * output_addr, unsigned max_thread, Corpus * corpus):output_file(output_addr,ofstream::out) {
    this->output_addr = output_addr;
    this->max_thread = max_thread;
    this->corpus = corpus;


    this->bit = this->corpus->agg_ptr->begin();
    this->lit = this->bit->second.begin();
}

void Writer::end_file() {
    this->output_file.close();

}

vector<pair<unsigned ,bool> > * Writer::getlist(uint32_t & p1, uint32_t &p2) {

    typename std::unordered_map<uint32_t, std::unordered_map<uint32_t ,std::vector<std::pair<unsigned ,bool> > > >::iterator temp_bit;
    typename std::unordered_map<uint32_t, std::vector<std::pair<unsigned ,bool> > >::iterator temp_lit;
    this->jobLock.lock();

    if(this->bit != this->corpus->agg_ptr->end()){

        if(this->lit != this->bit->second.end()){
            temp_lit = this->lit;
            this->lit++;
            p1 = this->bit->first;
            this->jobLock.unlock();
            p2 = temp_lit->first;
            return &(temp_lit->second);
        }else{
            this->bit++;
            if(this->bit == this->corpus->agg_ptr->end()){
                this->jobLock.unlock();
                return NULL;
            }
            else{
                this->lit = this->bit->second.begin();
                temp_lit = this->lit;
                this->lit++;
                p1 = this->bit->first;
                this->jobLock.unlock();
                p2 = temp_lit->first;
                return &(temp_lit->second);
            }
        }
    }
    else{
        this->jobLock.unlock();
        return NULL;
    }

    this->jobLock.unlock();
    return NULL;
}


void Writer::output(std::vector<std::pair<unsigned long, unsigned long> > *matches, uint32_t p1, uint32_t p2) {

    this->printLock.lock();
    for(auto it=matches->begin(); it != matches->end(); ++it){
        //this->output_file<<this->corpus.
    }

    this->printLock.unlock();
}