cmake_minimum_required(VERSION 3.28)
project(msg_gateway)

include_directories(. ../common ../rpc ../service_lib/user)
include_directories(${THIRD_PARTY_PATH}/boost/include
        ${THIRD_PARTY_PATH}/yaml-cpp/include
        ${THIRD_PARTY_PATH}/grpc/include)
link_directories(${THIRD_PARTY_PATH}/yaml-cpp/lib)

aux_source_directory(./ SRC_LIST)

add_executable(msg_gateway ${SRC_LIST})
set_target_properties(msg_gateway PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug
)

find_package(Boost REQUIRED COMPONENTS CONFIG)
find_package(yaml-cpp REQUIRED)

target_link_libraries(msg_gateway PRIVATE ${Boost_LIBRARIES} common rpc user_lib yaml-cpp)
