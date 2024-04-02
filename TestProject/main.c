#include <stdio.h>

#include <Containers/Array.h>
#include <Logger.h>

#include <GLFW/glfw3.h>

int main(int argc, char** argv)
{
    // glfwSetErrorCallback(NULL);
    if(glfwInit() == false)
    {
        LogError("WOOPS");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

    while (!glfwWindowShouldClose(window))
    {
        // render(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Array array;
    //
    // ArrayInit(&array, 16, 64);

    for(int i = 0; i < 10000; ++i)
    {
        printf("TEST   %d\n", i);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}