@ECHO OFF
REM Build Everything

ECHO "Building everything..."


PUSHD Engine
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL%)

PUSHD TestProject
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL%)

ECHO "All assemblies built successfully."