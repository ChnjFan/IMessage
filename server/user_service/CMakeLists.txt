cmake_minimum_required(VERSION 3.28)
project(user_service)

include_directories(. ../common ${PROJECT_SERVER_PATH}/rpc)
include_directories(${THIRD_PARTY_PATH}/boost/include
        ${THIRD_PARTY_PATH}/yaml-cpp/include
        ${THIRD_PARTY_PATH}/grpc/include)
link_directories(${THIRD_PARTY_PATH}/yaml-cpp/lib)

aux_source_directory(./ SRC_LIST)

add_executable(user_service ${SRC_LIST})
set_target_properties(user_service PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug
)

set(absl_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/absl")
set(utf8_range_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/utf8_range")
set(protobuf_DIR "${THIRD_PARTY_PATH}/grpc/lib/cmake/protobuf")

find_package(Boost REQUIRED COMPONENTS CONFIG)
find_package(yaml-cpp REQUIRED)
find_package(protobuf REQUIRED COMPONENTS CONFIG)
find_package(grpc REQUIRED COMPONENTS CONFIG)

target_link_libraries(user_service PRIVATE ${Boost_LIBRARIES} gRPC::grpc++ common rpc yaml-cpp)
