cmake_minimum_required(VERSION 3.20)
project(DirectX12Portfolio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(OUTPUT_BIN "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BIN})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_BIN})

foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} config_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${OUTPUT_BIN})
endforeach()

if(NOT DXC_EXECUTABLE)
    # Try simple lookup on PATH first
    find_program(DXC_EXECUTABLE dxc HINTS ENV PATH)
endif()

if(NOT DXC_EXECUTABLE)
    # Search known Windows Kits folders (may contain versioned subfolders)
    file(GLOB DXC_CANDIDATES
        "$ENV{ProgramFiles(x86)}/Windows Kits/10/bin/*/x64/dxc.exe"
        "$ENV{ProgramFiles}/Windows Kits/10/bin/*/x64/dxc.exe"
        "$ENV{WindowsSdkDir}/bin/*/x64/dxc.exe"
    )
    list(LENGTH DXC_CANDIDATES _dxclen)
    if(_dxclen GREATER 0)
        list(GET DXC_CANDIDATES 0 DXC_EXECUTABLE)
    endif()
endif()

if(NOT DXC_EXECUTABLE)
    message(WARNING "DXC (dxc.exe) not found. Shader compilation custom commands will be skipped. Set -DDXC_EXECUTABLE=path/to/dxc.exe to override.")
else()
    message(STATUS "Found DXC: ${DXC_EXECUTABLE}")
endif()


function(compile_shader TARGET_NAME SHADER_FILE ENTRY PROFILE)
    if(NOT DXC_EXECUTABLE)
        message(WARNING "Skipping shader compile for ${SHADER_FILE}: dxc not found")
        return()
    endif()

    get_filename_component(FILE_NAME ${SHADER_FILE} NAME_WE)
    get_filename_component(SOURCE_FILE ${SHADER_FILE} ABSOLUTE)

    set(TARGET_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}")
    set(OUTPUT_FILE "${TARGET_OUTPUT_DIR}/${FILE_NAME}.cso")

    add_custom_command(
        OUTPUT "${OUTPUT_FILE}"
        # Create the directory at build-time
        COMMAND ${CMAKE_COMMAND} -E make_directory "${TARGET_OUTPUT_DIR}"
        # Run DXC
        COMMAND "${DXC_EXECUTABLE}" -T ${PROFILE} -E ${ENTRY} -Fo "${OUTPUT_FILE}" "${SOURCE_FILE}"
        DEPENDS "${SOURCE_FILE}"
        COMMENT "DXC: Compiling ${FILE_NAME} [${PROFILE}]"
        VERBATIM
    )

    set(SHADER_TARGET "${TARGET_NAME}_${FILE_NAME}_gen")
    add_custom_target(${SHADER_TARGET} DEPENDS "${OUTPUT_FILE}")

    add_dependencies(${TARGET_NAME} ${SHADER_TARGET})

    set_source_files_properties("${OUTPUT_FILE}" PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
endfunction()

set(CMAKE_DEBUG_POSTFIX "")

# ThridParty
add_subdirectory(ThirdParty/ImGui)

# Engine 
add_subdirectory(Engine/Dx12Core)
add_subdirectory(Engine/Dx12AppLauncher)

# Projects
file(GLOB ALL_PROJECT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Projects/*")
foreach(FULL_PATH ${ALL_PROJECT_DIRS})
    if(IS_DIRECTORY "${FULL_PATH}" AND EXISTS "${FULL_PATH}/CMakeLists.txt")
        get_filename_component(DIR_NAME "${FULL_PATH}" NAME)
        if(NOT "${DIR_NAME}" STREQUAL "Template") # skips template directory
            message(STATUS "Configuring Project: ${DIR_NAME}")
            add_subdirectory("${FULL_PATH}" "${CMAKE_BINARY_DIR}/Projects/${DIR_NAME}") # build this project
        endif()
    endif()
endforeach()