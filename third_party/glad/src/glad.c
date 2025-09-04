/* third_party/glad/src/glad.c
 *
 * Minimal OpenGL symbol loader to match the trimmed header in
 * third_party/glad/include/glad/glad.h.
 *
 * This is NOT the official GLAD generator output; it's a tiny,
 * portable loader sufficient for this demo. You may swap it for a
 * full GLAD build at any time.
 */

#include "glad/glad.h"

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#elif defined(__APPLE__)
#  include <dlfcn.h>
#else
#  include <dlfcn.h>
#endif

/* ---- Globals: function pointers ---- */
/* Base */
PFNGLCLEARPROC                 glad_glClear = 0;
PFNGLCLEARCOLORPROC            glad_glClearColor = 0;
PFNGLVIEWPORTPROC              glad_glViewport = 0;
PFNGLENABLEPROC                glad_glEnable = 0;
PFNGLDISABLEPROC               glad_glDisable = 0;
PFNGLBLENDFUNCPROC             glad_glBlendFunc = 0;
PFNGLGETERRORPROC              glad_glGetError = 0;
PFNGLGETSTRINGPROC             glad_glGetString = 0;

/* Buffers/VAO */
PFNGLGENBUFFERSPROC            glad_glGenBuffers = 0;
PFNGLBINDBUFFERPROC            glad_glBindBuffer = 0;
PFNGLBUFFERDATAPROC            glad_glBufferData = 0;
PFNGLDELETEBUFFERSPROC         glad_glDeleteBuffers = 0;

PFNGLGENVERTEXARRAYSPROC       glad_glGenVertexArrays = 0;
PFNGLBINDVERTEXARRAYPROC       glad_glBindVertexArray = 0;
PFNGLDELETEVERTEXARRAYSPROC    glad_glDeleteVertexArrays = 0;

PFNGLVERTEXATTRIBPOINTERPROC   glad_glVertexAttribPointer = 0;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray = 0;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray = 0;

/* Shaders/programs */
PFNGLCREATESHADERPROC          glad_glCreateShader = 0;
PFNGLSHADERSOURCEPROC          glad_glShaderSource = 0;
PFNGLCOMPILESHADERPROC         glad_glCompileShader = 0;
PFNGLGETSHADERIVPROC           glad_glGetShaderiv = 0;
PFNGLGETSHADERINFOLOGPROC      glad_glGetShaderInfoLog = 0;
PFNGLDELETESHADERPROC          glad_glDeleteShader = 0;

PFNGLCREATEPROGRAMPROC         glad_glCreateProgram = 0;
PFNGLATTACHSHADERPROC          glad_glAttachShader = 0;
PFNGLLINKPROGRAMPROC           glad_glLinkProgram = 0;
PFNGLGETPROGRAMIVPROC          glad_glGetProgramiv = 0;
PFNGLGETPROGRAMINFOLOGPROC     glad_glGetProgramInfoLog = 0;
PFNGLUSEPROGRAMPROC            glad_glUseProgram = 0;
PFNGLDELETEPROGRAMPROC         glad_glDeleteProgram = 0;

PFNGLGETUNIFORMLOCATIONPROC    glad_glGetUniformLocation = 0;
PFNGLUNIFORM1IPROC             glad_glUniform1i = 0;
PFNGLUNIFORMMATRIX4FVPROC      glad_glUniformMatrix4fv = 0;
PFNGLUNIFORM4FPROC             glad_glUniform4f = 0;

PFNGLGETATTRIBLOCATIONPROC     glad_glGetAttribLocation = 0;

/* Textures */
PFNGLGENTEXTURESPROC           glad_glGenTextures = 0;
PFNGLBINDTEXTUREPROC           glad_glBindTexture = 0;
PFNGLTEXIMAGE2DPROC            glad_glTexImage2D = 0;
PFNGLTEXPARAMETERIPROC         glad_glTexParameteri = 0;
PFNGLDELETETEXTURESPROC        glad_glDeleteTextures = 0;
PFNGLACTIVETEXTUREPROC         glad_glActiveTexture = 0;

/* Drawing */
PFNGLDRAWARRAYSPROC            glad_glDrawArrays = 0;
PFNGLDRAWELEMENTSPROC          glad_glDrawElements = 0;

/* ---- Platform loader helpers ---- */

#if defined(_WIN32)

static HMODULE wxgl_libgl = NULL;

typedef void* (APIENTRY *PFNWGLGETPROCADDRESS)(const char *name);

static PFNWGLGETPROCADDRESS wxgl_wglGetProcAddress = NULL;

static int wxgl_open_libgl(void) {
    if (wxgl_libgl) return 1;
    wxgl_libgl = GetModuleHandleA("opengl32.dll");
    if (!wxgl_libgl) {
        wxgl_libgl = LoadLibraryA("opengl32.dll");
    }
    if (!wxgl_libgl) return 0;
    wxgl_wglGetProcAddress = (PFNWGLGETPROCADDRESS)GetProcAddress(wxgl_libgl, "wglGetProcAddress");
    return 1;
}

static int wxgl_is_invalid_ptr(void* p) {
    return p == NULL || p == (void*)0x1 || p == (void*)0x2 ||
           p == (void*)0x3 || p == (void*)-1;
}

static void* wxgl_get_proc(const char* name) {
    void* p = NULL;
    if (!wxgl_libgl && !wxgl_open_libgl()) return NULL;
    if (wxgl_wglGetProcAddress) {
        p = wxgl_wglGetProcAddress(name);
        if (!wxgl_is_invalid_ptr(p)) {
            return p;
        }
    }
    return (void*)GetProcAddress(wxgl_libgl, name);
}

#elif defined(__APPLE__)

static void* wxgl_libgl = NULL;

static int wxgl_open_libgl(void) {
    if (wxgl_libgl) return 1;
    /* Prefer framework path; fall back to libGL.dylib if needed */
    wxgl_libgl = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY | RTLD_LOCAL);
    if (!wxgl_libgl) {
        wxgl_libgl = dlopen("libGL.dylib", RTLD_LAZY | RTLD_LOCAL);
    }
    return wxgl_libgl != NULL;
}

static void* wxgl_get_proc(const char* name) {
    if (!wxgl_libgl && !wxgl_open_libgl()) return NULL;
    return dlsym(wxgl_libgl, name);
}

#else /* Linux/Unix (GLX) */

static void* wxgl_libgl = NULL;

typedef void* (*PFNGLXGETPROCADDRESSPROC)(const unsigned char*);

static PFNGLXGETPROCADDRESSPROC wxgl_glXGetProcAddress = NULL;

static int wxgl_open_libgl(void) {
    if (wxgl_libgl) return 1;
    /* libGL.so.1 is the usual SONAME; fall back to libGL.so */
    wxgl_libgl = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!wxgl_libgl) {
        wxgl_libgl = dlopen("libGL.so", RTLD_LAZY | RTLD_LOCAL);
    }
    if (!wxgl_libgl) return 0;

    wxgl_glXGetProcAddress = (PFNGLXGETPROCADDRESSPROC)dlsym(wxgl_libgl, "glXGetProcAddressARB");
    if (!wxgl_glXGetProcAddress) {
        wxgl_glXGetProcAddress = (PFNGLXGETPROCADDRESSPROC)dlsym(wxgl_libgl, "glXGetProcAddress");
    }
    return 1;
}

static void* wxgl_get_proc(const char* name) {
    if (!wxgl_libgl && !wxgl_open_libgl()) return NULL;
    void* p = NULL;
    if (wxgl_glXGetProcAddress) {
        p = wxgl_glXGetProcAddress((const unsigned char*)name);
    }
    if (!p) {
        p = dlsym(wxgl_libgl, name);
    }
    return p;
}

#endif

/* ---- Public loader ---- */

int gladLoadGL(void) {
    if (!wxgl_open_libgl()) {
        return 0;
    }

    int missing = 0;

#define WXGL_LOAD(required_type, var, sym) \
    do { \
        var = (required_type)wxgl_get_proc(sym); \
        if (!(var)) { missing++; } \
    } while (0)

    /* Base */
    WXGL_LOAD(PFNGLCLEARPROC,               glad_glClear,               "glClear");
    WXGL_LOAD(PFNGLCLEARCOLORPROC,          glad_glClearColor,          "glClearColor");
    WXGL_LOAD(PFNGLVIEWPORTPROC,            glad_glViewport,            "glViewport");
    WXGL_LOAD(PFNGLENABLEPROC,              glad_glEnable,              "glEnable");
    WXGL_LOAD(PFNGLDISABLEPROC,             glad_glDisable,             "glDisable");
    WXGL_LOAD(PFNGLBLENDFUNCPROC,           glad_glBlendFunc,           "glBlendFunc");
    WXGL_LOAD(PFNGLGETERRORPROC,            glad_glGetError,            "glGetError");
    WXGL_LOAD(PFNGLGETSTRINGPROC,           glad_glGetString,           "glGetString");

    /* Buffers/VAO */
    WXGL_LOAD(PFNGLGENBUFFERSPROC,          glad_glGenBuffers,          "glGenBuffers");
    WXGL_LOAD(PFNGLBINDBUFFERPROC,          glad_glBindBuffer,          "glBindBuffer");
    WXGL_LOAD(PFNGLBUFFERDATAPROC,          glad_glBufferData,          "glBufferData");
    WXGL_LOAD(PFNGLDELETEBUFFERSPROC,       glad_glDeleteBuffers,       "glDeleteBuffers");

    WXGL_LOAD(PFNGLGENVERTEXARRAYSPROC,     glad_glGenVertexArrays,     "glGenVertexArrays");
    WXGL_LOAD(PFNGLBINDVERTEXARRAYPROC,     glad_glBindVertexArray,     "glBindVertexArray");
    WXGL_LOAD(PFNGLDELETEVERTEXARRAYSPROC,  glad_glDeleteVertexArrays,  "glDeleteVertexArrays");

    WXGL_LOAD(PFNGLVERTEXATTRIBPOINTERPROC, glad_glVertexAttribPointer, "glVertexAttribPointer");
    WXGL_LOAD(PFNGLENABLEVERTEXATTRIBARRAYPROC,  glad_glEnableVertexAttribArray, "glEnableVertexAttribArray");
    WXGL_LOAD(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glad_glDisableVertexAttribArray, "glDisableVertexAttribArray");

    /* Shaders/programs */
    WXGL_LOAD(PFNGLCREATESHADERPROC,        glad_glCreateShader,        "glCreateShader");
    WXGL_LOAD(PFNGLSHADERSOURCEPROC,        glad_glShaderSource,        "glShaderSource");
    WXGL_LOAD(PFNGLCOMPILESHADERPROC,       glad_glCompileShader,       "glCompileShader");
    WXGL_LOAD(PFNGLGETSHADERIVPROC,         glad_glGetShaderiv,         "glGetShaderiv");
    WXGL_LOAD(PFNGLGETSHADERINFOLOGPROC,    glad_glGetShaderInfoLog,    "glGetShaderInfoLog");
    WXGL_LOAD(PFNGLDELETESHADERPROC,        glad_glDeleteShader,        "glDeleteShader");

    WXGL_LOAD(PFNGLCREATEPROGRAMPROC,       glad_glCreateProgram,       "glCreateProgram");
    WXGL_LOAD(PFNGLATTACHSHADERPROC,        glad_glAttachShader,        "glAttachShader");
    WXGL_LOAD(PFNGLLINKPROGRAMPROC,         glad_glLinkProgram,         "glLinkProgram");
    WXGL_LOAD(PFNGLGETPROGRAMIVPROC,        glad_glGetProgramiv,        "glGetProgramiv");
    WXGL_LOAD(PFNGLGETPROGRAMINFOLOGPROC,   glad_glGetProgramInfoLog,   "glGetProgramInfoLog");
    WXGL_LOAD(PFNGLUSEPROGRAMPROC,          glad_glUseProgram,          "glUseProgram");
    WXGL_LOAD(PFNGLDELETEPROGRAMPROC,       glad_glDeleteProgram,       "glDeleteProgram");

    WXGL_LOAD(PFNGLGETUNIFORMLOCATIONPROC,  glad_glGetUniformLocation,  "glGetUniformLocation");
    WXGL_LOAD(PFNGLUNIFORM1IPROC,           glad_glUniform1i,           "glUniform1i");
    WXGL_LOAD(PFNGLUNIFORMMATRIX4FVPROC,    glad_glUniformMatrix4fv,    "glUniformMatrix4fv");
    WXGL_LOAD(PFNGLUNIFORM4FPROC,           glad_glUniform4f,           "glUniform4f");

    WXGL_LOAD(PFNGLGETATTRIBLOCATIONPROC,   glad_glGetAttribLocation,   "glGetAttribLocation");

/* Textures */
// WXGL_LOAD(PFNGLGENTEXTURESPROC,         glad_glGenTextures,         "glGenTextures");
// WXGL_LOAD(PFNGLBINDTEXTUREPROC,         glad_glBindTexture,         "glBindTexture");
// WXGL_LOAD(PFNGLTEXIMAGE2DPROC,          glad_glTexImage2D,          "glTexImage2D");
// WXGL_LOAD(PFNGLTEXPARAMETERIPROC,       glad_glTexParameteri,       "glTexParameteri");
// WXGL_LOAD(PFNGLDELETETEXTURESPROC,      glad_glDeleteTextures,      "glDeleteTextures");
// WXGL_LOAD(PFNGLACTIVETEXTUREPROC,       glad_glActiveTexture,       "glActiveTexture");

    glad_glGenTextures   = (PFNGLGENTEXTURESPROC)  wxgl_get_proc("glGenTextures");   if (!glad_glGenTextures)   ++missing;
    glad_glBindTexture   = (PFNGLBINDTEXTUREPROC)  wxgl_get_proc("glBindTexture");   if (!glad_glBindTexture)   ++missing;
    glad_glTexImage2D    = (PFNGLTEXIMAGE2DPROC)   wxgl_get_proc("glTexImage2D");    if (!glad_glTexImage2D)    ++missing;
    glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC)wxgl_get_proc("glTexParameteri"); if (!glad_glTexParameteri) ++missing;
    glad_glDeleteTextures= (PFNGLDELETETEXTURESPROC)wxgl_get_proc("glDeleteTextures");if (!glad_glDeleteTextures)++missing;
    glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC)wxgl_get_proc("glActiveTexture"); if (!glad_glActiveTexture) ++missing;

    /* Drawing */
    WXGL_LOAD(PFNGLDRAWARRAYSPROC,          glad_glDrawArrays,          "glDrawArrays");
    WXGL_LOAD(PFNGLDRAWELEMENTSPROC,        glad_glDrawElements,        "glDrawElements");

#undef WXGL_LOAD

    return missing == 0 ? 1 : 0;
}