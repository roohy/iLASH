//
// Created by Ruhollah Shemirani on 3/29/17.
//



#ifndef IBD_EXPERIMENT_H
#define IBD_EXPERIMENT_H


#include "context.h"
#include <unordered_map>
#include <vector>
#include <string>
#include "corpus.h"
#include "filereader.h"




class Experiment{ //A run of iLASH is an experiment

public:
    Context context;

    Corpus corpus;

    void set_context(Context ); //sets up context
    void setup_context(RunOptions * );
    void read_bulk(const char*,const char*); //for now will only read in bulk
    //void read_bulk_single(const char*,const char*);

    void write_to_file(const char * );




};

#endif //IBD_EXPERIMENT_H
