#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "headers/experiment.h"

using namespace std;

class RunOptions{
public:
    string map_addr;
    string ped_addr;
    unsigned slice_size;
    unsigned perm_count;
    unsigned shingle_size;
    unsigned bucket_count;
    unsigned max_thread;
    string  out_addr;
    double interest_threshold;
    double match_threshold;
    unsigned short max_error;
    unsigned step_size;
    unsigned shingle_overlap;
    double minimum_length;

};

RunOptions extractFromFile(char* file_addr){
    RunOptions runOptions;
    fstream optionFile(file_addr,ifstream::in);
    string option;
    string value;
    while(optionFile>>option){
        if(option == "map"){
            optionFile>>runOptions.map_addr;
        }
        else if(option == "ped"){
            optionFile>>runOptions.ped_addr;
        }
        else if(option == "slice_size"){
            optionFile>>runOptions.slice_size;
        }
        else if(option == "perm_count"){
            optionFile>>runOptions.perm_count;
        }
        else if(option == "shingle_size"){
            optionFile>>runOptions.shingle_size;

        }
        else if(option == "bucket_count"){
            optionFile>>runOptions.bucket_count;
        }
        else if(option == "max_thread"){
            optionFile>>runOptions.max_thread;
        }
        else if(option == "output"){
            optionFile>>runOptions.out_addr;
        }
        else if(option == "interest_threshold"){
            optionFile>>runOptions.interest_threshold;
        }
        else if(option == "match_threshold"){
            optionFile>>runOptions.match_threshold;
        }
        else if(option == "step_size"){
            optionFile>>runOptions.step_size;
        }
        else if(option == "shingle_overlap"){
            optionFile>>runOptions.shingle_overlap;
        }
        else if(option == "max_error"){
            optionFile>>runOptions.max_error;
        }
        else if(option == "min_length"){
            optionFile>>runOptions.minimum_length;
        }

    }
    return runOptions;
}

int main(int argc, char *argv[]) {
    cout<<"Reading from:"<<argv[1]<<endl;
    RunOptions runOptions = extractFromFile(argv[1]);
    cout<<"Context from"<<runOptions.map_addr<<endl;
    Experiment xp;
    xp.setup_context(runOptions.map_addr.c_str(),runOptions.slice_size,runOptions.step_size,runOptions.perm_count,runOptions.shingle_size
            ,runOptions.shingle_overlap,runOptions.bucket_count,runOptions.interest_threshold,runOptions.match_threshold,runOptions.minimum_length,runOptions.max_error);
    xp.read_bulk(runOptions.ped_addr.c_str(),runOptions.out_addr.c_str());
    //xp.read_bulk_single(runOptions.ped_addr.c_str(),runOptions.out_addr.c_str());


}