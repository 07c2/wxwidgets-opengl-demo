# cmake/ToolchainHints.cmake
# Non-intrusive helper hints for locating toolchains & dependencies.
# Included early in the top-level CMakeLists.txt (OPTIONAL).
#
# Goals:
# - Make wxWidgets easier to find across platforms without hard-coding paths.
# - Allow users to provide prefix hints without editing the main CMakeLists.txt.
# - Print actionable guidance for popular package managers (Homebrew, vcpkg).
#
# This file should NOT enforce toolchains or fail configuration by itself.

# ---------------------------------------------------------------------------
# 1) wx-config hint (used by CMake's FindwxWidgets)
#    Users may specify a custom wx-config path via -DWXGL_WX_CONFIG=/path/to/wx-config
#    Otherwise try to auto-detect common names/locations.
# ---------------------------------------------------------------------------
if(NOT wxWidgets_CONFIG_EXECUTABLE)
    set(WXGL_WX_CONFIG "" CACHE FILEPATH "Path to wx-config executable (overrides auto-detection)")
    if(WXGL_WX_CONFIG)
        set(wxWidgets_CONFIG_EXECUTABLE "${WXGL_WX_CONFIG}" CACHE FILEPATH "" FORCE)
        message(STATUS "[ToolchainHints] Using user-provided wx-config: ${wxWidgets_CONFIG_EXECUTABLE}")
    else()
        # Try common names and prefixes
        set(_wx_names wx-config-3.2 wx-config-3.1 wx-config)
        if(APPLE)
            set(_wx_hints /opt/homebrew/bin /usr/local/bin /usr/bin)
        else()
            set(_wx_hints /usr/local/bin /usr/bin)
        endif()
        find_program(_WXGL_WX_CONFIG NAMES ${_wx_names} HINTS ${_wx_hints})
        if(_WXGL_WX_CONFIG)
            set(wxWidgets_CONFIG_EXECUTABLE "${_WXGL_WX_CONFIG}" CACHE FILEPATH "" FORCE)
            message(STATUS "[ToolchainHints] Auto-detected wx-config: ${wxWidgets_CONFIG_EXECUTABLE}")
        else()
            message(STATUS "[ToolchainHints] wx-config not found in PATH. If FindwxWidgets fails, set -DWXGL_WX_CONFIG=/full/path/to/wx-config")
        endif()
    endif()
endif()

# On macOS with Homebrew, provide an extra hint if wx-config exists there.
if(APPLE AND NOT DEFINED wxWidgets_CONFIG_EXECUTABLE)
    if(DEFINED ENV{HOMEBREW_PREFIX})
        set(_brew "$ENV{HOMEBREW_PREFIX}")
    elseif(EXISTS "/opt/homebrew")
        set(_brew "/opt/homebrew")
    elseif(EXISTS "/usr/local")
        set(_brew "/usr/local")
    endif()
    if(_brew AND EXISTS "${_brew}/bin/wx-config")
        set(wxWidgets_CONFIG_EXECUTABLE "${_brew}/bin/wx-config" CACHE FILEPATH "" FORCE)
        message(STATUS "[ToolchainHints] Homebrew wx-config detected: ${wxWidgets_CONFIG_EXECUTABLE}")
    endif()
endif()

# ---------------------------------------------------------------------------
# 2) Extra prefix path (appended to CMAKE_PREFIX_PATH)
#    Useful for custom installs without altering environment variables.
# ---------------------------------------------------------------------------
set(WXGL_EXTRA_PREFIX_PATH "" CACHE PATH "Additional install prefix to append to CMAKE_PREFIX_PATH")
if(WXGL_EXTRA_PREFIX_PATH)
    list(APPEND CMAKE_PREFIX_PATH "${WXGL_EXTRA_PREFIX_PATH}")
    # Keep it visible/editable in cache GUIs
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" CACHE STRING "Prefix list for find_package()" FORCE)
    message(STATUS "[ToolchainHints] Added to CMAKE_PREFIX_PATH: ${WXGL_EXTRA_PREFIX_PATH}")
endif()

# ---------------------------------------------------------------------------
# 3) vcpkg guidance (cannot set toolchain here, only print a hint)
# ---------------------------------------------------------------------------
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "[ToolchainHints] Detected VCPKG_ROOT, but no CMAKE_TOOLCHAIN_FILE set.")
    message(STATUS "  Re-configure with: -DCMAKE_TOOLCHAIN_FILE=$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

# ---------------------------------------------------------------------------
# 4) Friendly distro hints (only informational)
# ---------------------------------------------------------------------------
if(NOT DEFINED wxWidgets_CONFIG_EXECUTABLE)
    if(UNIX AND NOT APPLE)
        message(STATUS "[ToolchainHints] On Debian/Ubuntu, install: sudo apt-get install libwxgtk3.2-dev libgtk-3-dev")
        message(STATUS "[ToolchainHints] On Fedora/RHEL, install:   sudo dnf install wxGTK3-devel gtk3-devel")
    endif()
endif()