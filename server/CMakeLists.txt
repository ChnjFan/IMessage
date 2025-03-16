cmake_minimum_required(VERSION 3.28)
project(IMessage)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/")
set(CMAKE_CXX_STANDARD 17)
set(THIRD_PARTY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/")

add_definitions( -g -W -Wall -lpthread -lprotobuf)

include_directories(./)
link_directories(./)

add_subdirectory(src/service)
add_subdirectory(src/network)
