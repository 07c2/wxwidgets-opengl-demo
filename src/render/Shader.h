// src/render/Shader.h
#pragma once

#include <string>
#include "glad/glad.h"

/**
 * Shader
 * Minimal GLSL shader helper for OpenGL 2.1-era pipelines.
 * - CompileFromSource(vs, fs, debugName): compiles, links and owns a program object.
 * - Use(): glUseProgram(program).
 * - Program(): returns GL program handle (0 if not ready).
 * - LastLog(): returns last compile/link info log (for diagnostics).
 *
 * Notes:
 * - Requires a current GL context.
 * - On failure, LastLog() contains the error messages.
 */
class Shader
{
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Compile and link from in-memory source strings.
    // debugName is optional (can be nullptr) and used only for log context.
    bool CompileFromSource(const char* vs_src,
                           const char* fs_src,
                           const char* debugName);

    // Activate the program (no-op if not compiled).
    void Use() const;

    // Release the GL program.
    void Reset();

    // Accessors
    unsigned Program() const { return m_prog; }
    const std::string& LastLog() const { return m_lastLog; }

private:
    bool compile(GLuint& outShader, unsigned type, const char* src, std::string& log);
    bool link(GLuint prog, GLuint vs, GLuint fs, std::string& log);

private:
    unsigned     m_prog {0};
    std::string  m_lastLog;
};