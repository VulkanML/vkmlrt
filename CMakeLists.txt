cmake_minimum_required(VERSION 3.12)
project(vkmlrt VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)


find_package(Vulkan REQUIRED)
if (WIN32)
   set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()

add_subdirectory(ext/volk)
add_subdirectory(ext/VulkanMemoryAllocator)

include_directories(inc)
add_library(runtime STATIC src/rt.cpp inc/pgrm.hpp inc/buf.hpp inc/dev.hpp inc/rt.hpp)
target_include_directories( runtime PRIVATE ext/volk ext/VulkanMemoryAllocator/include)
target_include_directories( runtime PUBLIC "${Vulkan_INCLUDE_DIRS}" )
target_link_libraries(runtime "${Vulkan_LIBRARIES}")
target_link_libraries(runtime volk)

add_subdirectory(apps)

