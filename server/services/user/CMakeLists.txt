cmake_minimum_required(VERSION 3.28)
project(service_user)

include_directories(./
        /usr/local/openssl-arm64/include
        ${PROJECT_SERVER_PATH}/common
        ${PROJECT_SERVER_PATH}/rpc
        ${PROJECT_SERVER_PATH}/storage)
include_directories(
        /usr/local/mysql-connector-c++/include/
        ${THIRD_PARTY_PATH}/boost/include
        ${THIRD_PARTY_PATH}/yaml-cpp/include
        ${THIRD_PARTY_PATH}/grpc/include
        ${THIRD_PARTY_PATH}/jwt-cpp/include)
link_directories(
        /usr/local/openssl-arm64/lib
        /usr/local/mysql-connector-c++/lib64
        ${THIRD_PARTY_PATH}/yaml-cpp/lib)

aux_source_directory(./ SRC_LIST)

add_executable(service_user ${SRC_LIST}
        ../client/ClientManager.cpp
        ../client/ClientManager.h)
set_target_properties(service_user PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug
)

set(absl_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/absl")
set(utf8_range_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/utf8_range")
set(protobuf_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/protobuf")

find_package(Boost REQUIRED COMPONENTS CONFIG)
find_package(yaml-cpp REQUIRED)
find_package(protobuf REQUIRED COMPONENTS CONFIG)
find_package(grpc REQUIRED COMPONENTS CONFIG)
set(OPENSSL_ROOT_DIR "/usr/local/openssl-arm64/")
message(STATUS "OPENSSL_ROOT_DIR: ${OPENSSL_ROOT_DIR}")
find_package(jwt-cpp REQUIRED COMPONENTS CONFIG)

target_link_libraries(service_user PRIVATE ${Boost_LIBRARIES} gRPC::grpc++ common storage rpc yaml-cpp)
