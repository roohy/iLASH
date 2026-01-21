//
// Created by Alex Petty on 11/15/22.
//

#ifndef ILASH_VCF_INPUT_SOURCE_H
#define ILASH_VCF_INPUT_SOURCE_H

#include <iterator>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "input_source.h"

/**
 * Decompresses and transposes a VCF such that samples are returned as a complete record.
 * Memory usage is limited by chunk_size, which will control the maximum number of sample
 * records that are held in memory at once.
 *
 * Maybe instead of chunk size it could take "number of chunks" and figure out
 * how to divide the input?
 *
 * We also could potentially send another format to the threads, as they are duplicating the splitting
 * that we already did here.
 */
class Vcf_Input_Source : public Input_Source {
public:
    Vcf_Input_Source(const char *input_addr, size_t = 3000);
    ~Vcf_Input_Source() override = default;

    bool getNextLine(std::string &line) override;
private:
    std::unique_ptr<std::istream> instream;
    std::vector<std::string> lines;
    std::vector<std::string>::iterator iter;

    const size_t chunk_size;
    size_t chunk_start_idx;
    size_t chunk_end_idx; // Ended up only using a start index
    const std::string input_file_path;

    bool transposeNextChunk();
    void resetIStream();
};

#endif //ILASH_VCF_INPUT_SOURCE_H
