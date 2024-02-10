cmake_minimum_required(VERSION 3.5)
project(ilash)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS iostreams)
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES src/main.cpp src/headers/context.h src/context.cpp src/headers/filereader.h src/headers/experiment.h src/headers/corpus.h src/headers/fnv.h src/headers/minhasher.h src/headers/lsh_slave.h src/filereader.cpp src/minhasher.cpp src/experiment.cpp src/corpus.cpp src/lsh_slave.cpp src/headers/writer.h src/writer.cpp src/ped_input_source.cpp src/headers/ped_input_source.h src/headers/vcf_input_source.h src/headers/input_source.h src/input_source.cpp src/vcf_input_source.cpp)
add_executable(ilash ${SOURCE_FILES})
target_link_libraries(ilash Threads::Threads)
target_link_libraries(ilash ${Boost_LIBRARIES})

add_executable(vcf2phased-ped src/vcf2ped_main.cpp src/vcf_input_source.cpp)
target_include_directories(vcf2phased-ped PRIVATE src/headers)

target_link_libraries(vcf2phased-ped Threads::Threads)
target_link_libraries(vcf2phased-ped ${Boost_LIBRARIES})
