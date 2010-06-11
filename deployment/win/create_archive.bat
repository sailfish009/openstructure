@ECHO OFF
REM create a zip archive for deployment on windows
REM no regisrty entries
REM no PATHs will be changed except in the DNG session
set inst_dir=openstructure1.0.0a1_2381
set archive_name=OpenStructureWin32
if exist "%inst_dir%" rmdir /S /Q "%inst_dir%" 
if exist %archive_name%.zip del %archive_name%.zip*.*
REM dir /S %inst_dir%
mkdir "%inst_dir%\share\openstructure\examples"
mkdir "%inst_dir%\lib\site-packages"
mkdir "%inst_dir%\bin"
xcopy  /E /I ..\..\examples "%inst_dir%\share\openstructure\examples"
xcopy  /E /I ..\..\stage\share "%inst_dir%\share\"
REM xcopy  /E /I ..\..\website\html "%inst_dir%\share\doc"
copy  d:\programs\bin\python.exe "%inst_dir%\bin"

xcopy  /E /I /EXCLUDE:excludes d:\programs\lib "%inst_dir%\lib"
xcopy  /E /I ..\..\stage\lib "%inst_dir%\lib"
xcopy /E /I /EXCLUDE:excludes ..\..\stage\bin "%inst_dir%\bin"
set PATH="c:\Program Files (x86)"\7-Zip\;%PATH%
REM 7z a -r -t7z %archive_name% "%inst_dir%" > log
REM if exist %archive_name%.7z copy /b 7zS.sfx + config.txt + %archive_name%.7z %archive_name%.exe
7z a -r -tzip %archive_name% "%inst_dir%" > log
