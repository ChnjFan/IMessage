cmake_minimum_required(VERSION 3.28)
project(service_client)

include_directories(. ${THIRD_PARTY_PATH}/grpc/include
                    ${PROJECT_SERVER_PATH}/rpc
                    ${PROJECT_SERVER_PATH}/common)
link_directories(${THIRD_PARTY_PATH}/grpc/lib)

aux_source_directory(./ SRC_LIST)

add_library(service_client SHARED ${SRC_LIST})
target_compile_definitions(service_client PRIVATE PROJECT_ROOT="${PROJECT_SERVER_PATH}/")
set(absl_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/absl")
set(utf8_range_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/utf8_range")
set(protobuf_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/protobuf")

find_package(protobuf REQUIRED COMPONENTS CONFIG)
find_package(grpc REQUIRED COMPONENTS CONFIG)

target_link_libraries(service_client PRIVATE gRPC::grpc++ rpc common)
