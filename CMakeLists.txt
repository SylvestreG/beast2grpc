project(beast2grpc CXX)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

find_package(protoc CONFIG)
find_package(protobuf CONFIG)
find_package(gRPC CONFIG REQUIRED)

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/grpc)

get_filename_component(hw_proto "reflection.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

set(hw_proto_srcs "${CMAKE_BINARY_DIR}/reflection.pb.cc")
set(hw_proto_hdrs "${CMAKE_BINARY_DIR}/reflection.pb.h")
set(hw_grpc_srcs "${CMAKE_BINARY_DIR}/reflection.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_BINARY_DIR}/reflection.grpc.pb.h")

add_custom_command(
  OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
  COMMAND protobuf::protoc
  ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
  --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
  -I "${hw_proto_path}"
  --plugin=protoc-gen-grpc="$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
  "${hw_proto}"
  DEPENDS "${hw_proto}")

add_executable(beast2grpc   ${CMAKE_SOURCE_DIR}/main.cc
                            ${CMAKE_SOURCE_DIR}/grpc/call_rpc.cc
                            ${hw_proto_srcs}
                            ${hw_grpc_srcs}
                            ${CMAKE_SOURCE_DIR}/proto_reflection_descriptor_database.cc
                            ${CMAKE_SOURCE_DIR}/reflection.proto)
target_link_libraries(beast2grpc "CONAN_PKG::spdlog" "CONAN_PKG::protobuf" "CONAN_PKG::grpc")