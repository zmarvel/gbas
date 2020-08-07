FROM debian:buster

RUN apt update && \
    apt install -y \
        g++ \
        cmake \
        clang \
        make \
        libboost-test-dev \
        curl

RUN curl -L -O https://github.com/Kitware/CMake/releases/download/v3.18.1/cmake-3.18.1-Linux-x86_64.tar.gz && \
    tar xvf cmake-3.18.1-Linux-x86_64.tar.gz && \
    cp -a cmake-3.18.1-Linux-x86_64/bin/* /usr/local/bin
