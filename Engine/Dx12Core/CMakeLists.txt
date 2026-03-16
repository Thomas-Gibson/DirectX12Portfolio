cmake_minimum_required(VERSION 3.20)
project(Dx12Core)

# 1. DISCOVER SOURCES
file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp" "Source/*.h" "Source/*.hpp")
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# 2. AGILITY SDK AUTOMATION (Keep this early)
set(AGILITY_VERSION "1.614.1")
set(AGILITY_URL "https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/${AGILITY_VERSION}")
set(AGILITY_SDK_ROOT "${CMAKE_BINARY_DIR}/agility_sdk")

if(NOT EXISTS "${AGILITY_SDK_ROOT}")
    message(STATUS "Dx12Core: Downloading Agility SDK v${AGILITY_VERSION}...")
    file(DOWNLOAD ${AGILITY_URL} "${CMAKE_BINARY_DIR}/agility.zip" SHOW_PROGRESS)
    file(ARCHIVE_EXTRACT INPUT "${CMAKE_BINARY_DIR}/agility.zip" DESTINATION ${AGILITY_SDK_ROOT})
endif()

# 3. FETCH EXTERNAL DEPENDENCIES (Assimp & DirectXTex)
include(FetchContent)

# --- Assimp ---
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v6.0.4 
  GIT_SHALLOW    ON      
)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

# --- DirectXTK12 ---
FetchContent_Declare(
  directxtk12
  GIT_REPOSITORY https://github.com/microsoft/DirectXTK12.git
  GIT_TAG        sept2023
  GIT_SHALLOW    ON
)

# NOW MAKE THEM BOTH AVAILABLE
FetchContent_MakeAvailable(assimp directxtk12)

# 4. INCLUDE DIRECTORIES
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC "${AGILITY_SDK_ROOT}/build/native/include")

#target_include_directories(Dx12Core PUBLIC 
#    "${directxtex_SOURCE_DIR}/DirectXTex"
#    "${directxtex_SOURCE_DIR}/WICTextureLoader"
#    "${directxtex_SOURCE_DIR}/ScreenGrab"
#)

# Only if the DirectXTK12 target doesn't automatically export Inc
#target_include_directories(${PROJECT_NAME} PUBLIC ${directxtk12_SOURCE_DIR}/Inc)

# 5. LINKING (Now that DirectXTex and Assimp are defined!)
target_link_libraries(${PROJECT_NAME} PUBLIC 
    Dx12ImGui 
    DirectXTK12
    dxgi.lib 
    d3dcompiler.lib 
    dwmapi.lib 
    user32.lib
    #"${AGILITY_SDK_ROOT}/build/native/bin/x64/d3d12.lib" This file doesn't exist only Dx12Core.dll & d3d12SDKLayers.dll & d3dconfig.exe
)

# DirectXTex and Assimp targets are now known to CMake
target_link_libraries(${PROJECT_NAME} PRIVATE 
    assimp::assimp
)

# 6. DEFINITIONS & PROPERTIES
set(AGILITY_DLL_PATH "${AGILITY_SDK_ROOT}/build/native/bin/x64/D3D12Core.dll" CACHE INTERNAL "")

target_compile_definitions(${PROJECT_NAME} PUBLIC 
    D3D12SDKVersion=614
    D3D12SDKPath=\".\\\\D3D12\\\\\"
)