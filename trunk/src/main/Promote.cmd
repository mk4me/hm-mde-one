SetLocal

if "%1"==""           goto USAGE
if "%2"==""           goto USAGE

set Configuration=%1
set Platform=%2

set BuildPath=%~dp0
set OSGPath=%BuildPath%..\..\binaries\%Platform%\osg\%Configuration%
rem Nalezy przygotowaæ wersje tego przygotowana to tworzenia roznych wersji buildow
rem i przyda³oby sie to zrobic w miare szybszko bo sie zrobi burdel
rem set DebugBinPath=%OSGPath%\bin\Debug
rem set OSGBinPath=%OSGPath%\bin   - wczesniej bylo tak- bo by³ podzia³ na bin i share
set OSGBinPath=%OSGPath%
set OutputBinPath=%BuildPath%..\..\bin\%Platform%\%Configuration%
set QtBinDir=%BuildPath%..\..\binaries\%Platform%\QT\%Configuration%

if not exist "%OutputBinPath%" md "%OutputBinPath%"

@rem Kopiujemy  
xcopy "%OSGBinPath%\*.dll" "%OutputBinPath%" /S /Y /D /I /R /EXCLUDE:PromoteExcludes.txt
@rem xcopy "%OSGBinPath%\*.dll" "%BuildPath%\%Configuration%" /S /Y /D /I /R /EXCLUDE:PromoteExcludes.txt
@rem Kompiujemy biblioteki Qt
if %Configuration% == Debug (
  xcopy "%QtBinDir%\QtCored4.dll" "%OutputBinPath%"
  xcopy "%QtBinDir%\QtGuid4.dll" "%OutputBinPath%"
  xcopy "%QtBinDir%\QtOpenGLd4.dll" "%OutputBinPath%"
  
) else (
  xcopy "%QtBinDir%\QtCore4.dll" "%OutputBinPath%"
  xcopy "%QtBinDir%\QtGui4.dll" "%OutputBinPath%"
  xcopy "%QtBinDir%\QtOpenGL4.dll" "%OutputBinPath%"
)

goto :EOF

:USAGE
@echo .
@echo usage: Promote "Configuration" "Platform" 
@echo .

exit 1