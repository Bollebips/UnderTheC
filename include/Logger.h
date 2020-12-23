#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdbool.h>

void _LogAssert(const char* file, const int line, const bool check, const char* message, ...);
void _LogError(const char* file, const int line, const char* message, ...);
void _LogWarning(const char* file, const int line, const char* message, ...);
void _LogInfo(const char* file, const int line, const char* message, ...);

#ifdef DEBUG
#define LogAssert(b, ...) _LogAssert(__FILE__, __LINE__, b, __VA_ARGS__)
#define LogError(b, ...) _LogError(__FILE__, __LINE__, b, __VA_ARGS__)
#define LogWarning(b, ...) _LogWarning(__FILE__, __LINE__, b, __VA_ARGS__)
#define LogInfo(b, ...) _LogInfo(__FILE__, __LINE__, b, __VA_ARGS__)
#else
#define LogAssert(b, ...)
#define LogError(b, ...)
#define LogWarning(b, ...)
#define LogInfo(b, ...)
#endif

#endif