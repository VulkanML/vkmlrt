find_package(Vulkan REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc)
add_library(runtime STATIC rt.cpp)
target_include_directories( runtime PUBLIC "${Vulkan_INCLUDE_DIRS}" )
target_link_libraries(runtime "${Vulkan_LIBRARIES}")

