#include <memory>

#include "headers/input_source.h"
#include "headers/vcf_input_source.h"
#include "headers/ped_input_source.h"

std::unique_ptr<Input_Source> Input_Source::buildInputSource(const char *input_addr) {
    if (boost::algorithm::ends_with(input_addr, ".vcf")) {
        return std::make_unique<Vcf_Input_Source>(input_addr);
    } else if (boost::algorithm::ends_with(input_addr, ".ped")) {
        return std::make_unique<Ped_Input_Source>(input_addr);
    }

    throw std::runtime_error{"Input file not detected as .ped or .vcf. Cannot determine input type."};
}