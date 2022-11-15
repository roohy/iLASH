//
// Created by Alex Petty on 11/15/22.
//

#ifndef ILASH_VCF_INPUT_SOURCE_H
#define ILASH_VCF_INPUT_SOURCE_H

#include <iterator>
#include <fstream>
#include <string>
#include <vector>

#include "input_source.h"

class Vcf_Input_Source : public Input_Source {
public:
    Vcf_Input_Source(const char *input_addr);
    ~Vcf_Input_Source() = default;

    bool getNextLine(std::string &line) override;
private:
    std::ifstream instream;
    std::vector<std::string> lines;
    std::vector<std::string>::iterator iter;

    static std::vector<std::string> transposeVcfToPed(std::ifstream &infile);
};

    /*
    // Actually this iterator looks like it's for Vcf_Input_Source
    struct Iterator {
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::string;
        using pointer = value_type *;
        using reference = value_type &;

        Iterator(pointer ptr) : m_ptr(ptr) {}

        reference operator*() const { return *m_ptr; }

        pointer operator->() { return m_ptr; }

        Iterator &operator++() {
            m_ptr++;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator &a, const Iterator &b) { return a.m_ptr == b.m_ptr; };

        friend bool operator!=(const Iterator &a, const Iterator &b) { return a.m_ptr != b.m_ptr; };
    private:
        std::string *m_ptr;
        std::istream instream;
    };
};
*/
#endif //ILASH_VCF_INPUT_SOURCE_H
