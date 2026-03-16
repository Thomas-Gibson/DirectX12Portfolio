cmake_minimum_required(VERSION 3.20)
project(Dx12AppLauncher)

file(GLOB_RECURSE SOURCE_FILES "*.cpp" "*.h" "*.hpp")

if(NOT SOURCE_FILES)
    message(FATAL_ERROR "CMake couldn't find any source files! Check your paths.")
endif()

add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})

# Link to your Core. 
target_link_libraries(${PROJECT_NAME} PRIVATE Dx12Core)

# Copy D3D12 folder
set(LOCAL_AGILITY_DLL "${CMAKE_BINARY_DIR}/agility_sdk/build/native/bin/x64/D3D12Core.dll")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/D3D12"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LOCAL_AGILITY_DLL}" 
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/D3D12/D3D12Core.dll"
    COMMENT "Copying Agility SDK runtimes from automated build directory..."
)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Projects")

target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_SOURCE_DIR="${CMAKE_SOURCE_DIR}/")