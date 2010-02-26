@ECHO OFF
REM create a zip archive for deployment on windows
REM no regisrty entries
REM no PATHs will be changed except in the DNG session
if exist openstructure rmdir /S /Q openstructure 
if exist OpenStructure_WIN32.zip del OpenStructure_WIN32.zip
REM dir /S openstructure
mkdir openstructure\share\openstructure\examples
mkdir openstructure\lib\site-packages
mkdir openstructure\bin
xcopy  /E /I ..\..\examples openstructure\share\openstructure\examples
xcopy  /E /I ..\..\stage\share openstructure\share\

xcopy  /E /I d:\programs\bin openstructure\bin
REM copy  d:\programs\lib\boost*iostreams*.* d:\programs\lib\boost*unit_test_frame*.* d:\programs\lib\boost*python*.* d:\programs\lib\boost*math*.* d:\programs\lib\boost*system*.* d:\programs\lib\boost*program_options*.* d:\programs\lib\boost*regex*.* d:\programs\lib\_*.* d:\programs\lib\*.py d:\programs\lib\*png*13*.* d:\programs\lib\*zlib*.* d:\programs\lib\*tif*.* d:\programs\lib\py*.* d:\programs\lib\*select*.*  d:\programs\lib\*unicode*.* openstructure\lib

xcopy  /E /I /EXCLUDE:excludes d:\programs\lib openstructure\lib
xcopy  /E /I ..\..\stage\lib openstructure\lib
xcopy /E /I ..\..\stage\bin openstructure\bin
set PATH="c:\Program Files (x86)"\7-Zip\;%PATH%
7z a -r -tzip OpenStructure_WIN32.zip openstructure > log