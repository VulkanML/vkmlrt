cmake_minimum_required(VERSION 3.12)
project(vkmlrt VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS YES)


find_package(Vulkan REQUIRED)
if (WIN32)
   set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
   # set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_subdirectory(ext/volk)
add_subdirectory(ext/VulkanMemoryAllocator)

add_library(vkmlrt STATIC inc/buf.h inc/rt.h src/rt.cpp )
target_include_directories(vkmlrt PUBLIC inc)
target_include_directories(vkmlrt PUBLIC ext/volk) 
target_include_directories(vkmlrt PUBLIC ext/VulkanMemoryAllocator/include)

target_link_libraries(vkmlrt PUBLIC Vulkan::Vulkan )
target_link_libraries(vkmlrt PUBLIC volk)
target_link_libraries(vkmlrt PRIVATE VulkanMemoryAllocator)

if(WIN32)
    set(VKMLRT_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/lib/vkmlrt.dll)
else()
   set(VKMLRT_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/lib/vkmlrt.so)
endif()



# install(TARGETS vkmlrt EXPORT vkmlrtTargets 
#         RUNTIME DESTINATION bin
#         LIBRARY DESTINATION lib
#         ARCHIVE DESTINATION lib)

# # Create a package configuration file
# install(EXPORT vkmlrtTargets 
#         FILE vkmlrtConfig.cmake
#         NAMESPACE vkmlrt::
#         DESTINATION cmake)


add_subdirectory(apps)

