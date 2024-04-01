#include <stdio.h>

#include <Containers/Array.h>
#include <Logger.h>
int main()
{
    Array array;

    ArrayInit(&array, 16, 64);


    for(int i = 0; i < 10; ++i)
    {
        LogAssert(i < 9, "TESTERY TESTEROOO\n");
        printf("TEST   %d\n", i);
    }

    return 0;
}