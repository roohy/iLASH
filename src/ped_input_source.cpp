#include <iostream>
#include <fstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zstd.hpp>

#include "headers/ped_input_source.h"

namespace bio = boost::iostreams;

Ped_Input_Source::Ped_Input_Source(const char *input_addr) {
    std::ifstream istream(input_addr, std::ios::binary);
    char signature[4];
    if (!istream.read(signature, 4)) {
        // The file could not be read
//        throw FileException(input_file_path.c_str());
        throw std::runtime_error(std::string("Could not open file") + input_addr);
    }

    auto in = std::make_unique<bio::filtering_stream<bio::input>>();
    if (signature[0] == '\x1f' && signature[1] == '\x8b') {
        // This is a gzip file
        in->push(bio::gzip_decompressor());
    } else if (signature[0] == '\x28' && signature[1] == '\xb5' && signature[2] == '\x2f' && signature[3] == '\xfd') {
        // This is a zstd file
        in->push(bio::zstd_decompressor());
    }
    // We assume uncompressed if not zstd or gzip

    in->push(bio::file_source(input_addr));

    instream = std::move(in);
}

bool Ped_Input_Source::getNextLine(std::string &line) {
    return static_cast<bool>(std::getline(*instream, line));
}
