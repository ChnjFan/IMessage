cmake_minimum_required(VERSION 3.28)
project(user_lib)

include_directories(. ${THIRD_PARTY_PATH}/grpc/include
                    ${PROJECT_SERVER_PATH}/rpc)
link_directories(${THIRD_PARTY_PATH}/grpc/lib)

aux_source_directory(./ SRC_LIST)

add_library(user_lib SHARED ${SRC_LIST}
        UserClient.cpp
        UserClient.h
        UserConfig.h)
target_compile_definitions(user_lib PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}/../../")
set(absl_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/absl")
set(utf8_range_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/utf8_range")
set(protobuf_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/protobuf")

find_package(protobuf REQUIRED COMPONENTS CONFIG)
find_package(grpc REQUIRED COMPONENTS CONFIG)

target_link_libraries(user_lib PRIVATE gRPC::grpc++ rpc)
