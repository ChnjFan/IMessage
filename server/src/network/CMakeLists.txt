cmake_minimum_required(VERSION 3.28)
project(network)

include_directories(.)
aux_source_directory(./ SRC_LIST)

add_library(network SHARED ${SRC_LIST})

#find_package(Poco REQUIRED COMPONENTS Net Util XML JSON)
#find_package(protobuf REQUIRED PATHS ${THIRD_PARTY_PATH}/protobuf/lib)
#target_link_libraries(net PRIVATE Poco::Net Poco::Util Poco::XML Poco::JSON protocol base zookeeper_mt)
