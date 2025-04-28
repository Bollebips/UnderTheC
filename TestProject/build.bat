@ECHO OFF
REM Build script for engine
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET compiler=clang
SET assembly=UnderTheC_TestProject
SET engine=UnderTheC
SET compilerFlags=-g -Wvarargs -Wall
REM -Wall -Werror
REM THIS GLFW include SHOULD BE moved to engine I think. Also, glfw3.dll should be copied to bin folder.
SET includeFlags=-Isrc -I../Engine/src -I../3rdParty/glfw-3.4/include -I../3rdParty/glew-2.1.0/include
SET linkerFlags=-L../bin/ -l%engine%.lib -L../3rdParty/glfw-3.4/lib-static-ucrt/ -L../3rdParty/glew-2.1.0/lib/Release/x64 -lglfw3dll -lglew32s -lopengl32
SET defines=-DDEBUG -DUTCeIMPORT

ECHO "Building %assembly%%..."
%compiler% %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%