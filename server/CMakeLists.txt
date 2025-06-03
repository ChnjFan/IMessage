cmake_minimum_required(VERSION 3.28)
project(IMServer)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/")
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_SERVER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/")
set(THIRD_PARTY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/")

add_definitions( -g -W -Wall -lpthread -lprotobuf)

# 公共目录
add_subdirectory(common)
add_subdirectory(rpc)

# 服务器目录
add_subdirectory(msggateway)

# 数据库模块
add_subdirectory(storage)

# 服务模块
add_subdirectory(services/user)
# 服务客户端
add_subdirectory(services/client)
