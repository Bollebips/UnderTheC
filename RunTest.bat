@ECHO OFF
REM Build script for engine
SetLocal EnableDelayedExpansion

SET exe=UnderTheC_TestProject
ECHO "Running %exe%%.exe..."

CALL "bin/%exe%.exe"