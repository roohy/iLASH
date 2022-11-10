FROM ubuntu:22.04 as build-container

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake
    
WORKDIR /app
COPY . .
RUN rm -rf build
RUN mkdir build
WORKDIR /app/build
RUN cmake -DCMAKE_BUILD_TYPE=Release ..
RUN make -j2

# Run container without build dependencies
FROM ubuntu:22.04 as run-container

WORKDIR /app
COPY --from=build-container /app/build/ilash /app/ilash
CMD "/app/ilash"
