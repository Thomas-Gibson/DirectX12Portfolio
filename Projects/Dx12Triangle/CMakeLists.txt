cmake_minimum_required(VERSION 3.20)
project(Dx12Triangle)

file(GLOB_RECURSE SOURCE_FILES
    "*.cpp"
    "*.h"
    "*.hpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Dx12Core Dx12ImGui)

# Call the function for your Mesh and Amplification shaders
# Provide absolute paths so the root helper can resolve the files reliably
compile_shader(${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/VertexShader.hlsl" "main" "vs_6_0")
compile_shader(${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/PixelShader.hlsl" "main" "ps_6_0")
