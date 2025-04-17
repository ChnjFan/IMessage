cmake_minimum_required(VERSION 3.28)
project(msg_gateway)

include_directories(. ../common)
include_directories(${Boost_INCLUDE_DIRS}
        ${THIRD_PARTY_PATH}/yaml-cpp/include)
link_directories(${THIRD_PARTY_PATH}/yaml-cpp/lib)

aux_source_directory(./ SRC_LIST)

add_executable(msg_gateway ${SRC_LIST}
        ConnServer.cpp
        Client.h
        Session.h
        MsgGatewayServer.h)
set_target_properties(msg_gateway PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug
)

find_package(Boost REQUIRED COMPONENTS system)
find_package(yaml-cpp REQUIRED)
target_link_libraries(msg_gateway PRIVATE ${Boost_LIBRARIES} common yaml-cpp)
