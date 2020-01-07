cmake_minimum_required(VERSION 3.5)
project(ilash)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCE_FILES src/main.cpp src/headers/context.h src/context.cpp src/headers/filereader.h src/headers/experiment.h src/headers/corpus.h src/headers/fnv.h src/headers/minhasher.h src/headers/lsh_slave.h src/filereader.cpp src/minhasher.cpp src/experiment.cpp src/corpus.cpp src/lsh_slave.cpp src/headers/writer.h src/writer.cpp)
add_executable(ilash ${SOURCE_FILES})
target_link_libraries(ilash Threads::Threads)