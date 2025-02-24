cmake_minimum_required(VERSION 3.12)

project(RoboVis)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


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

set(SYSTEMS_SOURCES
    "src/Systems/UserCameraInput.cpp"
    "src/Systems/Rendering/Rendering.cpp"
    "src/Systems/Rendering/VisualModelRendering.cpp"
    "src/Systems/GUI/EntitySettings.cpp"
    "src/Systems/Rendering/ShaderManager.cpp"
)


set(ECS_SOURCES
    "src/ECS/EntityManager.cpp"
    "src/ECS/ComponentType.cpp"
)


set(API_SOURCES
    "src/API/ModelAPI.cpp"
)



set(SOURCES
    src/DrawingUtils.cpp
    src/Visualizer.cpp
    src/Visualizer_visual_objects.cpp
    "src/VisualizerNew.cpp"
)


set(EXAMPLE_SOURCES
    examples/SpringMassSimulation.cpp
)

set(ECS_MAIN
    src/ecs_main.cpp
)

add_compile_definitions(SHADER_BASE_PATH="${CMAKE_SOURCE_DIR}/shaders")

add_library(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${RAYMGUI_SOURCES} ${RLIGHTS_SOURCES})

add_executable(SpringMassSimulation ${ECS_MAIN} ${ECS_SOURCES} ${API_SOURCES} ${ENTITY_SOURCES} ${SYSTEMS_SOURCES} ${SOURCES} ${IMGUI_SOURCES} ${RAYMGUI_SOURCES} ${RLIGHTS_SOURCES} )


target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(SpringMassSimulation PRIVATE raylib)