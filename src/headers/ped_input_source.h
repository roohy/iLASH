#ifndef ILASH_PED_INPUT_SOURCE_H
#define ILASH_PED_INPUT_SOURCE_H

#include <iterator>
#include <istream>
#include <fstream>
#include <memory>

#include "input_source.h"

class Ped_Input_Source : public Input_Source {
public:
    Ped_Input_Source(const char *input_addr);
    ~Ped_Input_Source() = default;

    bool getNextLine(std::string &line) override;
private:
    std::unique_ptr<std::istream> instream;
};

#endif //ILASH_PED_INPUT_SOURCE_H
