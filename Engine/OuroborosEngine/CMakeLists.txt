cmake_minimum_required(VERSION 3.22.1)

message("From OuruborosEngine")

set(CMAKE_CXX_STANDARD 20)
set(PROGRAM_NAME Engine)
project(${PROGRAM_NAME})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  src/*.cpp
  src/*.h
  src/*.inl
)

add_executable (${PROGRAM_NAME} ${SOURCES})

target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyobjloader)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/brigand/include)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/boost_1_79_0)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/yaml-cpp/include)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/lua-5.4.2/include)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/sol2/include)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../shared)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../external)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../external/glew-2.1.0/include)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../external/glm)
target_include_directories(${PROGRAM_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../external/glfw/include)

if (NOT TARGET ImGuizmo)
    add_subdirectory(
        ../ImGuizmo/ImGuizmo
        ${CMAKE_CURRENT_SOURCE_DIR}/build/ImGuizmo/ImGuizmo
)
endif()

add_subdirectory(
    OuroborosRenderer
)

set(
    ADDITIONAL_LIBRARIES 
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/yaml-cpp/build/Debug/libyaml-cppmdd.lib optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/yaml-cpp/build/Release/libyaml-cppmd.lib
)

target_link_libraries(${PROGRAM_NAME} 
    PRIVATE
    ouroboros::renderer 
    ImGuizmo
    ImGui
    ${ADDITIONAL_LIBRARIES}
    # ${CMAKE_CURRENT_SOURCE_DIR}/external/yaml-cpp/build/Release/libyaml-cppmd.lib
)

add_dependencies(${PROGRAM_NAME} ouroboros::renderer) #ImGuizmo)

foreach(FILE ${SOURCES}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "Source Files${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "Header Files${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.inl")
       set(GROUP "Header Files${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()
