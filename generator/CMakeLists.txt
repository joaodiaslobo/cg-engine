project(generator)

find_package(tinyxml2 CONFIG REQUIRED)

add_executable(generator src/main.cpp src/generator.cpp)
target_link_libraries(generator PRIVATE tinyxml2::tinyxml2)