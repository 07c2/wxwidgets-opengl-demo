/* third_party/glad/include/KHR/khrplatform.h
 *
 * Minimal Khronos platform header shim for this demo.
 * Provides basic types/macros used by GL/EGL headers.
 * This is intentionally small to keep the repository lightweight.
 */

#ifndef KHRONOS_KHRPLATFORM_H_
#define KHRONOS_KHRPLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Calling convention / visibility ---- */
#ifndef KHRONOS_APIENTRY
#  ifndef APIENTRY
#    if defined(_WIN32) || defined(__WIN32__)
#      define KHRONOS_APIENTRY __stdcall
#    else
#      define KHRONOS_APIENTRY
#    endif
#  else
#    define KHRONOS_APIENTRY APIENTRY
#  endif
#endif

#ifndef KHRONOS_APICALL
#  define KHRONOS_APICALL
#endif

#ifndef KHRONOS_APIATTRIBUTES
#  define KHRONOS_APIATTRIBUTES
#endif

/* ---- Standard integer/size types ---- */
#include <stddef.h>
#include <stdint.h>

typedef int32_t   khronos_int32_t;
typedef uint32_t  khronos_uint32_t;
typedef int64_t   khronos_int64_t;
typedef uint64_t  khronos_uint64_t;
typedef intptr_t  khronos_intptr_t;
typedef uintptr_t khronos_uintptr_t;
typedef ptrdiff_t khronos_ssize_t;

/* ---- Boolean enum sometimes referenced by EGL/GL headers ---- */
typedef enum {
    KHRONOS_FALSE = 0,
    KHRONOS_TRUE  = 1
} khronos_boolean_enum_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KHRONOS_KHRPLATFORM_H_ */