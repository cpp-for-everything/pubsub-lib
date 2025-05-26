FROM --platform=linux/amd64 ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=C.UTF-8

# Install base tools and languages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    wget \
    python3 \
    python3-pip \
    nodejs \
    npm \
    libtbb-dev \
    qtbase5-dev \
    qt5-qmake \
    libboost-all-dev \
    pkg-config \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Verify GCC version
RUN gcc --version

# Clone your benchmark repo
RUN git clone https://github.com/cpp-for-everything/pubsub-lib.git /opt/pubsub-lib

WORKDIR /opt/pubsub-lib

# Configure and build
RUN cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_BM=ON -DENABLE_QT=ON -DENABLE_BOOST=ON \
 && cmake --build build

CMD ["/bin/bash"]
