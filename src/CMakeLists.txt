


set(HEADER_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
include_directories(${HEADER_ROOT})
add_library(runtime STATIC rt.cpp ${HEADER_ROOT}/pgrm.hpp ${HEADER_ROOT}/buf.hpp ${HEADER_ROOT}/dev.hpp ${HEADER_ROOT}/rt.hpp)
target_include_directories( runtime PUBLIC "${Vulkan_INCLUDE_DIRS}" )
target_link_libraries(runtime "${Vulkan_LIBRARIES}")
target_link_libraries(runtime Vulkan::Vulkan)
target_link_libraries(runtime volk)
