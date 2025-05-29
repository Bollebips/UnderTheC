#include "FileIO.h"
#include "Logger.h"

#include <stdio.h>

const char* GetStringFromFile(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");

    if(file == NULL)
    {
        LogError("Unable to open file at path %s", filePath);
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    long length = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* result = (char*)malloc(length + 1);
    if(result == NULL)
    {
        LogError("Memory allocation failed for file %s", filePath);
        return NULL;
    }

    fread(result, sizeof(char), length, file);
    result[length] = '\0';
    fclose(file);

    return result;
}
