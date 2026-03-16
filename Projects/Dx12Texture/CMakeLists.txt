cmake_minimum_required(VERSION 3.20)
project(Dx12Texture)

file(GLOB_RECURSE SOURCE_FILES
    "*.cpp"
    "*.h"
    "*.hpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Dx12Core)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.assetroot" "${CMAKE_CURRENT_SOURCE_DIR}/Assets")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.assetroot" 
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.assetroot"
    COMMENT "Copying ${PROJECT_NAME}.assetroot to output directory"
)


compile_shader(${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/Dx12Textures_VertexShader.hlsl" "main" "vs_6_0")
compile_shader(${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/Dx12Textures_PixelShader.hlsl" "main" "ps_6_0")
