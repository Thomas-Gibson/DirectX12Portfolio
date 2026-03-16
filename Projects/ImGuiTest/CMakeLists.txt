cmake_minimum_required(VERSION 3.20)
project(ImGuiTest)

file(GLOB_RECURSE SOURCE_FILES
    "*.cpp"
    "*.h"
    "*.hpp"
)

add_library(ImGuiTest SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Dx12Core Dx12ImGui)