cmake_minimum_required(VERSION 3.22.1)

message("From OuruborosRenderer")

set(CMAKE_CXX_STANDARD 20)

set(LIBRARY_NAME Renderer)
project(${LIBRARY_NAME})

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.inl
)

set(
    ADDITIONAL_LIBRARIES 
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/lib_debug/GenericCodeGend.lib                    optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/GenericCodeGen.lib
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/lib_debug/glslangd.lib                           optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/glslang.lib
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/lib_debug/glslang-default-resource-limitsd.lib   optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/glslang-default-resource-limits.lib
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/lib_debug/HLSLd.lib                              optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/HLSL.lib
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/lib_debug/MachineIndependentd.lib                optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/MachineIndependent.lib
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/lib_debug/OGLCompilerd.lib                       optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/OGLCompiler.lib
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/lib_debug/OSDependentd.lib                       optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/OSDependent.lib
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/lib_debug/SPIRVd.lib                             optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/SPIRV.lib
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/lib_debug/SPVRemapperd.lib                       optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/SPVRemapper.lib
)

add_library(
    ${LIBRARY_NAME} STATIC 
    ${SRC_FILES} ${HEADER_FILES}
    )

add_library(ouroboros::renderer ALIAS ${LIBRARY_NAME})

add_subdirectory(../../external/glfw
${CMAKE_CURRENT_SOURCE_DIR}/glfw)

if (NOT TARGET ImGui)
    add_subdirectory(
            ../../ImGuizmo/ImGui
            ${CMAKE_CURRENT_SOURCE_DIR}/build/ImGuizmo/ImGui
)
endif()

if (NOT TARGET ImGuizmo)
    add_subdirectory(
            ../../ImGuizmo/ImGuizmo
            ${CMAKE_CURRENT_SOURCE_DIR}/build/ImGuizmo/ImGuizmo
)
endif()

target_link_libraries(${LIBRARY_NAME} 
PUBLIC
    ImGui
    ImGuizmo
    glfw
    ${ADDITIONAL_LIBRARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/lib/vulkan-1.lib
)

target_link_libraries(${LIBRARY_NAME} PUBLIC ImGuizmo)
add_dependencies(${LIBRARY_NAME} ImGuizmo)

install(DIRECTORY       ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/install/
)

install(TARGETS         ${LIBRARY_NAME}
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/install/
)

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glslang/include)

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/opengl)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../external)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../shared)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../external/vulkan/include)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../external/glew-2.1.0/include)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../external/glm)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../external/glfw/include)