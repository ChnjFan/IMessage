cmake_minimum_required(VERSION 3.28)
project(IMService)

include_directories(.)

aux_source_directory(./ SRC_LIST)

add_executable(IMService ${SRC_LIST} )
set_target_properties(IMService PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug
)

#target_link_libraries(im_server PRIVATE Poco::Net Poco::Util Poco::XML Poco::JSON proto gRPC::grpc++ protobuf)