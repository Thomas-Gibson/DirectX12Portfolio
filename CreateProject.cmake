#use: cmake -DNAME=MyApp -P CreateProject.cmake

get_filename_component(SCRIPT_DIR "${CMAKE_SCRIPT_MODE_FILE}" PATH)

if(NOT NAME)
    message(FATAL_ERROR "Usage: cmake -DNAME=MyApp -P CreateProject.cmake")
endif()

# We set APP_NAME so configure_file can find @APP_NAME@ in your template
set(APP_NAME ${NAME})
set(TEMPLATE_DIR "${SCRIPT_DIR}/Projects/Template")
set(TARGET_DIR   "${SCRIPT_DIR}/Projects/${NAME}")

file(MAKE_DIRECTORY "${TARGET_DIR}")

# 1. Generate Header
configure_file(
    "${TEMPLATE_DIR}/Dx12App.hpp" 
    "${TARGET_DIR}/${NAME}App.hpp" 
    @ONLY
)

# 2. Generate Source
configure_file(
    "${TEMPLATE_DIR}/Dx12App.cpp" 
    "${TARGET_DIR}/${NAME}App.cpp" 
    @ONLY
)

# 3. Generate the local CMakeLists.txt <--- NEW STEP
configure_file("${TEMPLATE_DIR}/TemplateCMakeLists.txt.in" "${TARGET_DIR}/CMakeLists.txt" @ONLY)

message("--- Boom! Generated ${NAME}.hpp and ${NAME}.cpp ---")