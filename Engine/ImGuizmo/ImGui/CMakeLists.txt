cmake_minimum_required(VERSION 3.22.1)

set(GRAPHICS_API "Vulkan")
set(UTILITY_API "GLFW")

message("From ImGui")
message("Using Graphics API")
message(${GRAPHICS_API})

set(LIBRARY_NAME ImGui)

project(${LIBRARY_NAME})
set(
    SHARED_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_widgets.cpp
    )

if("${GRAPHICS_API}" STREQUAL "Vulkan")
    set(
        GRAPHIC_API_DEPENDENCY_FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/backends/imgui_impl_vulkan.cpp
        )
endif()

if("${UTILITY_API}" STREQUAL "GLFW")
    set(
        UTILITY_API_DEPENDENCY_FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/backends/imgui_impl_glfw.cpp
        )
endif()

add_library(
    ${LIBRARY_NAME} STATIC 
    ${SHARED_FILES} 
    ${GRAPHIC_API_DEPENDENCY_FILES} 
    ${UTILITY_API_DEPENDENCY_FILES}
    )

install(DIRECTORY       ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/install/
)

install(TARGETS         ${LIBRARY_NAME}
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/install/
)

if("${UTILITY_API}" STREQUAL "GLFW")
    target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../external/vulkan/include/vulkan)
endif()
if("${UTILITY_API}" STREQUAL "GLFW")
    target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../external/glfw/include)
endif()

add_dependencies(${LIBRARY_NAME} ImGui)

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends)

# add_custom_command(... COMMAND cmake -P copy_to_build.cmake)