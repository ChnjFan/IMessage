#!/bin/bash

source "./utils.sh"

BASE_DIR=$(cd $(dirname $0); pwd -P)
echo $BASE_DIR

install_boost() {
    check_user
    mkdir boost
    cd boost || exit
    download boost_1_87_0.tar.gz https://archives.boost.io/release/1.87.0/source/${INSTALL_PKG}.tar.gz
    tar -zxvf boost_1_87_0.tar.gz
    cd boost_1_87_0 || exit
    ./bootstrap.sh --prefix=/home/fan/IMessage/third_party/boost/
    ./b2 install --prefix=/home/fan/IMessage/third_party/boost/

    return 0
}

install_log4cpp() {
    check_user
    mkdir log4cxx
    cd log4cx || exit
    apt-get install libapr1-dev libaprutil1-dev
    wget https://downloads.apache.org/logging//log4cxx/1.4.0/apache-log4cxx-1.4.0.tar.gz
    tar xf apache-log4cxx-1.4.0.tar.gz
    cd apache-log4cxx-1.4.0
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/home/fan/IMessage/third_party/log4cxx/
    make
    sudo make install
}

#install_boost
install_log4cpp