#include "Logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static void PrintHeader();
static void PrintErrorTag();
static void PrintWarningTag();
static void PrintInfoTag();
static void PrintBody(const char* output, ...);

void LogError(const char* output, ...)
{
    PrintHeader();
    PrintErrorTag();
    PrintBody(output);
}

void LogWarning(const char* output, ...)
{
    PrintHeader();
    PrintWarningTag();
    PrintBody(output);
}

void LogInfo(const char* output, ...)
{
    PrintHeader();
    PrintInfoTag();
    PrintBody(output);
}

/* ----------------------------------------------------- STATICS ---------------------------------------------------- */

static void PrintHeader()
{
    char timeBuffer[32];
    time_t t = time(NULL);
    struct tm tm = { 0 };
    localtime_s(&tm, &t);

    strftime(timeBuffer, 32, "%d-%m-%y %H:%M:%S ", &tm);

    vprintf(timeBuffer, NULL);
}

static void PrintErrorTag()
{
    printf("\033[1;31m");
    printf("[ERROR] ");
    printf("\033[0m");
}

static void PrintWarningTag()
{
    printf("\033[1;33m");
    printf("[WARNING] ");
    printf("\033[0m");
}

static void PrintInfoTag()
{
    printf("\033[0m");
    printf("[INFO] ");
}

static void PrintBody(const char* output, ...)
{
    va_list argp;
    va_start(argp, output);
    vprintf(output, argp);
    va_end(argp);

    printf("\n");
}