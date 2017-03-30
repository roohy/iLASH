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




class Experiment{

public:
    Context context;

    Corpus corpus;

    void set_context(Context );
    void setup_context(const char*,unsigned ,unsigned ,unsigned ,unsigned ,unsigned ,unsigned ,double,double,double);

    void read_bulk(const char*,const char*);
    //void read_bulk_single(const char*,const char*);

    void write_to_file(const char * );




};

#endif //IBD_EXPERIMENT_H
