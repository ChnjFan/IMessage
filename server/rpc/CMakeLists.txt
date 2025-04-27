cmake_minimum_required(VERSION 3.28)
project(rpc)

include_directories(. ${THIRD_PARTY_PATH}/grpc/include)
link_directories(${THIRD_PARTY_PATH}/grpc/lib)

file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(child ${children})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
        aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${child} SRC_LIST)
    endif()
endforeach()

add_library(rpc SHARED ${SRC_LIST})
target_compile_definitions(rpc PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}/../")
set(absl_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/absl")
set(utf8_range_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/utf8_range")
set(protobuf_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/protobuf")

find_package(protobuf REQUIRED COMPONENTS CONFIG)
find_package(grpc REQUIRED COMPONENTS CONFIG)

target_link_libraries(rpc PRIVATE gRPC::grpc++)
