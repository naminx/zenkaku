cmake_minimum_required(VERSION 3.16)
project(zenkaku LANGUAGES CXX)

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the executable and source files
add_executable(zenkaku
    zenkaku.cc
)

# Optional: include CLI11 if headers are placed in include/
target_include_directories(zenkaku PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install rule for nix to find a target
install(TARGETS zenkaku
    RUNTIME DESTINATION bin
)
