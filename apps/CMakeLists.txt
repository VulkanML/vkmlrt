


add_executable(trial trial.cpp)
target_include_directories(trial PRIVATE "inc")
target_link_libraries(trial runtime)