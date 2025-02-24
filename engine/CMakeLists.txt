project(engine)

find_package(glfw3 CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/engine/src)

file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp src/*.h)

add_executable(engine ${ENGINE_SOURCES})
target_link_libraries(engine PRIVATE tinyxml2::tinyxml2 glfw ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})