cmake_minimum_required(VERSION 3.12)
project(vkrt LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Specify the language compiler
# Set the C++ compiler for Windows
# if (WIN32)
#   set(CMAKE_CXX_COMPILER cl)
# endif()

add_subdirectory(ext/Vulkan-Headers)
include_directories(ext/Vulkan-Headers/include)
find_package(Vulkan REQUIRED)
add_subdirectory(ext/VulkanMemoryAllocator)
include_directories(ext/VulkanMemoryAllocator/include)
add_subdirectory(ext/volk)



include_directories(inc)
add_library(runtime STATIC src/rt.cpp inc/pgrm.hpp inc/buf.hpp inc/dev.hpp inc/rt.hpp)
target_link_directories(runtime PUBLIC )
add_subdirectory(src)
add_subdirectory(apps)

