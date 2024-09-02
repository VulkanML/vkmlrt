add_executable(trial trial.cpp)

find_library(VKMLRT_LIBRARY NAMES vkmlrt)
message(STATUS "VKMLRT_LIBRARY = ${VKMLRT_LIBRARY}")

target_link_libraries(trial PRIVATE  vkmlrt.dll)
