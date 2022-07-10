FROM debian:bullseye

RUN apt update && \
    apt install -y --no-install-recommends g++ cmake clang ninja-build libboost-test-dev && \
    rm -rf /var/lib/apt/lists/*
