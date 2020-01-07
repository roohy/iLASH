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

//Captures one line in map file
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
    this->minhash_threshold = 55;

}

//Reads the map file from address. Please refere to PLINK PED/MAP format specification. 
void Context::read_map(const char * map_addr) {
    ifstream maps(map_addr,ifstream::in);
    if(maps.fail()){
        throw FileException();
    }
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
    cout<<"Loaded "<<this->map_data.size()<<" lines of SNP data."<<endl;
    return;

}
//After loading the map file, these slicing function can divide the SNPs into windows. 
void Context::auto_slice_map(double min_length,double cm_overlap) {
    if(!this->map_flag){ //check to see if map file is loaded.
        cout<<"No Map file is available"<<endl;
        throw "No Map file read before...";
    }
    min_length -= 0.1;
    unsigned base = 0,last=0; //Pointers to the begining and the end of the current slice
    unsigned overlap_point=0; //pointer to keep track of the shingle overlap
    unsigned shingle_i = 0;


    while(last+this->shingle_size < this->map_data.size()){ //check to make sure we are not overflowing
        last  = last + 1; 
        if(this->map_data[last].chrome != this->map_data[base].chrome){ //check to see if chromosome changes. NOT SUPPORTED YET!
            cout<<"We don't support the multiple chromosome runs yet"<<endl;
            break;
        }
        else if(this->map_data[last].gen_dist-this->map_data[base].gen_dist >= min_length){ 
            cout<<"Slice detected: "<<base<<"-"<<last<<"; Dist: "<<this->map_data[last].gen_dist-this->map_data[base].gen_dist<<endl;
            this->slice_idx.push_back(make_pair(base,last)); //adds each slice's coordinates to the list.
            base = overlap_point;



        }
        if(this->map_data[last].gen_dist-this->map_data[base].gen_dist >= min_length-cm_overlap && overlap_point <= base){
            overlap_point = last; //handling the overlap
        }
    }
    if(base == last){ //there are 2 ways we can reach the end of a chromosome, this part addresses both of them. 
        this->slice_idx[this->slice_idx.size()-1].second = this->map_data.size();
    }else{
        this->slice_idx.push_back(make_pair(base,this->map_data.size()));
    }
    cout<<"Number of slices:"<<this->slice_idx.size()<<'\n';
    this->slice_count = this->slice_idx.size();
    //This part breaks up each slice into a set of shingle coordinates. See Context::shingles_in_slice function
    unsigned temp_shingle_count;
    for(unsigned k = 0 ; k < this->slice_count ; k++){
        temp_shingle_count = this->shingles_in_slice(this->slice_idx[k].first,this->slice_idx[k].second);
        this->shingle_idx.push_back(make_pair(shingle_i,shingle_i+temp_shingle_count));
        if(this->slice_idx[k].second-this->slice_idx[k].first > this->minhash_threshold)
            this->minhashable.push_back(true);
        else
            this->minhashable.push_back(false);
        shingle_i += temp_shingle_count;
    }
    cout<<"Done with slicing.\n";

}

void Context::slice_map(unsigned slice_size, unsigned step_size) {
    if(!this->map_flag){
        cout<<"No Map file is available!"<<endl;
        throw "No Map file read before...";
    }
    this->slice_size = slice_size;
    this->step_size = step_size;

    unsigned i = 0;
    unsigned shingle_i = 0;
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

    unsigned temp_shingle_count;
    for(unsigned k = 0 ; k < this->slice_count ; k++){
        temp_shingle_count = this->shingles_in_slice(this->slice_idx[k].first,this->slice_idx[k].second);
        this->shingle_idx.push_back(make_pair(shingle_i,shingle_i+temp_shingle_count));
        shingle_i += temp_shingle_count;
    }
    cout<<"Done with slicing\n";
}

void Context::prepare_for_minhash(unsigned perm_count,long long seed) {
    this->perm_count = perm_count;
    /* if(shingle_size%8 != 0){
        this->p_shingle_size = (shingle_size/8)+1;
    }
    else{
        this->p_shingle_size = shingle_size/8;
    }*/

    this->perm_matrix = new pair<unsigned long, unsigned long>*[this->slice_count];

    default_random_engine generator(seed);
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
    cout<<"Approximating for "<<this->threshold<<" for interest and "<<this->match_thresh<<" for Match\n";
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
    if(this->minimum_match == 0)
        this->minimum_match = this->bucket_count+1;

    cout<<"Interest T:"<<this->minimum_interest<<"\n Match T:"<<this->minimum_match<<endl;
}

void Context::prepare_context(RunOptions * runOptions) {
    this->read_map(runOptions->map_addr.c_str());
    this->auto_slice = runOptions->auto_slice;
    this->shingle_size = runOptions->shingle_size;
    this->shingle_overlap = runOptions->shingle_overlap;
    if(auto_slice){
        this->auto_slice_map(runOptions->slice_length,runOptions->cm_overlap);
    }else{
        this->slice_map(runOptions->slice_size,runOptions->step_size);
    }
    cout<<"The seed used for pseudo-random number generation is:"<<runOptions->seed<<endl;
    this->prepare_for_minhash(runOptions->perm_count,runOptions->seed);
    this->bucket_count = runOptions->bucket_count;
    this->bucket_size = runOptions->perm_count/bucket_count;
    this->threshold = runOptions->interest_threshold;

    this->match_thresh = runOptions->match_threshold;

    this->minimum_length = runOptions->minimum_length;
    this->max_error = runOptions->max_error;

    this->minhash_threshold = runOptions->minhash_threshold;

    this->approximate();

    this->thread_count = std::thread::hardware_concurrency();
    cout<<"The number of cores on the host: "<<this->thread_count<<endl;
    this->thread_count *= 2;
    if(runOptions->max_thread > 0 )
        this->thread_count = runOptions->max_thread;



}


bool Context::same_chrom(unsigned long &i1, unsigned long &i2) {
    return (this->map_data[i1].chrome == this->map_data[i2].chrome);
}

bool Context::is_last_slice(unsigned slice_number) {
    return !(slice_number < this->slice_idx.size()-1 && (this->map_data[this->slice_idx[slice_number].first].chrome
                                                  == this->map_data[this->slice_idx[slice_number+1].first].chrome));
}
bool Context::is_first_slice(unsigned slice_number){
    return !(slice_number > 0 && (this->map_data[this->slice_idx[slice_number].first].chrome
                           == this->map_data[this->slice_idx[slice_number-1].first].chrome));

}
//Counts the number of shingles and creates a map of beginning and endings
unsigned Context::shingles_in_slice(unsigned long first, unsigned long last) {
    unsigned counter = 0 ;
    while(first+this->shingle_size < last){
        counter++;
        this->shingle_map.push_back(make_pair(first,first+shingle_size));
        first += this->shingle_size-this->shingle_overlap;
    }
    if(first<last){
        this->shingle_map.push_back(make_pair(first,last));
        counter++;
    }

    return counter;
}
