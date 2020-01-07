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



//Using a RunOptions instance, this function initializes the Context of the experiment. Please read the context class docs.
void Experiment::setup_context(RunOptions * runOptions) {
    this->context.prepare_context(runOptions);
    this->corpus.initializer(&(this->context));

}
//This function encapsulates the main function of iLASH. Right now in public version, iLASH only estimates iLASH in bulk files.
//This function gets the input and output addresses. Estimates the iLASH tracts in the input file and writes it in the
//output file using output_addr.
void Experiment::read_bulk(const char *input_addr, const char *output_addr) {

    queue<string *> *linesQ = new queue<string *>(); //Samples will be loaded in the this queue
    mutex *linesLock = new mutex; //This lock is in charge of synchronizing the linesQ
    bool runFlag = true;

    vector<thread> lsh_thread_list;

    for(unsigned i = 0; i < this->context.thread_count; i++){
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
    //start estimating iLASH from the LSH hash structure
    cout<<"Writing\n";
    cout<<"---"<<this->corpus.agg_ptr->size()<<"\n";
    this->write_to_file(output_addr);

}


void Experiment::write_to_file(const char *output_addr) {
    //see the writer class for more details
    Writer writer(output_addr,this->context.thread_count,&(this->corpus));
    writer.run();
    writer.end_file();
}