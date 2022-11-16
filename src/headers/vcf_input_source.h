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

class Vcf_Input_Source : public Input_Source {
public:
    Vcf_Input_Source(const char *input_addr);
    Vcf_Input_Source(std::unique_ptr<std::istream> &&input_stream);
    ~Vcf_Input_Source() override = default;

    bool getNextLine(std::string &line) override;
private:
    std::unique_ptr<std::istream> instream;
    std::vector<std::string> lines;
    std::vector<std::string>::iterator iter;

    static std::vector<std::string> transposeVcfToPed(std::istream &infile);
};

#endif //ILASH_VCF_INPUT_SOURCE_H
