#!/bin/bash

source "./utils.sh"
source "./boost_install.sh"
source "./grpc_install.sh"

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

main() {
  check_user
  check_dependencies
  install_boost
  install_grpc
}

main