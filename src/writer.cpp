//
// Created by Ruhollah Shemirani on 3/30/17.
//

#include "headers/writer.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <algorithm>

using namespace std;

void thread_caller(Writer * writer){
    writer->threaded();
}

Writer::Writer(const char * output_addr, unsigned max_thread, Corpus * corpus):output_file(output_addr,ofstream::out) {

    this->output_addr = output_addr;
    this->max_thread = max_thread;
    this->corpus = corpus;


//    this->bit = this->corpus->agg_ptr->begin();
//    this->lit = this->bit->second.begin();
    this->iter = this->corpus->agg_ptr->begin();
}

void Writer::end_file() {
    this->output_file.close();

}

vector<pair<unsigned ,bool> > * Writer::getlist(uint32_t & p1, uint32_t &p2) {

//    typename std::unordered_map<uint32_t, std::unordered_map<uint32_t ,std::vector<std::pair<unsigned ,bool> > > >::iterator temp_bit;
//    typename std::unordered_map<uint32_t, std::vector<std::pair<unsigned ,bool> > >::iterator temp_lit;
    typename std::unordered_map<uint64_t,std::vector<std::pair<unsigned ,bool> > >::iterator temp_it;
    this->jobLock.lock();
    if(this->iter != this->corpus->agg_ptr->end()){
        temp_it = this->iter;
        this->iter++;
        this->jobLock.unlock();
        p2 = (uint32_t)(temp_it->first & 0xFFFFFFFFLL);
        p1 = (uint32_t)((temp_it->first & 0xFFFFFFFF00000000LL) >> 32);
        return &(temp_it->second);
    }
    else{
        this->jobLock.unlock();
        return NULL;
    }
    /*if(this->bit != this->corpus->agg_ptr->end()){

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
    }*/

    this->jobLock.unlock();
    return NULL;
}


void Writer::output(std::vector<std::pair<unsigned , unsigned > > *matches, uint32_t p1, uint32_t p2) {

    double jaccard;
    unsigned long head;
    unsigned long tail;

    for(auto it=matches->begin(); it != matches->end(); ++it){
        jaccard = this->similarity(p1,p2, it->first, it->second);
        head = this->corpus->context->shingle_map[it->second].second-1;
        tail = this->corpus->context->shingle_map[it->first].first;
        this->printLock.lock();
        this->output_file<<this->corpus->dna_id[p1]<<'\t'<<this->corpus->dna_id[p2]<<'\t'
                         <<this->corpus->context->map_data[tail].chrome<<'\t'
                         <<this->corpus->context->map_data[tail].position
                         <<'\t'<<this->corpus->context->map_data[head].position
                         <<'\t'<<this->corpus->context->map_data[tail].RSID<<'\t'
                         <<this->corpus->context->map_data[head].RSID<<'\t'
                         <<this->corpus->context->map_data[head].gen_dist
                         -this->corpus->context->map_data[tail].gen_dist<<'\t'
                         <<jaccard<<'\n';
        this->printLock.unlock();
    }


}

inline double Writer::similarity(uint32_t p1, uint32_t p2, unsigned  st, unsigned  end){
    unsigned count= 0;
    unsigned correct = 0;
    for( unsigned long i = st; i < end; i++){
        count++;
        if(this->corpus->dna_hashes[p1][i] == this->corpus->dna_hashes[p2][i])
            correct++;


    }

    return ((double)correct)/((double)count);
}

void Writer::run() {
    cout<<"Starting Writer Threads...\n";
    vector<thread> thread_list;
//    this->max_thread = 1;
    for(unsigned i = 0 ; i < this->max_thread ; i++){
        thread_list.push_back(thread(thread_caller,this));
    }
    cout<<"All Threads Working\n";
    for(unsigned i = 0 ; i < this->max_thread ; i++){
        thread_list[i].join();
    }
    cout<<"Everything done in the writer part\n";
}

void Writer::threaded() {
    uint32_t p1;
    uint32_t p2;
    vector<pair<unsigned , bool> > * matches;
    matches = this->getlist(p1,p2);
    vector<pair<unsigned , unsigned > > * result =  new vector<pair<unsigned , unsigned > >();
    //cout<<"Thread Starting to match\n";
    while(matches != NULL) {
        result->clear();
        //sort it

        sort(matches->begin(), matches->end()); //uses a combination of quick sort and other methods to sort
        unsigned head = 0, tail = 0;
        unsigned short rem_error = 0;//this->corpus->context->max_error; TODO: add rem_error
        //unsigned  maximum_bound = 0;
        //bool ready = false;
        //now that it is sortet, we through with it
        for (auto it = matches->begin(); it != matches->end(); ++it) {

            //we have to see first where the last head is compared to our position
            if(head < this->corpus->context->shingle_idx[it->first].first-1){ //this means we are not ready for this part. or we are just begining to work or something.
                this->shingleSafeAdd(head,tail,result); //adding the results of the past iterations (TODO: bug for shingles bigger than min match length)
                if(this->corpus->context->is_first_slice(it->first)){ //if we are the first slice we just use the slice as the tail
                   head = tail =  this->corpus->context->shingle_idx[it->first].first;

                }
                else{ //if not the first slice we can back extend

                    tail = this->shingle_back_extend(this->corpus->context->shingle_idx[it->first].first,head,rem_error,p1,p2)+1;
                    head = this->corpus->context->shingle_idx[it->first].first;
                }
            }// now we have a good tail
            if(it->second ){ //this is for extention of head. When there is a match we just add it as a whole.

                head = this->corpus->context->shingle_idx[it->first].second-1;
            }
            else{


                head = this->shingle_extend(head,this->corpus->context->shingle_idx[it->first].second,rem_error,p1,p2); //optimization


                while(head < this->corpus->context->shingle_idx[it->first].second){

                    if(head > tail)
                        this->shingleSafeAdd(head-1,tail,result);
                    head = tail = head+1;

                    head = this->shingle_extend(head,this->corpus->context->shingle_idx[it->first].second,rem_error,p1,p2);

                }

                if(head > tail )
                    head--;
                else {
                    head = tail =  this->corpus->context->shingle_idx[it->first].second-1;
                }

            }
            if(it+1 != matches->end()){

                if((it+1)->first != it->first+1){
                        head = this->shingle_extend(head,this->corpus->context->shingle_idx[it->first+1].second,rem_error,p1,p2)-1;
                }
            }else{

                if(this->corpus->context->is_last_slice(it->first)){

                    this->shingleSafeAdd(head,tail,result);

                }else{
                    head = this->shingle_extend(head,this->corpus->context->shingle_idx[it->first+1].second,rem_error,p1,p2)-1;
                    this->shingleSafeAdd(head,tail,result);
                }
            }

        }
        this->output(result, p1, p2);
        matches = this->getlist(p1, p2);

    }



    /*uint32_t p1;
    uint32_t p2;
    vector<pair<unsigned , bool> > * matches;
    matches = this->getlist(p1,p2);
    vector<pair<unsigned long, unsigned long> > * result =  new vector<pair<unsigned long, unsigned long> >();
    //cout<<"Thread Starting to match\n";
    while(matches != NULL) {
        result->clear();
        //sort it
        //cout<<"Matching"<<p1<<"---"<<p2<<'\n';
        sort(matches->begin(), matches->end()); //uses a combination of quick sort and other methods to sort
        unsigned long head = 0, tail = 0;
        unsigned short rem_error = this->corpus->context->max_error;
        unsigned long maximum_bound = 0;
        bool ready = false;
        //now that it is sortet, we through with it
        for (auto it = matches->begin(); it != matches->end(); ++it) {
            //check to see if the last chunk has done something for us
            if (!ready) {
                //if it is ready it means head and tail are valid
                //if not ready it means we shuold make them valid

                head = tail = this->corpus->context->slice_idx[it->first].first;
                if (!this->corpus->context->is_first_slice(it->first)) { // if it is not the first slice on a chromosome
                    tail = this->back_extend(tail, this->corpus->context->slice_idx[it->first - 1].first, rem_error,p1,p2)+1;
                    //if it is not a first so it has a back
                }
            } // now we have a valid head an tail at the begining.
            //we should see if it is a match or not.

            if (it->second) {

                head = this->corpus->context->slice_idx[it->first].second - 1;
                if ((it + 1) != matches->end() && !this->corpus->context->is_last_slice(it->first)) {
                    if ((it + 1)->first == it->first + 1) {
                        ready = true;
                        continue;
                    }
                    else {
                        //cout<<"ha\n";
                        head = this->extend(head, this->corpus->context->slice_idx[it->first + 1].second, rem_error,p1,p2)-1;
                        this->safeAdd(head, tail, result);
                        ready = false;
                    }
                }
                else {
                    if (this->corpus->context->is_last_slice(it->first)) {
                        this->safeAdd(head, tail, result);
                    }
                    else {
                        head = this->extend(head, this->corpus->context->slice_idx[it->first + 1].second, rem_error,p1,p2)-1;
                        this->safeAdd(head,tail,result);
                    }
                    ready = false;
                }
            }
            else {
                if ((it + 1) != matches->end() && !this->corpus->context->is_last_slice(it->first)) {
                    if ((it + 1)->first == it->first + 1) {//the next one is 100%
                        maximum_bound = this->corpus->context->slice_idx[(it + 1)->first].first;
                        ready = true;
                    }
                    else {
                        maximum_bound = this->corpus->context->slice_idx[it->first + 1].second;
                        ready = false;
                        if((it+1)->first == it->first+2){
                            ready = true;
                        }
                    }
                }
                else {
                    if (this->corpus->context->is_last_slice(it->first)) {
                        maximum_bound = this->corpus->context->slice_idx[it->first].second;
                    }
                    else {
                        maximum_bound = this->corpus->context->slice_idx[it->first + 1].second;
                    }

                    ready = false;
                }
                head = this->extend(head, maximum_bound, rem_error,p1,p2)-1;
                while (head < maximum_bound - 1) {
                    this->safeAdd(head, tail, result);
                    tail = head + 2;

                    head = this->extend(head, maximum_bound, rem_error,p1,p2)-1;


                }
                if (!ready) {
                    this->safeAdd(head, tail, result);
                }

            }


        }

        this->output(result, p1, p2);
        matches = this->getlist(p1, p2);
    }*/
}


/*inline void Writer::safeAdd(unsigned long head, unsigned long tail, std::vector<std::pair<unsigned long, unsigned long> > *matches) {
    cout<<"Safe add called with "<<head<<"---"<<tail<<'\n';
    if(this->corpus->context->map_data[head].gen_dist - this->corpus->context->map_data[tail].gen_dist >= this->corpus->context->minimum_length){
        //cout<<"owww\n";

        matches->push_back(make_pair(tail,head));
    }
}*/

inline void Writer::shingleSafeAdd(unsigned head, unsigned tail, std::vector<std::pair<unsigned , unsigned > > *matches) {
    if(this->corpus->context->map_data[this->corpus->context->shingle_map[head].second-1].gen_dist - this->corpus->context->map_data[this->corpus->context->shingle_map[tail].first].gen_dist >= this->corpus->context->minimum_length){
        //cout<<"owww\n";
        matches->push_back(make_pair(tail,head));
    }
}

inline unsigned long Writer::extend(unsigned long tail, unsigned long bound, unsigned short error,uint32_t p1,uint32_t p2) {
    for(unsigned long i = tail; i < bound; i++){
        if(this->corpus->dna_data[p1][i] != this->corpus->dna_data[p2][i] ){
            if(error > 0)
                error--;
            else{
                return i;
            }
        }
    }
    return bound;
}

inline unsigned  long Writer::back_extend(unsigned long head, unsigned long bound, unsigned short error, uint32_t p1,
                                          uint32_t p2) {
    for(unsigned long i = head ; i > bound ; i--){
        if(this->corpus->dna_data[p1][i] != this->corpus->dna_data[p2][i]){
            if(error > 0)
                error--;
            else
                return i;
        }
    }
    return bound;
}


inline unsigned  Writer::shingle_back_extend(unsigned head, unsigned bound, unsigned short error, uint32_t p1,
                                             uint32_t p2) {
    while(head > bound){
        if(this->corpus->dna_hashes[p1][head] != this->corpus->dna_hashes[p2][head]){
            if(error > 0 )
                error--;
            else
                return head;

        }
        --head;
    }
    return bound;

}

inline unsigned Writer::shingle_extend(unsigned head, unsigned bound, unsigned short error, uint32_t p1, uint32_t p2) {
    while(head < bound){
        if(this->corpus->dna_hashes[p1][head] != this->corpus->dna_hashes[p2][head]){
            if(error > 0 )
                error--;
            else
                return head;

        }
        ++head;
    }
    return bound;
}