include_directories(${CMAKE_SOURCE_DIR})

add_executable(ut ${CMAKE_SOURCE_DIR}/src/config.cc
                  ${CMAKE_SOURCE_DIR}/tests/config.cc
                  ${CMAKE_SOURCE_DIR}/tests/main.cc)

target_link_libraries(ut CONAN_PKG::gtest CONAN_PKG::spdlog CONAN_PKG::nlohmann_json)