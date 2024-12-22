#---------------------------------------------------------------------------------------------------
# CMake Build Configuration for the Ray Tracing Weekend Series
#
# See README.md for guidance.
#---------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(gpu-raytracer LANGUAGES CXX CUDA)

set(CMAKE_CUDA_STANDARD 20)

# Source
set ( EXTERNAL
)

set ( SOURCE_ONE_WEEKEND
  main.cu
)

set ( SOURCE_NEXT_WEEK
)

set ( SOURCE_REST_OF_YOUR_LIFE
)

# Specific compiler flags below. We're not going to add options for all possible compilers, but if
# you're new to CMake (like we are), the following may be a helpful example if you're using a
# different compiler or want to set different compiler options.

message (STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})
message (STATUS "Release flags: " ${CMAKE_CXX_FLAGS_RELEASE})
message (STATUS "Debug flags: " ${CMAKE_CXX_FLAGS_DEBUG})

# Executables
add_executable(raytracer      ${EXTERNAL} ${SOURCE_ONE_WEEKEND})

# Request that particles be built with -std=c++11
# As this is a public compile feature anything that links to 
# particles will also build with -std=c++11
target_compile_features(raytracer PUBLIC cxx_std_20)
 
# We need to explicitly state that we need all CUDA files in the 
# particle library to be built with -dc as the member functions 
# could be called by other libraries and executables
set_target_properties( raytracer
                       PROPERTIES CUDA_SEPARABLE_COMPILATION ON)