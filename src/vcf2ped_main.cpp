#include <iostream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zstd.hpp>
#include <boost/algorithm/string.hpp>

#include "headers/vcf_input_source.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: vcf2ped my_vcf.vcf.gz\n";
        return 0;
    }

    auto filename = argv[1];
    if (!boost::algorithm::ends_with(filename, ".vcf.gz")) {
        std::cout << "Only supports .vcf.gz inputs\n";
        return 1;
    }

    auto in = std::make_unique<boost::iostreams::filtering_stream<boost::iostreams::input>>();
    in->push(boost::iostreams::gzip_decompressor());
    in->push(boost::iostreams::file_source(filename));

    Vcf_Input_Source transposer(std::move(in));

    std::string infileName = std::string(filename);
    std::string outfileName = infileName.substr(0, infileName.size() - 7) + ".ped.zst";

    std::cout << "Outfile name is " << outfileName << "\n";

    boost::iostreams::filtering_stream<boost::iostreams::output> out;
    out.push(boost::iostreams::zstd_compressor());
    out.push(boost::iostreams::file_sink(outfileName));

    std::string line;
    while (transposer.getNextLine(line)) {
        out << line << "\n";
    }
}
