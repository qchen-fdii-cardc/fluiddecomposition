cmake_minimum_required(VERSION 3.10)
project(PODLibrary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable specific warnings for MSVC and add bigobj flag
if(MSVC)
    add_compile_options(/wd4819)  # Disable warning C4819
    add_compile_options(/wd4244)  # Disable warning C4996
    add_compile_options(/bigobj)  # Enable big object files
endif()

# Set Eigen3 path
set(EIGEN3_INCLUDE_DIR "D:/sources/eigen-3.4.0")
include_directories(${EIGEN3_INCLUDE_DIR})

# Create POD library
add_library(pod src/pod.cpp)
target_include_directories(pod PUBLIC include)
target_include_directories(pod PUBLIC ${EIGEN3_INCLUDE_DIR})

# Create demo executable
add_executable(pod_demo examples/pod_demo.cpp)
target_link_libraries(pod_demo PRIVATE pod)
