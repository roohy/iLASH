//
// Created by Ruhollah Shemirani on 3/29/17.
//

#ifndef IBD_CONTEXT_H
#define IBD_CONTEXT_H


#include <string>
//#include <map>
#include <vector>
#include <mutex>
#include <unordered_set>


typedef uint8_t dnabit;

class MapData{
public:
    std::string chrome; //chr number
    std::string RSID; //SNP IS
    double gen_dist;//Distance from the base in cM
    unsigned position;//Position in the sequence
    MapData(std::string,std::string,double,unsigned);




};

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



    unsigned long slice_count; //number of slices
    unsigned perm_count; //Number of permutations
    std::pair<unsigned long, unsigned long> **perm_matrix;
    unsigned shingle_size;
    unsigned bucket_count;
    unsigned bucket_size;
    double *approx_list;
    double threshold;
    unsigned minimum_match;
    unsigned minimum_interest;
    unsigned shingle_overlap;
    unsigned slice_size;
    unsigned step_size;
    double match_thresh;
    double minimum_length;
    unsigned short max_error;
    bool auto_slice;









    Context();
    void read_map(const char* );
    void auto_slice_map( double,double);
    void slice_map(unsigned,unsigned);
    void prepare_for_minhash(unsigned );
    void prepare_context(const char *,unsigned,unsigned ,unsigned ,unsigned ,unsigned ,unsigned ,double,double,double,
                         unsigned short,bool,double);
    void approximate();
    bool same_chrom(unsigned long &i1, unsigned long &i2);
    bool is_last_slice(unsigned);
    bool is_first_slice(unsigned);
    unsigned shingles_in_slice(unsigned long, unsigned long);

};

#endif //IBD_CONTEXT_H
