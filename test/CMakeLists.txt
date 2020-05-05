add_executable(ut main.cc)

target_link_libraries(ut CONAN_PKG::gtest)