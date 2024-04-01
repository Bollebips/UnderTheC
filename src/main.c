#include <stdio.h>

#include <Containers/Array.h>
#include <Logger.h>
int main()
{
    printf("Hello world!");

    Array array;

    ArrayInit(&array, 16, 64);

    for(int i = 0; i < 100000; ++i)
    {
        printf("%d\n", i);
    }

    return 0;
}