cmake_minimum_required(VERSION 3.16)

project(wxgl_overlay_demo
    VERSION 0.1.0
    DESCRIPTION "wxWidgets + OpenGL interactive overlay demo"
    LANGUAGES C CXX)

# ---- C++ standard ----
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Optional, local CMake helpers ----
include(cmake/ConfigWarnings.cmake OPTIONAL)
include(cmake/ToolchainHints.cmake OPTIONAL)

# ---- Dependencies ----
find_package(wxWidgets COMPONENTS core base gl REQUIRED)
find_package(OpenGL REQUIRED)

# wxWidgets legacy module exposes variables and a helper include.
# This sets necessary compile flags and include directories.
include(${wxWidgets_USE_FILE})

# OpenGL loader: glad (vendored)
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)
set_property(TARGET glad PROPERTY C_STANDARD 99)

# ---- Sources ----
set(APP_SOURCES
    src/app/main.cpp
    src/app/MainFrame.cpp  src/app/MainFrame.h
    src/app/GLCanvas.cpp   src/app/GLCanvas.h
    src/app/SidePanel.cpp  src/app/SidePanel.h
    src/app/Events.h
)

set(RENDER_SOURCES
    src/render/Renderer.cpp    src/render/Renderer.h
    src/render/RenderState.h
    src/render/Scene.cpp       src/render/Scene.h
    src/render/UIOverlay.cpp   src/render/UIOverlay.h
    src/render/Texture.cpp     src/render/Texture.h
    src/render/Shader.cpp      src/render/Shader.h
    src/render/Mesh.cpp        src/render/Mesh.h
    src/render/Quad.cpp        src/render/Quad.h
    src/render/GlCheck.h
)

add_executable(wxgl_overlay_demo
    ${APP_SOURCES}
    ${RENDER_SOURCES}
)

# Group sources nicely in IDEs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${APP_SOURCES} ${RENDER_SOURCES})

# Include paths for headers within this repo (e.g., stb)
target_include_directories(wxgl_overlay_demo
    PRIVATE
        src
        third_party/stb
)

# Platform-specific tweaks
if(APPLE)
    # Silence deprecated OpenGL warnings on macOS (OpenGL remains usable)
    target_compile_definitions(wxgl_overlay_demo PRIVATE GL_SILENCE_DEPRECATION=1)
endif()

# Warnings (kept moderate to be toolchain-friendly)
if(MSVC)
    target_compile_options(wxgl_overlay_demo PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(wxgl_overlay_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link libraries
target_link_libraries(wxgl_overlay_demo
    PRIVATE
        ${wxWidgets_LIBRARIES}
        OpenGL::GL
        glad
)

# Resource directory available at runtime (optional helper define)
target_compile_definitions(wxgl_overlay_demo
    PRIVATE APP_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources")

# Copy resources next to the built executable for easy running from build tree
add_custom_command(TARGET wxgl_overlay_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:wxgl_overlay_demo>/resources")