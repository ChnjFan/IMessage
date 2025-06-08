cmake_minimum_required(VERSION 3.28)
project(storage)

include_directories(. /usr/local/mysql-connector-c++/include/
        ${THIRD_PARTY_PATH}/boost/include
        ${THIRD_PARTY_PATH}/log4cxx/include
        ${THIRD_PARTY_PATH}/yaml-cpp/include
        ${PROJECT_SERVER_PATH}/common)
link_directories(/usr/local/mysql-connector-c++/lib64
        ${THIRD_PARTY_PATH}/boost/lib
        ${THIRD_PARTY_PATH}/yaml-cpp/lib)

file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(child ${children})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
        aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${child} SRC_LIST)
    endif()
endforeach()

add_library(storage SHARED ${SRC_LIST}
        cache/UserCache.cpp
        cache/UserCache.h
        controller/DBConnPool.h
        controller/DBConnPool.cpp
        controller/DBConnPool.h)
target_compile_definitions(storage PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}/../")

find_package(Boost REQUIRED COMPONENTS CONFIG)
find_package(log4cxx REQUIRED COMPONENTS log4cxx)
find_package(yaml-cpp REQUIRED)

target_link_libraries(storage PRIVATE ${Boost_LIBRARIES} log4cxx yaml-cpp common)