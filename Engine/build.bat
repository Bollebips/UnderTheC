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
SET assembly=UnderTheC
SET compilerFlags=-g -shared -Wvarargs -Wall -Werror
REM -Wall -Werror
SET includeFlags=-Isrc -I../3rdParty/glfw-3.4/include -I../3rdParty/glew-2.1.0/include
SET linkerFlags=-luser32 -L../3rdParty/glew-2.1.0/lib/Release/x64 -lglew32s
SET defines=-DDEBUG -DUTCeEXPORT -D_CRT_SECURE_NO_WARNINGS

ECHO "Building %assembly%%..."
@REM %compiler% %cFilenames% %compilerFlags% -o bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%
%compiler% %cFilenames% %compilerFlags% -o ../bin/%assembly%.dll %defines% %includeFlags% %linkerFlags%