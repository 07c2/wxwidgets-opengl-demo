// src/render/Shader.cpp
#include "Shader.h"

#include <vector>
#include <sstream>

#include "glad/glad.h"

Shader::~Shader()
{
    Reset();
}

void Shader::Reset()
{
    if (m_prog) {
        glDeleteProgram(m_prog);
        m_prog = 0;
    }
    m_lastLog.clear();
}

bool Shader::compile(GLuint& outShader, unsigned type, const char* src, std::string& log)
{
    outShader = 0;
    if (!src) {
        log += "Empty shader source provided.\n";
        return false;
    }

    GLuint sh = glCreateShader(type);
    if (!sh) {
        log += "glCreateShader failed.\n";
        return false;
    }

    const GLchar* sources[1] = { src };
    glShaderSource(sh, 1, sources, nullptr);
    glCompileShader(sh);

    GLint ok = GL_FALSE;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);

    // Fetch info log if present
    GLint logLen = 0;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 1) {
        std::vector<GLchar> buf(static_cast<size_t>(logLen), 0);
        GLsizei written = 0;
        glGetShaderInfoLog(sh, logLen, &written, buf.data());
        log.append(buf.data(), buf.data() + (written > 0 ? written : 0));
        if (log.back() != '\n') log.push_back('\n');
    }

    if (!ok) {
        glDeleteShader(sh);
        return false;
    }

    outShader = sh;
    return true;
}

bool Shader::link(GLuint prog, GLuint vs, GLuint fs, std::string& log)
{
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);

    GLint logLen = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 1) {
        std::vector<GLchar> buf(static_cast<size_t>(logLen), 0);
        GLsizei written = 0;
        glGetProgramInfoLog(prog, logLen, &written, buf.data());
        log.append(buf.data(), buf.data() + (written > 0 ? written : 0));
        if (log.back() != '\n') log.push_back('\n');
    }

    // Detach & delete shaders regardless of success (no longer needed after link)
    glDeleteShader(vs);
    glDeleteShader(fs);

    return ok == GL_TRUE;
}

bool Shader::CompileFromSource(const char* vs_src,
                               const char* fs_src,
                               const char* debugName)
{
    Reset();

    std::ostringstream ctx;
    if (debugName && *debugName)
        ctx << "[" << debugName << "] ";

    GLuint vs = 0, fs = 0;
    std::string log;

    if (!compile(vs, GL_VERTEX_SHADER, vs_src, log)) {
        m_lastLog = ctx.str() + "Vertex shader compilation failed:\n" + log;
        return false;
    }

    if (!compile(fs, GL_FRAGMENT_SHADER, fs_src, log)) {
        if (vs) glDeleteShader(vs);
        m_lastLog = ctx.str() + "Fragment shader compilation failed:\n" + log;
        return false;
    }

    GLuint prog = glCreateProgram();
    if (!prog) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        m_lastLog = ctx.str() + "glCreateProgram failed.\n";
        return false;
    }

    std::string linkLog;
    const bool linked = link(prog, vs, fs, linkLog);
    if (!linked) {
        glDeleteProgram(prog);
        m_lastLog = ctx.str() + "Program link failed:\n" + linkLog;
        return false;
    }

    m_prog = prog;
    if (!linkLog.empty()) {
        m_lastLog = ctx.str() + linkLog;
    } else {
        m_lastLog.clear();
    }
    return true;
}

void Shader::Use() const
{
    if (m_prog) {
        glUseProgram(m_prog);
    }
}