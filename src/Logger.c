#include "Logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static void PrintHeader();
static void PrintErrorTag();
static void PrintWarningTag();
static void PrintInfoTag();
static void PrintBody(const char* output, ...);

/**
 * @brief Print an error to the console.
 * @param output The error to be printed.
 */
void LogError(const char* output, ...)
{
    PrintHeader();
    PrintErrorTag();
    PrintBody(output);
}

/**
 * @brief Print a warning to the console.
 * @param output The warning to be printed.
 */
void LogWarning(const char* output, ...)
{
    PrintHeader();
    PrintWarningTag();
    PrintBody(output);
}

/**
 * @brief Print info to the console.
 * @param output The info to be printed.
 */
void LogInfo(const char* output, ...)
{
    PrintHeader();
    PrintInfoTag();
    PrintBody(output);
}

/* ----------------------------------------------------- STATICS ---------------------------------------------------- */

/**
 * @brief Print the header of any logging. This contains the current timestamp.
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

/**
 * @brief Print the body, the actual contents of the log.
 * @param output The log to be printed.
 */
static void PrintBody(const char* output, ...)
{
    va_list argp;
    va_start(argp, output);
    vprintf(output, argp);
    va_end(argp);

    printf("\n");
}