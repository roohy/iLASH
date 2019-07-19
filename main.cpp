#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "headers/experiment.h"
#include "headers/context.h"

using namespace std;

//This class stores encapsulates the run configurations for iLASH experiment to start,given by the user.

RunOptions extractFromFile(char* file_addr){ //populates a runOptions instance from a file; given the address.
    RunOptions runOptions;
    fstream optionFile(file_addr,ifstream::in);
    string option;
    string value;
    runOptions.auto_slice = false;
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
        else if(option == "auto_slice"){
            optionFile>>runOptions.auto_slice;
            cout<<"auto_slice"<<runOptions.auto_slice<<endl;
        }
        else if(option == "cm_overlap"){
            optionFile>>runOptions.cm_overlap;
        }
        else if(option == "minhash_threshold"){
            optionFile>>runOptions.minhash_threshold;
        }
    }
    return runOptions;
}

int main(int argc, char *argv[]) {

    cout<<"Reading configurations from: "<<argv[1]<<endl;
    RunOptions runOptions = extractFromFile(argv[1]);
    cout<<"Map loading from address:"<<runOptions.map_addr<<endl;
    //Initializing the experiment
    Experiment xp;
    //setting up the context of the experiment
    xp.setup_context(&runOptions);
    //reading a bulk IBD file
    xp.read_bulk(runOptions.ped_addr.c_str(),runOptions.out_addr.c_str());



}