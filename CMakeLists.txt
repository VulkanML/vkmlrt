find_package(Vulkan REQUIRED)

cmake_minimum_required(VERSION 3.12)
project(vkrt LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Specify the language compiler
# Set the C++ compiler for Windows
# if (WIN32)
#   set(CMAKE_CXX_COMPILER cl)
# endif()

add_subdirectory(src)

add_executable(trial trial.cpp)
target_include_directories(trial PRIVATE "inc")
target_link_libraries(trial runtime)