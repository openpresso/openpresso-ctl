cmake_minimum_required(VERSION 3.24)

set(OPENPRESSO_CTL_VERSION "0.0.0" CACHE STRING "")
set(OPENPRESSO_CTL_VERSION_MAJOR "0" CACHE STRING "")
set(OPENPRESSO_CTL_VERSION_MINOR "0" CACHE STRING "")
set(OPENPRESSO_CTL_VERSION_PATCH "0" CACHE STRING "")
set(OPENPRESSO_CTL_REPO_CHANNEL "testing" CACHE STRING "")

option(DOCS_ONLY "Create targets only to make documentation" OFF)

project(openpresso-ctl VERSION ${OPENPRESSO_CTL_VERSION_MAJOR}.${OPENPRESSO_CTL_VERSION_MINOR}.${OPENPRESSO_CTL_VERSION_PATCH} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(OPENPRESSOD_SOCKET_PATH "/run/openpresso/openpresso.sock")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(openpresso-ctl-docs)
include(GNUInstallDirs)
include(FetchContent)

add_subdirectory(docs)

if(NOT DOCS_ONLY)
    find_package(openpresso_proto  CONFIG REQUIRED)
    find_package(CLI11             CONFIG REQUIRED)

    add_executable(openpresso-ctl
        src/main.cpp
        src/commands.cpp
        src/commands.hpp
        src/util.hpp
    )

    target_link_libraries(openpresso-ctl PRIVATE
        openpresso_proto::openpresso_proto
        CLI11::CLI11
    )

    target_compile_definitions(openpresso-ctl PRIVATE
        OPENPRESSOD_SOCKET_PATH="${OPENPRESSOD_SOCKET_PATH}"
    )

    install(TARGETS openpresso-ctl RUNTIME DESTINATION bin)

    add_subdirectory(packaging)
endif()