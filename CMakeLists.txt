cmake_minimum_required(VERSION 3.14)

project(RoboVis)

add_compile_definitions(GRAPHICS_API_OPENGL_43)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Define the macro so that raylib will compile with OpenGL 4.3 support.
find_package(raylib REQUIRED)

# Find the PCL package. You can also specify components if needed:
find_package(PCL 1.12 REQUIRED)

# Add PCL include directories and definitions:
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

include_directories("src" 
                    "thirdParty/rlImGui" 
                    "thirdParty/imgui-filebrowser" 
                    "thirdParty/imgui" 
                    "thirdParty/rlights"
                    "thirdParty/raylib-gizmo/src"
                    "thirdParty/tinyxml2"
                    )

SET(RLIGHTS_SOURCES
    thirdParty/rlights/rlights.cpp)

SET(TINYXML2_SOURCES
    thirdParty/tinyxml2/tinyxml2.cpp)


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
    "src/Systems/Rendering/GizmoRendering.cpp"
    "src/Systems/Rendering/PointCloudRendering.cpp"
    "src/Systems/Rendering/GUIRendering.cpp"
    "src/Systems/Rendering/HeightMapRendering.cpp"
    "src/Systems/Rendering/MapRendering.cpp"
    "src/Systems/GUI/EntitySettings.cpp"
    "src/Systems/Rendering/ShaderManager.cpp"
    "src/Systems/GizmoInteraction.cpp"

    "src/Systems/EntitySelection.cpp"
    "src/Systems/Serialization.cpp"
)


set(ECS_SOURCES
    "src/ECS/EntityManager.cpp"
    "src/ECS/ComponentType.cpp"
)


set(API_SOURCES
    "src/API/ModelAPI.cpp"
    "src/API/GizmoAPI.cpp"
    "src/API/PointCloudAPI.cpp"
    "src/API/HeightMapAPI.cpp"
    "src/API/GUIAPI.cpp"
    "src/API/MapAPI.cpp"
)



set(SOURCES
    "src/VisualizerNew.cpp"
)

set(RAYLIB_GIZMO_SOURCES
    "thirdParty/raylib-gizmo/src/raygizmo.c")


set(EXAMPLE_SOURCES
    examples/SpringMassSimulation.cpp
)

set(ECS_MAIN
    src/ecs_main.cpp
)

add_compile_definitions(SHADER_BASE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

# Skip RPATH to avoid conflicts with Anaconda libraries (Check this later )
#set(CMAKE_SKIP_RPATH TRUE)

add_library(${PROJECT_NAME}
            ${SOURCES}
            ${ECS_MAIN} 
            ${ECS_SOURCES} 
            ${API_SOURCES} 
            ${SYSTEMS_SOURCES} 
            ${IMGUI_SOURCES}
            ${TINYXML2_SOURCES} 
            ${RAYMGUI_SOURCES} 
            ${RLIGHTS_SOURCES}
            ${RAYLIB_GIZMO_SOURCES})


add_executable(SpringMassSimulation 
                ${ECS_MAIN} 
                ${ECS_SOURCES} 
                ${API_SOURCES} 
                ${SYSTEMS_SOURCES} 
                ${SOURCES} 
                ${IMGUI_SOURCES} 
                ${TINYXML2_SOURCES}
                ${RAYMGUI_SOURCES} 
                ${RLIGHTS_SOURCES}
                ${RAYLIB_GIZMO_SOURCES})

# set_target_properties(SpringMassSimulation PROPERTIES
#     BUILD_RPATH_USE_ORIGIN TRUE
#     BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib"
# )

# set_target_properties(SpringMassSimulation PROPERTIES
#     BUILD_RPATH_USE_ORIGIN TRUE
#     BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib"
# )


target_link_libraries(${PROJECT_NAME} PRIVATE raylib ${PCL_LIBRARIES})
target_link_libraries(SpringMassSimulation PRIVATE raylib ${PCL_LIBRARIES})