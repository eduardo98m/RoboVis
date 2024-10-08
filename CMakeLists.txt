cmake_minimum_required(VERSION 3.12)

project(RoboVis)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


find_package(raylib REQUIRED)

include_directories("src" "thirdParty/rlImGui" "thirdParty/imgui" "thirdParty/rlights")

SET(RLIGHTS_SOURCES
    thirdParty/rlights/rlights.cpp)

set(IMGUI_SOURCES
    thirdParty/imgui/imgui.cpp
    thirdParty/imgui/imgui_demo.cpp
    thirdParty/imgui/imgui_draw.cpp
    thirdParty/imgui/imgui_widgets.cpp
    thirdParty/imgui/imgui_tables.cpp
    thirdParty/imgui/backends/imgui_impl_opengl3.cpp
)

set(RAYMGUI_SOURCES
    thirdParty/rlImGui/rlImGui.cpp
)

set(SOURCES
    src/DrawingUtils.cpp
    src/Visualizer.cpp
    src/Visualizer_visual_objects.cpp
)


set(EXAMPLE_SOURCES
    examples/SpringMassSimulation.cpp
)

add_compile_definitions(SHADER_BASE_PATH="${CMAKE_SOURCE_DIR}/src/RoboVis/shaders")

add_library(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${RAYMGUI_SOURCES} ${RLIGHTS_SOURCES})

add_executable(SpringMassSimulation ${EXAMPLE_SOURCES} ${SOURCES} ${IMGUI_SOURCES} ${RAYMGUI_SOURCES} ${RLIGHTS_SOURCES})


target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(SpringMassSimulation PRIVATE raylib)