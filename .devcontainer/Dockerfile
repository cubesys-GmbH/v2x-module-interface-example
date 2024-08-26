FROM mcr.microsoft.com/vscode/devcontainers/base:noble
RUN userdel -r ubuntu

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
    libgps-dev \
    libcrypto++-dev \
    libprotobuf-dev \
    libboost-all-dev \
    libasio-dev \
    libcurl4-openssl-dev \
    libpoco-dev \
    libssl-dev \
    openssl \
    uncrustify \
    python3-protobuf
