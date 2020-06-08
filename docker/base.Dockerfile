### Base environment container ###
# Get the base Ubuntu image from Docker Hub
FROM ubuntu:bionic as base

# Update the base image and install build environment
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    curl \
    gcovr \
    httpie \
    libboost-log-dev \
    libboost-signals-dev \
    libboost-system-dev \
    libboost-regex-dev \
    libgtest-dev \
    netcat \
    wget 

# Install dependices for MONGOC (which is a dependency for mongocxx)
RUN apt-get install -y \
    pkg-config \
    libssl-dev \
    libsasl2-dev \
    libmongoc-1.0-0 \
    libbson-1.0 \
    libmongoc-dev \
    libbson-dev \
    git

# Install MONGOC
RUN cd ~ \
    && wget https://github.com/mongodb/mongo-c-driver/releases/download/1.16.2/mongo-c-driver-1.16.2.tar.gz \
    && tar xzf mongo-c-driver-1.16.2.tar.gz \
    && cd mongo-c-driver-1.16.2 \
    && mkdir cmake-build \
    && cd cmake-build \
    && cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. \
    && make \
    && make install \
    && cd ~ \
    && rm mongo-c-driver-1.16.2.tar.gz \
    && rm -rf mongo-c-driver-1.16.2

# Install MongoCXX
RUN cd ~ \
    && wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.5.1/mongo-cxx-driver-r3.5.1.tar.gz \
    && tar -xzf mongo-cxx-driver-r3.5.1.tar.gz \
    && cd mongo-cxx-driver-r3.5.1/build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && make EP_mnmlstc_core \
    && make \
    && make install \
    && cd ~ \
    && rm mongo-cxx-driver-r3.5.1.tar.gz \
    && rm -rf mongo-cxx-driver-r3.5.1

