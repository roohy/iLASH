//
// Created by Ruhollah Shemirani on 3/29/17.
//

#include <vector>
#include "headers/filereader.h"
#include "headers/minhasher.h"
#include <queue>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <unordered_map>
#include <thread>
#include "headers/lsh_slave.h"
#include <iostream>

using namespace std;

LSH_Slave::LSH_Slave(Corpus * corpus, std::mutex *linesLock, std::queue<std::string *> *linesQ, bool *runFlag) {
    this->corpus = corpus;
    this->linesLock = linesLock;
    this->linesQ = linesQ;
    this->runFlag = runFlag;
}

void LSH_Slave::run() { 
    string * line = NULL;
    while(*runFlag) {
        //trying to read from input Q
        this->linesLock->lock();
        if (this->linesQ->empty()) {
            //if empty, we release the lock and wait for another round
            this->linesLock->unlock();
            // *run_flag = false;
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        else {
            line = this->linesQ->front();
            this->linesQ->pop();
            this->linesLock->unlock();

            filereader *parser = new filereader(line,this->corpus->context);
            Minhasher *minhash = new Minhasher(this->corpus->context);

            parser->register_to_experiment(this->corpus); //registers the two haplotypes read into the corpus. 

            uint32_t * hash_buffer; //minhash buffer
            uint32_t ** lsh_buffer; //LSH temporary buffer
            std::unordered_map<uint32_t ,unsigned short> relatives[2]; //LSH hits for each haplotype. 


            for(unsigned i = 0; i < this->corpus->context->slice_idx.size(); i++){
                
                //setting the slice number will let the parser know what part of the coordinates to use
                parser->set_slice(i); 
                minhash->set_slice_num(i);
                //int counter = 0;

                //first, we parse
                if( (!this->corpus->context->auto_slice) || this->corpus->context->minhashable[i]){
                    while(parser->hasNext()){
                        //counter++;
                        //gets the hash buffer from the parser and feeds it into the minhash analyzer.
                        hash_buffer = parser->getNextHashed(); 
                        minhash->insert(hash_buffer);
                        //once all the hash values are inserted in the minhash analyzer, minhash analyzer can calculate the LSH values. 
                    }

                    //preparing for LSH
                    //based on the hashes fed into the minhash analyzer, it generates the lsh signature and returns in in a buffer. 
                    lsh_buffer = minhash->calculate_lsh();

                    relatives[0].clear();
                    relatives[1].clear();

                    this->corpus_generator(lsh_buffer,relatives,i,parser->ids);
                    this->aggregator(relatives,i,parser->ids);


                }else{
                    while(parser->hasNext()){
                        //counter++;
                        parser->getNextHashed();

                    }
                }

            }
            delete parser;
            delete minhash;
        }
    }

}

inline void LSH_Slave::corpus_generator(uint32_t **lsh_matrix, std::unordered_map<uint32_t, unsigned short> * relatives, unsigned slice_num, uint32_t *ids) {

    for(int i = 0 ; i < 2 ; i++){
        this->corpus->add_to_corpus(lsh_matrix[i],ids[i],slice_num,relatives+i);
    }

}

inline void LSH_Slave::aggregator(std::unordered_map<uint32_t, unsigned short> * relatives, unsigned slice_num, uint32_t * ids) {
    for(int i = 0 ; i < 2; i++){
        this->corpus->integrate(relatives+i,ids[i],slice_num);
    }
}