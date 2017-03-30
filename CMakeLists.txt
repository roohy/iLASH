cmake_minimum_required(VERSION 3.5)
project(IBD)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCE_FILES main.cpp headers/context.h context.cpp headers/filereader.h headers/experiment.h headers/corpus.h headers/fnv.h headers/minhasher.h headers/lsh_slave.h filereader.cpp minhasher.cpp experiment.cpp corpus.cpp lsh_slave.cpp headers/writer.h writer.cpp)
add_executable(IBD ${SOURCE_FILES})
target_link_libraries(IBD Threads::Threads)