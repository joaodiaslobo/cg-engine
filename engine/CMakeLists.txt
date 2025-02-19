project(engine)

find_package(glfw3 CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)

add_executable(engine src/main.cpp)
target_link_libraries(engine PRIVATE tinyxml2::tinyxml2 glfw)