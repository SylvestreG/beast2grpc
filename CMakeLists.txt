project(beast2grpc CXX)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

if (WIN32)
    macro(get_WIN32_WINNT version)
        if (WIN32 AND CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REPLACE "." "" ver ${ver})
            string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})

            set(${version} "0x${ver}")
        endif()
    endmacro()

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
endif (WIN32)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(gRPC CONFIG REQUIRED)

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)

get_filename_component(hw_proto "${CMAKE_SOURCE_DIR}/src/grpc/reflection.proto" ABSOLUTE)
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

add_executable(beast2grpc   ${CMAKE_SOURCE_DIR}/src/config.cc
                            ${CMAKE_SOURCE_DIR}/src/main.cc
                            ${CMAKE_SOURCE_DIR}/src/http/http_listener.cc
                            ${CMAKE_SOURCE_DIR}/src/http/http_session.cc
                            ${CMAKE_SOURCE_DIR}/src/grpc/call_rpc.cc
                            ${CMAKE_SOURCE_DIR}/src/grpc/grpc_fn.cc
                            ${CMAKE_SOURCE_DIR}/src/grpc/grpc_service.cc
                            ${CMAKE_SOURCE_DIR}/src/grpc/grpc_wrapper.cc
                            ${CMAKE_SOURCE_DIR}/src/grpc/proto_reflection_descriptor_database.cc
                            ${hw_proto_srcs}
                            ${hw_grpc_srcs})
target_link_libraries(beast2grpc CONAN_PKG::spdlog CONAN_PKG::protobuf CONAN_PKG::grpc CONAN_PKG::boost CONAN_PKG::lyra CONAN_PKG::nlohmann_json)

add_subdirectory(tests)
