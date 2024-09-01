cmake_minimum_required(VERSION 3.12)
project(vkrt LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Specify the language compiler
# Set the C++ compiler for Windows
# if (WIN32)
#   set(CMAKE_CXX_COMPILER cl)
# endif()

add_subdirectory(ext/Vulkan-Headers)
add_subdirectory(ext/VulkanMemoryAllocator)
add_subdirectory(ext/volk)
add_subdirectory(src)

