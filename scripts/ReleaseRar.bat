@ECHO OFF

REM Script to compress shit into Rar to be uploaded and released.
REM It is inteded to be executed from the Project Root Dir

ECHO "Compressing bin folder..."

IF NOT EXIST "%ProgramFiles%\WinRAR" (
    ECHO "Winrar Not Found"
    PAUSE
    EXIT
)

XCOPY /S /I bin "MHW Costume Armor"
del "MHW Costume Armor\*.log" 2>nul
del "MHW Costume Armor\*.a" 2>nul
del "MHW Costume Armor\SavedSets.json" 2>nul
del "MHW Costume Armor\Settings.json" 2>nul
rmdir /s /q "MHW Costume Armor\Backups" 2>nul

del "MHW Costume Armor.rar" 2>nul
"%ProgramFiles%\WinRAR\Rar.exe" a -ep1 -idq -r -y "MHW Costume Armor.rar" "MHW Costume Armor"

rmdir /S /Q "MHW Costume Armor"

PAUSE