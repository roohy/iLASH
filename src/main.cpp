#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <exception>
#include "headers/experiment.h"
#include "headers/context.h"

using namespace std;

//This class stores encapsulates the run configurations for iLASH experiment to start,given by the user.

RunOptions extractFromFile(char* file_addr){ //populates a runOptions instance from a file; given the address.
    RunOptions runOptions;
    ifstream optionFile(file_addr,ifstream::in);
    if(optionFile.fail()){
        throw FileException();
    }
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
        else if(option == "auto_slice"){
            optionFile>>runOptions.auto_slice;
            if(runOptions.auto_slice) {
                cout << "Automatic slicing mode is activated."<< endl;
            }
        }
        else if(option == "slice_length"){
            optionFile>>runOptions.slice_length;
        }
        else if(option == "cm_overlap"){
            optionFile>>runOptions.cm_overlap;
        }
        else if(option == "minhash_threshold"){
            optionFile>>runOptions.minhash_threshold;
        }
        else if(option == "haploid_mode"){
            optionFile>>runOptions.haploid_mode;
        }
        else if(option == "seed"){
            optionFile>>runOptions.seed;
        }
    }
    if (runOptions.map_addr.empty() || runOptions.ped_addr.empty()){
        throw FieldException();
    }
    return runOptions;
}

void print_help(){
    cout<<"*** iLASH (Identity by descent using LocAlity Sensitive Hashing) ***"<<endl;
    cout<<"Usage: ./ilash [configuration file address]"<<endl;
    cout<<"Configuration file contains the arguments to run iLASH. More details at: https://github.com/roohy/iLASH ."<<endl;
    cout<<"map and ped: Input file addresses. The input file should be phased and follow PLINK ped/map formatting."<<endl;
    cout<<"out : The address of the output IBD file."<<endl;
    cout<<"slice_length: The length of each slice in cM distance."<<endl;
    cout<<"min_length: The minimum length of an IBD segment in order to be written to the output."<<endl;
    cout<<"cm_overlap: The lenght of genotype overlap between two consecutive slices in cM."<<endl;
    cout<<"max_thread: Maximum number of threads allocated for the task."<<endl;
    cout<<"match_threshold/interest_threshold: Minimum estimated similarity for a pair to declared a match/further investigated for possible matches in the area."<<endl;

}

int main(int argc, char *argv[]) {
    if(argc<2){
        cout<<"Configuration file address is not passed to the program."<<endl;
        cout<<"You can refer to iLASH --help for more info."<<endl;
        return 0;
    }
    else if(string(argv[1]) == "--help"){
            print_help();
            return 0;
    }
    else if(string(argv[1]) == "--version"){
            cout<<"iLASH Version: "<<VERSION_MAJOR<<'.'<<VERSION_MINOR<<'.'<<VERSION_REVISION<<endl;

            return 0;
    }
    else{
        RunOptions runOptions;
        try
        {
            cout<<"*** iLASH (Identity by descent using LocAlity Sensitive Hashing) ***"<<endl;
            cout<<"Configuration file address: "<<argv[1]<<endl;
            runOptions = extractFromFile(argv[1]);
            cout<<"Map file address: "<<runOptions.map_addr<<endl;
            //Initializing the experiment
            Experiment xp;
            //setting up the context of the experiment
            xp.setup_context(&runOptions);
            //reading a bulk iLASH file
            //cout<<"The seed is "<<runOptions.seed<<'.'<<endl;
            cout<<"Genotype file:"<<runOptions.ped_addr<<'.'<<endl;
            xp.read_bulk(runOptions.ped_addr.c_str(),runOptions.out_addr.c_str());
        }
        catch (FileException e){
            cout<<e.what()<<endl;
            cout<<"Make sure iLASH has permission to access both the input and the output files."<<endl;
            return 0;
        }
        catch (FieldException e){
            cout<<e.what()<<endl;
            cout<<"Required fields for configuration file are: MAP and PED file address"<<endl;
            return 0;
        }

    }

}