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
        this->output_file<<this->corpus->dna_id[p1]<<'\t'<<this->corpus->dna_id[p2]<<'\t'
                         <<this->corpus->context->map_data[it->first].position
                         <<'\t'<<this->corpus->context->map_data[it->second].position
                         <<'\t'<<this->corpus->context->map_data[it->first].RSID<<'\t'
                         <<this->corpus->context->map_data[it->second].RSID<<'\t'
                         <<this->corpus->context->map_data[it->second].gen_dist
                         -this->corpus->context->map_data[it->first].gen_dist<<"cM"<<'\n';
    }

    this->printLock.unlock();
}

void Writer::run() {
    cout<<"Starting Writer Threads...\n";
    vector<thread> thread_list;
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
    vector<pair<unsigned long, unsigned long> > * result =  new vector<pair<unsigned long, unsigned long> >();
    //cout<<"Thread Starting to match\n";
    while(matches != NULL) {
        result->clear();
        //sort it
//        cout<<"Matching"<<p1<<"---"<<p2<<'\n';
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


            /*     //check if it is a match or just an interest
                 if(it->second){
                     //check for overlap with the last one
                     if(head >= this->corpus->context->slice_idx[it->first].first-1 ) {
                         //if an overlap add yourself to it. (it will already be on the same chromosome)
                         head = this->corpus->context->slice_idx[it->first].second;


                         //now check to see what happens to the next one. is it your neighbour?

                         //is there a next one
                         if((it+1) != matches->end()){

                             //is it right beside us in the slice list
                             if((it+1)->first == it->first+1){ //we maybe right beside each other!!!

                                 if(this->corpus->context->same_chrom(this->corpus->context->slice_idx[it->first].first,this->corpus->context->slice_idx[it->first+1].first)){
                                     //here we know we are neighbours with the next slice. and we are on the same chromosome
                                     //we know we are going to be a match right up to a good point, because of the overlap.
                                     //with no overlap, the conditions still stands. because we are neighbouring chromosomes
                                     //right next to each other. So we can go on and see what happens next.
                                     continue;
                                 }
                                 else{
                                     //we are not on the same chromosome so we add ourselves to the result vector
                                     //via safe add function, then we reset the head and tail and errors for the next one
                                     this->safeAdd(head,tail,result);
                                     head = tail = this->corpus->context->slice_idx[it->first+1].first;
                                     rem_error = this->corpus->context->max_error;
                                 }
                             }
                             else{
                                 //we have to extend ourselves and just go
                                 //we know there is a next one, we know it is not our neighbour
                                 //we try to extend current one and then back-extend the next one
                                 //we check to see if we share the chromosome with the next one
                                 if(this->corpus->context->same_chrom(this->corpus->context->slice_idx[it->first].first,this->corpus->context->slice_idx[it->first+1].first)){
                                     head = this->extend(head,this->corpus->context->slice_idx[it->first+1].second,rem_error);
                                 }
                                 this->safeAdd(head,tail,result);
                                 head = this->corpus->context->slice_idx[(it+1)->first].first;
                                 if(!this->corpus->context->is_first_slice((it+1)->first)){
                                     //we can and should extend the next one backwards
                                     tail = this->back_extend(head,this->corpus->context->slice_idx[(it+1)->first-1].first,rem_error);

                                 }
                                 else{
                                     tail = head;
                                 }
                                 //we have nothing to do more...

                             }
                         }
                         else{
                             //there is no next one
                             //we just extend this one and leave it at that.
                             //we first check to see if we are the last slice
                             if(!this->corpus->context->is_last_slice(it->first)){
                                 head = this->extend(head,this->corpus->context->slice_idx[it->first+1].second,rem_error);
                             }
                             this->safeAdd(head,tail,result);
                         }

                     }
                     else{ // no overlap
                         cout<<"somthing is wrong\n";
                     }

                 }//it is not a full match.
                 else{

                 }
             }


     */



        }

        this->output(result, p1, p2);
        matches = this->getlist(p1, p2);
    }
}


inline void Writer::safeAdd(unsigned long head, unsigned long tail, std::vector<std::pair<unsigned long, unsigned long> > *matches) {
    if(this->corpus->context->map_data[head].gen_dist - this->corpus->context->map_data[tail].gen_dist >= 0.7){
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