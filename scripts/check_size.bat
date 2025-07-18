@echo off
setlocal EnableDelayedExpansion

set "BINARY=%1"
set "MAX_SIZE=89088"

for %%A in (%BINARY%) do set SIZE=%%~zA

echo Binary size: %SIZE% bytes

set /a OVERFLOW=%SIZE% - %MAX_SIZE%

if %SIZE% GTR %MAX_SIZE% (
    echo ERROR: Binary size exceeds limit by !OVERFLOW! bytes [max allowed: %MAX_SIZE%]
    exit /b 1
)

endlocal
exit /b 0
