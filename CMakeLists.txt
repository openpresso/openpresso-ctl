cmake_minimum_required(VERSION 3.24)
project(openpresso-ctl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(openpresso_proto  CONFIG REQUIRED)
find_package(gRPC              CONFIG REQUIRED)
find_package(CLI11             CONFIG REQUIRED)

add_executable(openpresso-ctl
    src/main.cpp
    src/commands.cpp
    src/commands.hpp
    src/util.hpp
)

target_link_libraries(openpresso-ctl PRIVATE
    openpresso_proto::openpresso_proto
    gRPC::grpc++
    CLI11::CLI11
)

install(TARGETS openpresso-ctl RUNTIME DESTINATION bin)