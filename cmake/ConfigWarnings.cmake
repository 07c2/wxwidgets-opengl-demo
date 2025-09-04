# cmake/ConfigWarnings.cmake
# Centralized, toolchain-friendly warning setup for this project.
# Usage in top-level CMakeLists:
#   include(cmake/ConfigWarnings.cmake OPTIONAL)
#   WXGL_apply_warnings(<your-target>)
#
# Notes:
# - Keeps flags moderate to avoid false positives across compilers.
# - You can enforce Werror locally with -DWXGL_WARNINGS_AS_ERRORS=ON
#   or automatically on CI via the CI environment variable.

if(DEFINED WXGL_WARNINGS_INCLUDED)
    return()
endif()
set(WXGL_WARNINGS_INCLUDED TRUE)

option(WXGL_ENABLE_WARNINGS "Enable reasonable warnings for this project" ON)
option(WXGL_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

# If running on CI (common env var), default to treating warnings as errors.
if(DEFINED ENV{CI} AND NOT WXGL_WARNINGS_AS_ERRORS)
    set(WXGL_WARNINGS_AS_ERRORS ON)
endif()

function(WXGL_apply_warnings target)
    if(NOT TARGET ${target})
        message(WARNING "[ConfigWarnings] Target '${target}' not found when applying warnings.")
        return()
    endif()

    if(NOT WXGL_ENABLE_WARNINGS)
        return()
    endif()

    if(MSVC)
        # /permissive- for standard-conforming mode, /EHsc for C++ exceptions model
        target_compile_options(${target} PRIVATE /W4 /permissive- /EHsc)
        if(WXGL_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
        # Common noisy-but-safe disables can be added here if needed:
        # target_compile_options(${target} PRIVATE /wd4100 /wd4244)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            # Keep it moderate; enable selectively if codebase is warning-clean:
            # -Wshadow -Wconversion -Wsign-conversion
        )

        # Prefer colored diagnostics when supported
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag("-fdiagnostics-color=always" WXGL_HAS_COLOR_DIAG)
        if(WXGL_HAS_COLOR_DIAG)
            target_compile_options(${target} PRIVATE -fdiagnostics-color=always)
        endif()

        if(WXGL_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    else()
        message(STATUS "[ConfigWarnings] Unknown compiler '${CMAKE_CXX_COMPILER_ID}', using default flags.")
    endif()
endfunction()