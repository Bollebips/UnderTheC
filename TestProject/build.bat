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
SET compilerFlags=-g -Wvarargs -Wall -Werror
REM -Wall -Werror
SET includeFlags=-Isrc -I../Engine/src/
SET linkerFlags=-L../bin/ -l%engine%.lib
SET defines=-DDEBUG -DKIMPORT

ECHO "Building %assembly%%..."
%compiler% %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%