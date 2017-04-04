//
// Created by Ruhollah Shemirani on 3/29/17.
//

#include "headers/context.h"
#include "headers/experiment.h"
#include "headers/lsh_slave.h"
#include "headers/writer.h"

#include <thread>
#include <iostream>
#include <queue>
#include <mutex>
#include <fstream>

using namespace std;

void lsh_thread(Corpus *corpus,std::mutex * linesLock,std::queue<std::string*> * linesQ,bool *runFlag){
    LSH_Slave slave(corpus,linesLock,linesQ,runFlag);
    slave.run();

}


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

void Experiment::read_bulk(const char *input_addr, const char *output_addr) {

    unsigned max_threads = std::thread::hardware_concurrency();
    cout<<"The number of cores supported by this machine is "<<max_threads<<endl;
    max_threads += 2;

    queue<string *> *linesQ = new queue<string *>();
    mutex *linesLock = new mutex;
    bool runFlag = true;

    vector<thread> lsh_thread_list;

    for(unsigned i = 0; i < max_threads; i++){
        lsh_thread_list.push_back(thread(lsh_thread,&(this->corpus),linesLock,linesQ,&runFlag));

    }
    cout<<"Threads started"<<endl;

    ifstream ped_file(input_addr,ifstream::in);
    string* local_str_ptr = new string;
    long counter = 0;


    while(getline(ped_file,*local_str_ptr)){
//        cout<<"adding one "<<local_str_ptr->size()<<endl;
        linesLock->lock();
        linesQ->push(local_str_ptr);
        linesLock->unlock();
        local_str_ptr = new string;
        counter++;
        /*if(counter > 10){
            break;
        }*/
    }

    cout<<"Read everything from the file."<<endl;
    while(true){
        linesLock->lock();
        if(linesQ->empty()){
            linesLock->unlock();
            break;
        }
        linesLock->unlock();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    runFlag = false;
    cout<<"Waiting for threads for finish their jobs"<<'\n';
    for(unsigned i = 0 ; i < lsh_thread_list.size();i++){
        lsh_thread_list[i].join();
    }
    cout<<"Writing\n";
    cout<<"---"<<this->corpus.agg_ptr->size()<<"\n";
    this->write_to_file(output_addr);

}


void Experiment::write_to_file(const char *output_addr) {
    unsigned max_threads = std::thread::hardware_concurrency();
    max_threads += 10;
    Writer writer(output_addr,max_threads,&(this->corpus));
    writer.run();
    writer.end_file();
}