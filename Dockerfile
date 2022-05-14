FROM debian:bullseye

RUN apt update && \
    apt install -y \
        g++ \
        cmake \
        clang \
        make \
        libboost-test-dev \
        curl
