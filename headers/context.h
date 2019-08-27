//
// Created by Ruhollah Shemirani on 3/29/17.
//

#ifndef IBD_CONTEXT_H
#define IBD_CONTEXT_H


#include <string>
#include <vector>
#include <mutex>
#include <unordered_set>


typedef uint8_t dnabit;

//Run option is used to pass the program arguments from the file to the context class.
class RunOptions{
public:
    std::string map_addr; //map file address
    std::string ped_addr; //genotype file address, must be formatted in plink ped format.
    unsigned slice_size; //In case genetic distance is not used for slicing. Number of SNPs can be used.
    unsigned perm_count;    //Number of permutations for minhash signature
    unsigned shingle_size;  //size of tokens for Jaccard similarity score.
    unsigned bucket_count;  //Number of minhash signatures in each band to create LSH signature.
    unsigned max_thread; //doesn't work
    std::string  out_addr; //output address.
    double interest_threshold; //Threshold to declare an LSH match interesting enough to be analyzed in detail.
    double match_threshold; //Threshold to declare an LSH match an actual match without detailed analysis.
    unsigned short max_error; //doesn't work
    unsigned step_size; //The amount of non-overlap between two consecutive slices. In terms of number of SNPs
    unsigned shingle_overlap; //Overlap in terms of number SNPs between two consecutive shingles.
    double minimum_length; //Minimum length, in terms of genetic distance, for a segment to written to output as a match. This will be used in auto slice mode to determine the size of
    bool auto_slice; //if this flag is set to true, context will use min-length value to determine the window size.
    double cm_overlap; //for auto-slicing based on genetic distance, cm_overlap acts the same as step_size, only in terms of genetic distance.
    uint8_t minhash_threshold; //TODO:write it
    bool haploid_mode; //This option can be used to run iLASH over X chromosome

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

    uint8_t minhash_threshold;







    Context();
    void read_map(const char* ); //Read the map file
    void auto_slice_map( double,double); //slice the chromosome in auto-slice (distance-based) mode
    void slice_map(unsigned,unsigned); //slice the choromosome in SNP-based mode
    void prepare_for_minhash(unsigned ); //setup the minhashing variables
    void prepare_context(const char *,unsigned,unsigned ,unsigned ,unsigned ,unsigned ,unsigned ,double,double,double,
                         unsigned short,bool,double); //will be removed

    void prepare_context(RunOptions *); // prepare the context of the experiment
    void approximate(); //approximate the minimum number of LSH signatures need to be declared either interesting or match
    bool same_chrom(unsigned long &i1, unsigned long &i2); //check to see if the two are on the same chromosome
    bool is_last_slice(unsigned); 
    bool is_first_slice(unsigned);//look at the implementation for more details
    unsigned shingles_in_slice(unsigned long, unsigned long); //how many shingles in one slice

};

#endif //IBD_CONTEXT_H
