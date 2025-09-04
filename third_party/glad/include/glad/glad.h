/* third_party/glad/include/glad/glad.h
 *
 * Minimal OpenGL function loader header used by this demo project.
 * NOTE: This is a trimmed, self-contained loader (not the official GLAD output).
 * It exposes only the symbols actually used in the sample code to keep the repo small.
 * You can later replace it with an official generator output if you prefer.
 *
 * Public Domain / MIT-like: use at your own risk.
 */

#ifndef WXGL_MINI_GLAD_H_
#define WXGL_MINI_GLAD_H_

/* ---- C/C++ interop ---- */
#ifdef __cplusplus
extern "C" {
#endif

/* ---- Platform calling convention ---- */
#ifndef APIENTRY
#  if defined(_WIN32) || defined(__WIN32__)
#    define APIENTRY __stdcall
#  else
#    define APIENTRY
#  endif
#endif

/* ---- Basic types (kept minimal) ---- */
#include <stddef.h>
#include <stdint.h>

typedef unsigned int  GLenum;
typedef unsigned char GLboolean;
typedef unsigned int  GLbitfield;
typedef void          GLvoid;
typedef signed char   GLbyte;
typedef short         GLshort;
typedef int           GLint;
typedef int           GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int  GLuint;
typedef float         GLfloat;
typedef double        GLdouble;
typedef ptrdiff_t     GLsizeiptr;
typedef ptrdiff_t     GLintptr;
typedef char          GLchar;

/* ---- Common tokens (only the ones used in this project) ---- */
#ifndef GL_FALSE
#  define GL_FALSE 0
#endif
#ifndef GL_TRUE
#  define GL_TRUE 1
#endif

#ifndef GL_COLOR_BUFFER_BIT
#  define GL_COLOR_BUFFER_BIT 0x00004000
#endif
#ifndef GL_TRIANGLES
#  define GL_TRIANGLES 0x0004
#endif
#ifndef GL_TRIANGLE_STRIP
#  define GL_TRIANGLE_STRIP 0x0005
#endif
#ifndef GL_TRIANGLE_FAN
#  define GL_TRIANGLE_FAN 0x0006
#endif

/* Blending */
#ifndef GL_BLEND
#  define GL_BLEND 0x0BE2
#endif
#ifndef GL_SRC_ALPHA
#  define GL_SRC_ALPHA 0x0302
#endif
#ifndef GL_ONE_MINUS_SRC_ALPHA
#  define GL_ONE_MINUS_SRC_ALPHA 0x0303
#endif

/* Buffers & Arrays */
#ifndef GL_ARRAY_BUFFER
#  define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_ELEMENT_ARRAY_BUFFER
#  define GL_ELEMENT_ARRAY_BUFFER 0x8893
#endif
#ifndef GL_STATIC_DRAW
#  define GL_STATIC_DRAW 0x88E4
#endif
#ifndef GL_FLOAT
#  define GL_FLOAT 0x1406
#endif

/* Textures */
#ifndef GL_TEXTURE_2D
#  define GL_TEXTURE_2D 0x0DE1
#endif
#ifndef GL_TEXTURE0
#  define GL_TEXTURE0 0x84C0
#endif
#ifndef GL_TEXTURE_MIN_FILTER
#  define GL_TEXTURE_MIN_FILTER 0x2801
#endif
#ifndef GL_TEXTURE_MAG_FILTER
#  define GL_TEXTURE_MAG_FILTER 0x2800
#endif
#ifndef GL_TEXTURE_WRAP_S
#  define GL_TEXTURE_WRAP_S 0x2802
#endif
#ifndef GL_TEXTURE_WRAP_T
#  define GL_TEXTURE_WRAP_T 0x2803
#endif
#ifndef GL_CLAMP_TO_EDGE
#  define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_LINEAR
#  define GL_LINEAR 0x2601
#endif
#ifndef GL_NEAREST
#  define GL_NEAREST 0x2600
#endif
#ifndef GL_RGBA
#  define GL_RGBA 0x1908
#endif
#ifndef GL_RGB
#  define GL_RGB 0x1907
#endif
#ifndef GL_UNSIGNED_BYTE
#  define GL_UNSIGNED_BYTE 0x1401
#endif

/* Shaders / Programs */
#ifndef GL_VERTEX_SHADER
#  define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#  define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_COMPILE_STATUS
#  define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_LINK_STATUS
#  define GL_LINK_STATUS 0x8B82
#endif
#ifndef GL_INFO_LOG_LENGTH
#  define GL_INFO_LOG_LENGTH 0x8B84
#endif

/* ---- Function pointer typedefs ---- */
/* GL 1.0/1.1 bits (also loaded to keep code path uniform) */
typedef void     (APIENTRY *PFNGLCLEARPROC)        (GLbitfield mask);
typedef void     (APIENTRY *PFNGLCLEARCOLORPROC)   (GLfloat r, GLfloat g, GLfloat b, GLfloat a);
typedef void     (APIENTRY *PFNGLVIEWPORTPROC)     (GLint x, GLint y, GLsizei w, GLsizei h);
typedef void     (APIENTRY *PFNGLENABLEPROC)       (GLenum cap);
typedef void     (APIENTRY *PFNGLDISABLEPROC)      (GLenum cap);
typedef void     (APIENTRY *PFNGLBLENDFUNCPROC)    (GLenum sfactor, GLenum dfactor);
typedef GLenum   (APIENTRY *PFNGLGETERRORPROC)     (void);
typedef const GLubyte* (APIENTRY *PFNGLGETSTRINGPROC)(GLenum name);

/* VBO/VAO (GL 2.0+/3.0) */
typedef void     (APIENTRY *PFNGLGENBUFFERSPROC)   (GLsizei n, GLuint* buffers);
typedef void     (APIENTRY *PFNGLBINDBUFFERPROC)   (GLenum target, GLuint buffer);
typedef void     (APIENTRY *PFNGLBUFFERDATAPROC)   (GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void     (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);

typedef void     (APIENTRY *PFNGLGENVERTEXARRAYSPROC)   (GLsizei n, GLuint* arrays);
typedef void     (APIENTRY *PFNGLBINDVERTEXARRAYPROC)   (GLuint array);
typedef void     (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint* arrays);

typedef void     (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
typedef void     (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void     (APIENTRY *PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);

/* Shaders / programs (GL 2.0) */
typedef GLuint   (APIENTRY *PFNGLCREATESHADERPROC)   (GLenum type);
typedef void     (APIENTRY *PFNGLSHADERSOURCEPROC)   (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
typedef void     (APIENTRY *PFNGLCOMPILESHADERPROC)  (GLuint shader);
typedef void     (APIENTRY *PFNGLGETSHADERIVPROC)    (GLuint shader, GLenum pname, GLint* param);
typedef void     (APIENTRY *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
typedef void     (APIENTRY *PFNGLDELETESHADERPROC)   (GLuint shader);

typedef GLuint   (APIENTRY *PFNGLCREATEPROGRAMPROC)  (void);
typedef void     (APIENTRY *PFNGLATTACHSHADERPROC)   (GLuint program, GLuint shader);
typedef void     (APIENTRY *PFNGLLINKPROGRAMPROC)    (GLuint program);
typedef void     (APIENTRY *PFNGLGETPROGRAMIVPROC)   (GLuint program, GLenum pname, GLint* param);
typedef void     (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
typedef void     (APIENTRY *PFNGLUSEPROGRAMPROC)     (GLuint program);
typedef void     (APIENTRY *PFNGLDELETEPROGRAMPROC)  (GLuint program);

typedef GLint    (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar* name);
typedef void     (APIENTRY *PFNGLUNIFORM1IPROC)        (GLint location, GLint v0);
typedef void     (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void     (APIENTRY *PFNGLUNIFORM4FPROC)        (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

typedef GLint    (APIENTRY *PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar* name);

/* Textures (GL 1.1/2.0) */
typedef void     (APIENTRY *PFNGLGENTEXTURESPROC)   (GLsizei n, GLuint* textures);
typedef void     (APIENTRY *PFNGLBINDTEXTUREPROC)   (GLenum target, GLuint texture);
typedef void     (APIENTRY *PFNGLTEXIMAGE2DPROC)    (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
typedef void     (APIENTRY *PFNGLTEXPARAMETERIPROC) (GLenum target, GLenum pname, GLint param);
typedef void     (APIENTRY *PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
typedef void     (APIENTRY *PFNGLACTIVETEXTUREPROC) (GLenum texture);

/* Drawing */
typedef void     (APIENTRY *PFNGLDRAWARRAYSPROC)    (GLenum mode, GLint first, GLsizei count);
typedef void     (APIENTRY *PFNGLDRAWELEMENTSPROC)  (GLenum mode, GLsizei count, GLenum type, const void* indices);

/* ---- Extern function pointers (prefixed), plus convenience macros ---- */
/* Base */
extern PFNGLCLEARPROC                 glad_glClear;
extern PFNGLCLEARCOLORPROC            glad_glClearColor;
extern PFNGLVIEWPORTPROC              glad_glViewport;
extern PFNGLENABLEPROC                glad_glEnable;
extern PFNGLDISABLEPROC               glad_glDisable;
extern PFNGLBLENDFUNCPROC             glad_glBlendFunc;
extern PFNGLGETERRORPROC              glad_glGetError;
extern PFNGLGETSTRINGPROC             glad_glGetString;

/* Buffers/VAO */
extern PFNGLGENBUFFERSPROC            glad_glGenBuffers;
extern PFNGLBINDBUFFERPROC            glad_glBindBuffer;
extern PFNGLBUFFERDATAPROC            glad_glBufferData;
extern PFNGLDELETEBUFFERSPROC         glad_glDeleteBuffers;

extern PFNGLGENVERTEXARRAYSPROC       glad_glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC       glad_glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC    glad_glDeleteVertexArrays;

extern PFNGLVERTEXATTRIBPOINTERPROC   glad_glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray;

/* Shaders/programs */
extern PFNGLCREATESHADERPROC          glad_glCreateShader;
extern PFNGLSHADERSOURCEPROC          glad_glShaderSource;
extern PFNGLCOMPILESHADERPROC         glad_glCompileShader;
extern PFNGLGETSHADERIVPROC           glad_glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC      glad_glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC          glad_glDeleteShader;

extern PFNGLCREATEPROGRAMPROC         glad_glCreateProgram;
extern PFNGLATTACHSHADERPROC          glad_glAttachShader;
extern PFNGLLINKPROGRAMPROC           glad_glLinkProgram;
extern PFNGLGETPROGRAMIVPROC          glad_glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC     glad_glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC            glad_glUseProgram;
extern PFNGLDELETEPROGRAMPROC         glad_glDeleteProgram;

extern PFNGLGETUNIFORMLOCATIONPROC    glad_glGetUniformLocation;
extern PFNGLUNIFORM1IPROC             glad_glUniform1i;
extern PFNGLUNIFORMMATRIX4FVPROC      glad_glUniformMatrix4fv;
extern PFNGLUNIFORM4FPROC             glad_glUniform4f;

extern PFNGLGETATTRIBLOCATIONPROC     glad_glGetAttribLocation;

/* Textures */
extern PFNGLGENTEXTURESPROC           glad_glGenTextures;
extern PFNGLBINDTEXTUREPROC           glad_glBindTexture;
extern PFNGLTEXIMAGE2DPROC            glad_glTexImage2D;
extern PFNGLTEXPARAMETERIPROC         glad_glTexParameteri;
extern PFNGLDELETETEXTURESPROC        glad_glDeleteTextures;
extern PFNGLACTIVETEXTUREPROC         glad_glActiveTexture;

/* Drawing */
extern PFNGLDRAWARRAYSPROC            glad_glDrawArrays;
extern PFNGLDRAWELEMENTSPROC          glad_glDrawElements;

/* Map to standard GL names for user code convenience */
#define glClear                      glad_glClear
#define glClearColor                 glad_glClearColor
#define glViewport                   glad_glViewport
#define glEnable                     glad_glEnable
#define glDisable                    glad_glDisable
#define glBlendFunc                  glad_glBlendFunc
#define glGetError                   glad_glGetError
#define glGetString                  glad_glGetString

#define glGenBuffers                 glad_glGenBuffers
#define glBindBuffer                 glad_glBindBuffer
#define glBufferData                 glad_glBufferData
#define glDeleteBuffers              glad_glDeleteBuffers

#define glGenVertexArrays            glad_glGenVertexArrays
#define glBindVertexArray            glad_glBindVertexArray
#define glDeleteVertexArrays         glad_glDeleteVertexArrays

#define glVertexAttribPointer        glad_glVertexAttribPointer
#define glEnableVertexAttribArray    glad_glEnableVertexAttribArray
#define glDisableVertexAttribArray   glad_glDisableVertexAttribArray

#define glCreateShader               glad_glCreateShader
#define glShaderSource               glad_glShaderSource
#define glCompileShader              glad_glCompileShader
#define glGetShaderiv                glad_glGetShaderiv
#define glGetShaderInfoLog           glad_glGetShaderInfoLog
#define glDeleteShader               glad_glDeleteShader

#define glCreateProgram              glad_glCreateProgram
#define glAttachShader               glad_glAttachShader
#define glLinkProgram                glad_glLinkProgram
#define glGetProgramiv               glad_glGetProgramiv
#define glGetProgramInfoLog          glad_glGetProgramInfoLog
#define glUseProgram                 glad_glUseProgram
#define glDeleteProgram              glad_glDeleteProgram

#define glGetUniformLocation         glad_glGetUniformLocation
#define glUniform1i                  glad_glUniform1i
#define glUniformMatrix4fv           glad_glUniformMatrix4fv
#define glUniform4f                  glad_glUniform4f

#define glGetAttribLocation          glad_glGetAttribLocation

#define glGenTextures                glad_glGenTextures
#define glBindTexture                glad_glBindTexture
#define glTexImage2D                 glad_glTexImage2D
#define glTexParameteri              glad_glTexParameteri
#define glDeleteTextures             glad_glDeleteTextures
#define glActiveTexture              glad_glActiveTexture

#define glDrawArrays                 glad_glDrawArrays
#define glDrawElements               glad_glDrawElements

/* ---- Loader entry point ---- */
/* Returns non-zero on success. Must be called with a current GL context. */
int gladLoadGL(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WXGL_MINI_GLAD_H_ */