FROM mcr.microsoft.com/vscode/devcontainers/base:jammy

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive && apt-get -y install --no-install-recommends \
    nano \
    gnupg2 \
    git \
    protobuf-compiler \
    build-essential \
    cmake \
    clang-format \
    cppcheck \
    iputils-ping \
    libgeographic-dev \
    libgps-dev \
    libcrypto++-dev \
    libprotobuf-dev \
    libboost-all-dev \
    libasio-dev \
    libcurl4-openssl-dev \
    libpoco-dev \
    libssl-dev \
    openssl \
    uncrustify
