cmake_minimum_required(VERSION 3.22.1)

message("From ImGuizmo")

set(LIBRARY_NAME ImGuizmo)
project(${LIBRARY_NAME})

# LIB_OBJS = ImGuizmo.o GraphEditor.o ImCurveEdit.o ImGradient.o ImSequencer.o
# EXAMPLE_OBJS = example/imgui.o example/imgui_draw.o example/imgui_tables.o example/imgui_widgets.o example/main.o

add_library(
    ${LIBRARY_NAME} STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GraphEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImCurveEdit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGradient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGuizmo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImSequencer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/GraphEditor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ImCurveEdit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ImGradient.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ImGuizmo.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ImSequencer.h

    ${CMAKE_CURRENT_SOURCE_DIR}/example/imgui.h
    )

install(DIRECTORY       ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/install/
)
install(TARGETS         ${LIBRARY_NAME}
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/install/
)

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/example)