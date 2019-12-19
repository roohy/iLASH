//
// Created by Ruhollah Shemirani on 3/30/17.
//

#ifndef IBD_WRITER_H
#define IBD_WRITER_H

#include <vector>
#include "context.h"
#include "corpus.h"
#include <string>
#include <queue>
#include <fstream>
#include <iterator>

/*class Extender{
    Corpus * corpus;
    uint32_t p1;
    uint32_t p2;
    std::vector<std::pair<unsigned ,bool> > * matches;
public:
    Extender(uint32_t ,uint32_t ,std::vector<std::pair<unsigned ,bool> > *,Corpus *);
    std::vector<std::pair<unsigned long, unsigned long> > * extend();
};

*/
class Writer{
public:
    Writer(const char * , unsigned, Corpus *);

    void end_file();

    void run();

    void threaded();
    //inline void safeAdd(unsigned long, unsigned long,std::vector<std::pair<unsigned long, unsigned long> > *);
    inline void shingleSafeAdd(unsigned, unsigned ,std::vector<std::pair<unsigned , unsigned > > *);

    std::vector<std::pair<unsigned , bool> > * getlist(uint32_t &,uint32_t &);

    void output(std::vector<std::pair<unsigned , unsigned > > *,uint32_t , uint32_t);

    inline unsigned long extend(unsigned long tail, unsigned long bound, unsigned short error,uint32_t p1,uint32_t p2);
    inline unsigned long back_extend(unsigned long head, unsigned long bound, unsigned short error,uint32_t p1, uint32_t p2);


    inline unsigned  shingle_extend(unsigned  head, unsigned  bound, unsigned short error,uint32_t p1,uint32_t p2);
    inline unsigned  shingle_back_extend(unsigned  head, unsigned  bound, unsigned short error,uint32_t p1, uint32_t p2);

    inline double similarity(uint32_t p1, uint32_t p2, unsigned  st, unsigned  end);

    const char * output_addr;
    unsigned max_thread;
    Corpus * corpus;
    std::ofstream output_file;
    std::mutex printLock;
    std::mutex jobLock;
//    typename std::unordered_map<uint32_t, std::unordered_map<uint32_t ,std::vector<std::pair<unsigned ,bool> > > >::iterator bit;
//    typename std::unordered_map<uint32_t, std::vector<std::pair<unsigned ,bool> > >::iterator lit;
    typename std::unordered_map<uint64_t, std::vector<std::pair<unsigned ,bool> > >::iterator iter;

};
#endif //IBD_WRITER_H
