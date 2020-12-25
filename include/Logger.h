#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdbool.h>

void _LogAssert(const char* file, const int line, const char* expression, const char* expressionString, ...);
void _LogError(const char* file, const int line, const char* message, ...);
void _LogWarning(const char* file, const int line, const char* message, ...);
void _LogInfo(const char* file, const int line, const char* message, ...);

#ifdef DEBUG
#define LogAssert(expression, ...) _LogAssert(__FILE__, __LINE__, (const char*) expression, #expression, ## __VA_ARGS__, NULL)
#define LogError(message, ...) _LogError(__FILE__, __LINE__, message,  __VA_ARGS__)
#define LogWarning(message, ...) _LogWarning(__FILE__, __LINE__, message, __VA_ARGS__)
#define LogInfo(message, ...) _LogInfo(__FILE__, __LINE__, message, __VA_ARGS__)
#else
#define LogAssert(expression, ...)
#define LogError(message, ...)
#define LogWarning(message, ...)
#define LogInfo(message, ...)
#endif

#endif