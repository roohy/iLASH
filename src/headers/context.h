//
// Created by Ruhollah Shemirani on 3/29/17.
//

#ifndef IBD_CONTEXT_H
#define IBD_CONTEXT_H


#include <string>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <exception>
#include <chrono>
#include <thread>


typedef uint8_t dnabit;
static const unsigned VERSION_MAJOR = 1;
static const unsigned VERSION_MINOR = 0;
static const unsigned VERSION_REVISION = 0;

//Run option is used to pass the program arguments from the file to the context class.
class RunOptions{
public:
    std::string map_addr; //map file address
    std::string ped_addr; //genotype file address, must be formatted in plink ped format.
    unsigned slice_size = 800; //In case genetic distance is not used for slicing. Number of SNPs can be used.
    unsigned perm_count = 20;    //Number of permutations for minhash signature
    unsigned shingle_size = 20;  //size of tokens for Jaccard similarity score.
    unsigned bucket_count = 5;  //Number of minhash signatures in each band to create LSH signature.
    unsigned max_thread = 0; //doesn't work
    long long seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::string  out_addr = "match_file.ibd"; //output address.
    double interest_threshold = 0.7; //Threshold to declare an LSH match interesting enough to be analyzed in detail.
    double match_threshold = 0.99; //Threshold to declare an LSH match an actual match without detailed analysis.
    unsigned short max_error = 0; //doesn't work
    unsigned step_size = 800; //The amount of non-overlap between two consecutive slices. In terms of number of SNPs
    unsigned shingle_overlap = 0; //Overlap in terms of number SNPs between two consecutive shingles.
    double minimum_length = 3.0; //Minimum length, in terms of genetic distance, for a segment to written to output as a match. This will be used in auto slice mode to determine the size of
    double slice_length = 3.0;
    bool auto_slice = true; //if this flag is set to true, context will use min-length value to determine the window size.
    double cm_overlap = 1.5; //for auto-slicing based on genetic distance, cm_overlap acts the same as step_size, only in terms of genetic distance.
    uint8_t minhash_threshold = 55; //TODO:write it
    bool haploid_mode; //This option can be used to run iLASH over X chromosome

};


struct FileException: public std::exception
{
    const char* what() const throw()
    {
        return "File does not exsist or iLASH lacks proper permissions";
    }
};
struct FieldException: public std::exception
{
    const char* what() const throw()
    {
        return "One of the required fields is not present in the configuration file";
    }
};
struct DimensionException: public std::exception{
    const char* what() const throw()
    {
        return "The number of SNPs in at least one of the samples does not match the number of SNPs in the map file";
    }
};


//The MapData class encapsulates the lines of a MAP file.
class MapData{
public:
    std::string chrome; //chr number
    std::string RSID; //SNP IS
    double gen_dist;//Distance from the base in cM
    unsigned position;//Position in the sequence
    MapData(std::string,std::string,double,unsigned);




};


//Context class is the configuration and synchronization class behind every experiment.

class Context{
public:
    static const unsigned long word_count = 4294967311; //we use this prime number to approximate the maximum number of words
    //The maximum number of words for 30 SNPs long shingle is way less than this. This will also help us to reduce the number of
    //false positives. Because diving the results of our permutation over a prime number big enough, will generate a near random
    // number.
    bool map_flag; //Don't know what is this. :D
    std::vector<MapData> map_data; //Vector for map data read from MAP file
    std::vector<std::pair<unsigned long , unsigned  long> > shingle_map; //Map that shows where each shingle starts and ends (index on SNPs list)
    std::vector<std::pair<unsigned long,unsigned long> > slice_idx; //Map that shows where each slice starts and ends(index on SNPs list)
    std::vector<std::pair<unsigned ,unsigned > >shingle_idx; // Map that shows on which shingles a slice starts and ends (index on shingles)
//    std::vector<std::pair<bool,bool> >slice_extendable;//show wether we can extend a slice from both sides or not
    std::vector<bool>minhashable;

    //Some of the

    unsigned long slice_count; //number of slices
    unsigned perm_count; //Number of permutations
    std::pair<unsigned long, unsigned long> **perm_matrix; //values for every permutation on every slice
    unsigned shingle_size; 
    unsigned bucket_count;
    unsigned bucket_size;
    double *approx_list; //List of approximated similarity based on LSH settings
    double threshold; //interest threshold
    unsigned minimum_match; //minimum number of LSH signature to be declared a match.
    unsigned minimum_interest; //minimum number of LSH signatures to be interesting
    unsigned shingle_overlap; 
    unsigned slice_size;
    unsigned step_size;
    double match_thresh;
    double minimum_length;
    unsigned short max_error;
    bool auto_slice;
    unsigned thread_count;

    uint8_t minhash_threshold;







    Context();
    void read_map(const char* ); //Read the map file
    void auto_slice_map( double,double); //slice the chromosome in auto-slice (distance-based) mode
    void slice_map(unsigned,unsigned); //slice the choromosome in SNP-based mode
    void prepare_for_minhash(unsigned ,long long); //setup the minhashing variables

    void prepare_context(RunOptions *); // prepare the context of the experiment
    void approximate(); //approximate the minimum number of LSH signatures need to be declared either interesting or match
    bool same_chrom(unsigned long &i1, unsigned long &i2); //check to see if the two are on the same chromosome
    bool is_last_slice(unsigned); 
    bool is_first_slice(unsigned);//look at the implementation for more details
    unsigned shingles_in_slice(unsigned long, unsigned long); //how many shingles in one slice

};

#endif //IBD_CONTEXT_H
