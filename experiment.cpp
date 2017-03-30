//
// Created by Ruhollah Shemirani on 3/29/17.
//

#include "headers/context.h"
#include "headers/experiment.h"
#include "headers/lsh_slave.h"

#include <thread>
#include <iostream>
#include <queue>
#include <mutex>
#include <fstream>

using namespace std;



void Experiment::set_context(Context context) {
    this->context = context;
}

void Experiment::setup_context(const char * map_addr, unsigned slice_size, unsigned step_size
        , unsigned perm_count, unsigned shingle_size
        , unsigned shingle_overlap, unsigned bucket_count
        ,double threshold,double match_threshold,double minimum_length, unsigned short max_error) {

    this->context.prepare_context(map_addr,slice_size, step_size,perm_count,shingle_size,shingle_overlap
            ,bucket_count,threshold,match_threshold,minimum_length,max_error);
    this->corpus.initializer(&(this->context));

}

