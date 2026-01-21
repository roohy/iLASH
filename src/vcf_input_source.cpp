#include <fstream>
#include <istream>
#include <vector>
#include <string>
#include <memory>
#include <iostream>

#include <boost/tokenizer.hpp>
#include <boost/format.hpp>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zstd.hpp>

#include "headers/vcf_input_source.h"

namespace bio = boost::iostreams;

Vcf_Input_Source::Vcf_Input_Source(const char *input_addr, const size_t chunk_size) :
        input_file_path{input_addr},
        instream{},
        chunk_size{chunk_size},
        chunk_start_idx{0},
        chunk_end_idx{chunk_size} {
    transposeNextChunk();
    iter = lines.begin();
}


/**
 * Restart stream to beginning of file.
 *
 * Determine if file is uncompressed, gzipped, or zstd compressed.
 * We don't need to check this every time, but we are for now.
 *
 */
void Vcf_Input_Source::resetIStream() {
    std::ifstream istream(input_file_path, std::ios::binary);
    char signature[4];
    if (!istream.read(signature, 4)) {
        // The file could not be read
//        throw FileException(input_file_path.c_str());
        throw std::runtime_error("Could not open file" + input_file_path);
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

    in->push(bio::file_source(input_file_path));

    instream = std::move(in);
}

/**
 * Puts the next line into the passed std::string variable.
 * Moves it out of the vector, meaning that the content of the string is not copied.
 * @param line - The line to put the next individual's data into
 * @return true if the line was updated
 */
bool Vcf_Input_Source::getNextLine(std::string &line) {
    if (iter == lines.end()) {
        // Transpose will update lines
        if (!transposeNextChunk()) {
            return false;
        }

        iter = lines.begin();
    }

    line = std::move(*iter++);
    return true;
}

/**
 * PREVIOUSLY: Reads the entire vcf into memory, and transposes it into 1 line per individual.
 * This uses significant memory, based on the size of the input VCF.
 *
 * A more memory-efficient approach would be chunking it, making several passes over the input file each of which gets
 * a set of columns and building the phased .ped file in chunks.
 *
 * We could also store the genetic info in a more compact from, and build the full line string on demand.
 *
 * Update: This now reads in chunks, and returns an empty vector if we have reached the end of the data.
 *
 * @param infile - An ifstream of the VCF file to read in
 * @return a vector with one line per individual as a string
 */
bool Vcf_Input_Source::transposeNextChunk() {
    resetIStream();
    std::vector<std::string> pedLines;
    std::vector<std::string> sampleNames;

    std::string line;

    std::cout << "Starting next chunk\n";

    while (std::getline(*instream, line)) {

        // Skip info lines
        if (line.empty() || (line[0] == '#' && line[1] == '#')) {
            continue;
        }

        boost::char_separator<char> sep("\t");

        boost::tokenizer<boost::char_separator<char>> tok(line, sep);
        boost::tokenizer<boost::char_separator<char>>::iterator line_iter = tok.begin();
        auto end = tok.end();

        for (int i = 0; i < 9; ++i) {
            ++line_iter; // Skip to the sample data
        }

        for (int i = 0; i < chunk_start_idx; ++i) {
            if (++line_iter == end) { // Skip to start of this chunk
                return false; // Return empty vector if we reached end of input
            }
        }

        // Header with sample names, populate sample names then go to next line
        if (line[0] == '#') {
            for (; line_iter != end; ++line_iter) {
                sampleNames.emplace_back(*line_iter);
            }
            continue;
        }

        size_t i = 0; // Index within current chunk
        for (; line_iter != end; ++line_iter) {
            if (pedLines.size() <= i) {
                auto individualData = boost::format("0 %s 0 0 0 -9") % sampleNames.at(i);
                pedLines.emplace_back(individualData.str());

                // Debugging
                if (i % 10 == 0) {
                    std::cout << "Making record for individual " << i << " in chunk\n";
                }
            }

            auto pair = *line_iter;
            auto first = pair[0];
            auto second = pair[2];

            if (pair[1] != '|') {
                throw std::runtime_error("This VCF is not phased.");
            }

            pedLines[i].push_back(' ');
            pedLines[i].push_back(first);
            pedLines[i].push_back(' ');
            pedLines[i].push_back(second);

            ++i;
            if (i >= chunk_size) {
                break;
            }
        }
    }

    // Export results. Should this be a move?
    // If we're moving to threads and queues it should.
    chunk_start_idx += chunk_size;
    lines = pedLines;
    return true;
}