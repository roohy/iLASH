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

//Thread handle function for LSH worker threads in a master-slave model.
//Instantiates a LSH slave instance and then runs it.
//All the inputs are the required inputs for LSH threads.
void lsh_thread(Corpus *corpus,std::mutex * linesLock,std::queue<std::string*> * linesQ,bool *runFlag){
    LSH_Slave slave(corpus,linesLock,linesQ,runFlag);
    slave.run();
}

//Context get-set. Context objects can also be imported from a RunOptions instance using the setup_context functions.
void Experiment::set_context(Context context) {
    this->context = context;
}

//This function is deprecated. It is just here for code continuity reasons.
void Experiment::setup_context(const char * map_addr, unsigned slice_size, unsigned step_size
        , unsigned perm_count, unsigned shingle_size
        , unsigned shingle_overlap, unsigned bucket_count
        ,double threshold,double match_threshold,double minimum_length, unsigned short max_error, bool auto_slice,double cm_overlap) {

    this->context.prepare_context(map_addr,slice_size, step_size,perm_count,shingle_size,shingle_overlap
            ,bucket_count,threshold,match_threshold,minimum_length,max_error,auto_slice,cm_overlap);
    this->corpus.initializer(&(this->context));

}

//Using a RunOptions instance, this function initializes the Context of the experiment. Please read the context class docs.
void Experiment::setup_context(RunOptions * runOptions) {
    this->context.prepare_context(runOptions);
    this->corpus.initializer(&(this->context));

}
//This function encapsulates the main function of iLASH. Right now in public version, iLASH only estimates IBD in bulk files.
//This function gets the input and output addresses. Estimates the IBD tracts in the input file and writes it in the
//output file using output_addr.
void Experiment::read_bulk(const char *input_addr, const char *output_addr) {
    //Maximum number of available threads on the system. 
    unsigned max_threads = std::thread::hardware_concurrency();
    cout<<"The number of cores supported by this machine is "<<max_threads<<endl;
    max_threads *= 2;
//    max_threads = 1;

    queue<string *> *linesQ = new queue<string *>(); //Samples will be loaded in the this queue
    mutex *linesLock = new mutex; //This lock is in charge of synchronizing the linesQ
    bool runFlag = true;

    vector<thread> lsh_thread_list;

    for(unsigned i = 0; i < max_threads; i++){
        lsh_thread_list.push_back(thread(lsh_thread,&(this->corpus),linesLock,linesQ,&runFlag));

    }
    cout<<"Threads started"<<endl;

    ifstream ped_file(input_addr,ifstream::in);
    string* local_str_ptr = new string;
    long counter = 0;

    //Read everything from file and load it in a queue. Worker threads will process them.
    while(getline(ped_file,*local_str_ptr)){
        linesLock->lock();
        linesQ->push(local_str_ptr);
        linesLock->unlock();
        local_str_ptr = new string;
        counter++;
        /*if(counter > 5){
            break;
        }*/
    }
    //wait for threads to finish going through the queue
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
    //start estimating IBD from the LSH hash structure
    cout<<"Writing\n";
    cout<<"---"<<this->corpus.agg_ptr->size()<<"\n";
    this->write_to_file(output_addr);

}


void Experiment::write_to_file(const char *output_addr) {
    unsigned max_threads = std::thread::hardware_concurrency();
    max_threads *= 2;
    //see the writer class for more details
    Writer writer(output_addr,max_threads,&(this->corpus));
    writer.run();
    writer.end_file();
}