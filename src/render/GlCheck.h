// src/render/GlCheck.h
#pragma once

// Header-only, dependency-free (no wxWidgets) OpenGL error checking helpers.
// Usage:
//   WXGL_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
//   WXGL_GL_CHECKPOINT(); // check at an arbitrary point
//
// Enable checks (default in Debug builds) with:
//   -D WXGL_ENABLE_GL_CHECKS=ON
//
// Disable explicitly (even in Debug) with:
//   -D WXGL_ENABLE_GL_CHECKS=OFF
//
// Notes:
// - Requires a current GL context when calling glGetError.
// - Prints to stderr; integrate with your own logger if desired.

#include "glad/glad.h"
#include <cstdio>

#ifndef WXGL_ENABLE_GL_CHECKS
#  ifndef NDEBUG
#    define WXGL_ENABLE_GL_CHECKS 1
#  else
#    define WXGL_ENABLE_GL_CHECKS 0
#  endif
#endif

namespace wxgl {

// Map common GL error enums to readable strings.
inline const char* GlErrorName(unsigned err)
{
    switch (err) {
        case 0x0000: return "GL_NO_ERROR";
        case 0x0500: return "GL_INVALID_ENUM";
        case 0x0501: return "GL_INVALID_VALUE";
        case 0x0502: return "GL_INVALID_OPERATION";
        case 0x0503: return "GL_STACK_OVERFLOW";
        case 0x0504: return "GL_STACK_UNDERFLOW";
        case 0x0505: return "GL_OUT_OF_MEMORY";
        case 0x0506: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default:     return "GL_UNKNOWN_ERROR";
    }
}

// Drain and report all pending GL errors. Returns true if no errors were found.
inline bool CheckError(const char* file, int line, const char* what)
{
    bool ok = true;
    unsigned err;
    while ((err = glGetError()) != 0u) { // 0 == GL_NO_ERROR
        std::fprintf(stderr,
                     "[GL] error 0x%04X (%s) at %s:%d after %s\n",
                     err, GlErrorName(err),
                     file, line,
                     (what && *what) ? what : "checkpoint");
        ok = false;
        // continue draining in case multiple errors are queued
    }
    return ok;
}

} // namespace wxgl

#if WXGL_ENABLE_GL_CHECKS
    #define WXGL_GL(expr) do { expr; (void)::wxgl::CheckError(__FILE__, __LINE__, #expr); } while(0)
    #define WXGL_GL_CHECKPOINT() (void)::wxgl::CheckError(__FILE__, __LINE__, nullptr)
#else
    #define WXGL_GL(expr) do { expr; } while(0)
    #define WXGL_GL_CHECKPOINT() ((void)0)
#endif