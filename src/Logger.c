#include "Logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static void PrintHeader();
static void PrintAssertTag();
static void PrintErrorTag();
static void PrintWarningTag();
static void PrintInfoTag();

/**
 * @brief Print an error to the console if the assertion failed.
 * @param file The file where the assert was called from.
 * @param line The line where the assert was called from.
 * @param check The opperation to be asserted.
 * @param message The error to be printed in case of a failed assert.
 */
void _LogAssert(const char* file, const int line, const bool check, const char* message, ...)
{
    if(!check)
    {
        PrintHeader();
        PrintAssertTag();

        printf("%s:%d | ", file, line);

        va_list argp;
        va_start(argp, message);
        vprintf(message, argp);
        va_end(argp);

        printf("\n");

        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Print an error to the console.
 * @param file The file where the error was called from.
 * @param line The line where the error was called from.
 * @param message The error to be printed.
 */
void _LogError(const char* file, const int line, const char* message, ...)
{
    PrintHeader();
    PrintErrorTag();

    printf("%s:%d | ", file, line);

    va_list argp;
    va_start(argp, message);
    vprintf(message, argp);
    va_end(argp);
    printf("\n");

    exit(EXIT_FAILURE);
}

/**
 * @brief Print a warning to the console.
 * @param file The file where the warning was called from.
 * @param line The line where the warning was called from.
 * @param message The warning to be printed.
 */
void _LogWarning(const char* file, const int line, const char* message, ...)
{
    PrintHeader();
    PrintWarningTag();
    va_list argp;
    va_start(argp, message);
    vprintf(message, argp);
    va_end(argp);
    printf("\n");
}

/**
 * @brief Print info to the console.
 * @param file The file where the info was called from.
 * @param line The line where the info was called from.
 * @param message The info to be printed.
 */
void _LogInfo(const char* file, const int line, const char* message, ...)
{
    PrintHeader();
    PrintInfoTag();
    va_list argp;
    va_start(argp, message);
    vprintf(message, argp);
    va_end(argp);
    printf("\n");
}

/* ----------------------------------------------------- STATICS ---------------------------------------------------- */

/**
 * @brief Print the header of any message. This contains the current timestamp.
 */
static void PrintHeader()
{
    char timeBuffer[32];
    time_t t = time(NULL);
    struct tm tm = { 0 };
    localtime_s(&tm, &t);

    strftime(timeBuffer, 32, "%d-%m-%y %H:%M:%S ", &tm);

    vprintf(timeBuffer, NULL);
}

/**
 * @brief Print an assert tag.
 */
static void PrintAssertTag()
{
    printf("\033[1;31m");
    printf("[ASSERT] ");
    printf("\033[0m");
}

/**
 * @brief Print an error tag.
 */
static void PrintErrorTag()
{
    printf("\033[1;31m");
    printf("[ERROR] ");
    printf("\033[0m");
}

/**
 * @brief Print a warning tag.
 */
static void PrintWarningTag()
{
    printf("\033[1;33m");
    printf("[WARNING] ");
    printf("\033[0m");
}

/**
 * @brief Print an info tag.
 */
static void PrintInfoTag()
{
    printf("\033[0m");
    printf("[INFO] ");
}