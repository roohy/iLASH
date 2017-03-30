//
// Created by Ruhollah Shemirani on 3/29/17.
//


#include "headers/context.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <random>

using namespace std;


MapData::MapData(string chrom_in, string RSID_in, double dist_in, unsigned pos_in) {
    this->chrome = chrom_in;
    this->RSID = RSID_in;
    this->gen_dist = dist_in;
    this->position = pos_in;
}

Context::Context()
        :map_data(),slice_idx() {
    this->map_flag = false;
    this->minimum_match = 0;

}


void Context::read_map(const char * map_addr) {
    ifstream maps(map_addr,ifstream::in);
    string chrom;
    string RSID_in;
    double dist_in;
    unsigned pos_in;

    while(maps>>chrom){
        maps>>RSID_in;
        maps>>dist_in;
        maps>>pos_in;
        this->map_data.push_back(MapData(chrom,RSID_in,dist_in,pos_in));
    }

    this->map_flag = true;
    cout<<"Read "<<this->map_data.size()<<" lines"<<endl;
    return;

}

void Context::slice_map(unsigned slice_size, unsigned step_size) {
    if(!this->map_flag){
        cout<<"No Map file is available"<<endl;
        throw "No Map file read before...";
    }
    this->slice_size = slice_size;
    this->step_size = step_size;

    unsigned i = 0;
    for(; i+slice_size < this->map_data.size();i++){
        if( this->map_data[i].chrome != this->map_data[i+slice_size].chrome ){
            cout<<"Chromosomes colliding in one slice.Don't Worry. I am preventing it!!!"<<endl;
            unsigned j = i;
            while(this->map_data[i].chrome == this->map_data[j].chrome){
                j++;
            }
            this->slice_idx.push_back(pair<long,long>(i,j));
            i = j;
            continue;
        }

        this->slice_idx.push_back(pair<long,long>(i,i+slice_size));
        i += step_size;
    }

    if( i < this->map_data.size() && this->map_data.size()-i > this->shingle_size){
        this->slice_idx.push_back(pair<long,long>(i,this->map_data.size()));
    }
    cout<<"Number of slices:"<<this->slice_idx.size()<<'\n';
    this->slice_count = this->slice_idx.size();
}

void Context::prepare_for_minhash(unsigned perm_count, unsigned shingle_size, unsigned shingle_overlap) {
    this->perm_count = perm_count;
    this->shingle_size = shingle_size;
    this->shingle_overlap = shingle_overlap;
    /* if(shingle_size%8 != 0){
        this->p_shingle_size = (shingle_size/8)+1;
    }
    else{
        this->p_shingle_size = shingle_size/8;
    }*/

    this->perm_matrix = new pair<unsigned long, unsigned long>*[this->slice_count];

    default_random_engine generator;
    uniform_int_distribution<unsigned long> distribution(0,this->word_count);

    for(unsigned i  = 0 ; i < this->slice_count; i++){
        this->perm_matrix[i] = new pair<unsigned long,unsigned long>[this->perm_count];
        for (unsigned j = 0 ; j < this->perm_count; j++){
            this->perm_matrix[i][j].first = distribution(generator);
            this->perm_matrix[i][j].second = distribution(generator);
        }
    }
    return;

}

void Context::approximate() {
    this->approx_list = new double[this->bucket_count+1];
    double b = 1.0/double(this->bucket_size);
    this->minimum_match = 0;
    this->minimum_interest = 0;
    cout<<"Approximating\n";
    for(double i = 1 ; i < this->bucket_count+1; i++){

        this->approx_list[unsigned (i)] = pow(i/double(this->bucket_count),b);
        cout<<i<<"/"<<this->bucket_count<<": "<<this->approx_list[unsigned (i)]<<'\n';
        if(this->approx_list[unsigned (i)] >= this->threshold && this->minimum_interest == 0){
            this->minimum_interest = unsigned(i);
        }
        if(this->approx_list[unsigned (i)] >= this->match_thresh && this->minimum_match == 0){
            this->minimum_match = unsigned(i);
        }
    }

    return;
}

void Context::prepare_context(const char *map_addr, unsigned slice_size,unsigned step_size, unsigned perm_count, unsigned shingle_size
        ,unsigned shingle_overlap, unsigned bucket_count,double thresh,double match_threshold
        ,double minimum_length, unsigned short max_error) {
    this->read_map(map_addr);
    this->slice_map(slice_size,step_size);
    this->prepare_for_minhash(perm_count,shingle_size,shingle_overlap);
    this->bucket_count = bucket_count;
    this->bucket_size = perm_count/bucket_count;
    this->threshold = thresh;
    this->approximate();
    this->match_thresh = match_threshold;
    this->minimum_length = minimum_length;
    this->max_error = max_error;
}