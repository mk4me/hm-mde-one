SetLocal

if "%1"==""           goto USAGE
if "%2"==""           goto USAGE

set Configuration=%1
set Platform=%2

set BuildPath=%~dp0
set OSGPath=%BuildPath%..\..\..\OpenSceneGraph
rem Nalezy przygotowaæ wersje tego przygotowana to tworzenia roznych wersji buildow
rem i przyda³oby sie to zrobic w miare szybszko bo sie zrobi burdel
rem set DebugBinPath=%OSGPath%\bin\Debug
set OSGBinPath=%OSGPath%\bin
set OutputBinPath=%BuildPath%..\..\..\..\bin\%Platform%\%Configuration%
set QtBinDir=%QTDIR%\bin

if not exist "%OutputBinPath%" md "%OutputBinPath%"

@rem Kopiujemy  
xcopy "%OSGBinPath%\*.dll" "%OutputBinPath%" /S /Y /D /I /R /EXCLUDE:PromoteExcludes.txt
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