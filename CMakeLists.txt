# Telegram wrapper for MetaTrader
#
# Install dependencies using vcpkg:
#   .\vcpkg install openssl:x64-windows zlib:x64-windows
#
# Configure build:
#   cmake -A x64 ..
#
# Build:
#   cmake --build . --config Release

cmake_minimum_required(VERSION 3.14)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
    CACHE STRING "Vcpkg toolchain file")
project(tdjson_wrapper VERSION 0.0.1 LANGUAGES CXX C)
# set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_subdirectory(td)

add_library(TDJsonWrapper SHARED tdjson_wrapper.cpp)
target_link_libraries(TDJsonWrapper PRIVATE Td::TdJson)
