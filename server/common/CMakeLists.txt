cmake_minimum_required(VERSION 3.28)
project(common)

include_directories(. ${THIRD_PARTY_PATH}/boost/include
        ${THIRD_PARTY_PATH}/log4cxx/include
        ${THIRD_PARTY_PATH}/yaml-cpp/include)
link_directories(${THIRD_PARTY_PATH}/boost/lib
        ${THIRD_PARTY_PATH}/yaml-cpp/lib)
aux_source_directory(./ SRC_LIST)

add_library(common SHARED ${SRC_LIST}
        ConfigManager.cpp
        ConfigManager.h
        Message.cpp)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(common PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}/../")

find_package(Boost REQUIRED COMPONENTS CONFIG)
find_package(log4cxx REQUIRED COMPONENTS log4cxx)
find_package(yaml-cpp REQUIRED)

target_link_libraries(common PRIVATE ${Boost_LIBRARIES} log4cxx yaml-cpp)
