#ifndef DEFINES_H
#define DEFINES_H

// Unsigned int types.
#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned int uint;

// Signed int types.
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Floating point types
typedef float f32;
typedef double f64;

#ifdef VOXC_EXPORT
    // Exports
    #ifdef _MSC_VER
        #define VoxC_API __declspec(dllexport)
    #else
        #define VoxC_API __attribute__((visibility("default")))
    #endif
#else
    // Imports
    #ifdef _MSC_VER
        #define VoxC_API __declspec(dllimport)
    #else
        #define VoxC_API
    #endif
#endif

#endif // DEFINES_H
