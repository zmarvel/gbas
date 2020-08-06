FROM debian:buster

RUN apt update && \
    apt install -y \
        g++ \
        make \
        libboost-test-dev
