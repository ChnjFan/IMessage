cmake_minimum_required(VERSION 3.28)
project(IMExample)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/")
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_SERVER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/")
set(THIRD_PARTY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/")

add_definitions( -g -W -Wall -lpthread -lprotobuf)

include_directories(.)
include_directories(${THIRD_PARTY_PATH}/boost/include
        ${THIRD_PARTY_PATH}/yaml-cpp/include
        ${THIRD_PARTY_PATH}/grpc/include)
link_directories(${THIRD_PARTY_PATH}/yaml-cpp/lib)

aux_source_directory(./ SRC_LIST)

add_executable(login ${SRC_LIST})
set_target_properties(login PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug
)

find_package(Boost REQUIRED COMPONENTS CONFIG)
find_package(yaml-cpp REQUIRED)

target_link_libraries(login PRIVATE ${Boost_LIBRARIES} yaml-cpp)
