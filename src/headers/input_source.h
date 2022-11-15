#ifndef ILASH_INPUT_SOURCE_H
#define ILASH_INPUT_SOURCE_H

#include <string>
#include <memory>

#include <boost/algorithm/string/predicate.hpp>

struct Input_Source {
    virtual ~Input_Source() = default;
    virtual bool getNextLine(std::string &line) = 0;

    static std::unique_ptr<Input_Source> buildInputSource(const char *input_addr);
};

#endif //ILASH_INPUT_SOURCE_H
