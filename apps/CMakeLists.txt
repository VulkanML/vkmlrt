find_package(Vulkan REQUIRED CONFIG)

add_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_executable(trial trial.cpp)
target_include_directories(trial PRIVATE "inc")
target_link_libraries(trial runtime)