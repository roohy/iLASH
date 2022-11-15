#include <iostream>
#include <fstream>

#include "headers/ped_input_source.h"

Ped_Input_Source::Ped_Input_Source(const char *input_addr) : instream{input_addr, std::ifstream::in} {
}

bool Ped_Input_Source::getNextLine(std::string &line) {
    return static_cast<bool>(std::getline(instream, line));
}
