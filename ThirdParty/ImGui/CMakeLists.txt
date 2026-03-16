cmake_minimum_required(VERSION 3.20)
project(Dx12ImGui)

file(GLOB_RECURSE IMGUI_SOURCES
    "*.cpp"
    "*.h"
    "*.hpp"
)


add_library(Dx12ImGui STATIC ${IMGUI_SOURCES})

# It just needs to know WHERE the DX12 headers are to compile
target_include_directories(Dx12ImGui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_SOURCE_DIR}/ThirdParty/AgilitySDK/build/native/include"
)
