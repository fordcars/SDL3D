@echo off
REM Copyright 2016 Carl Hewett under the GNU GPL
REM See COPYING

title DDS Texture Compressor

setlocal EnableDelayedExpansion

set compressionType=DXT3
set compressorExeDirectory=C:\Program Files (x86)\AMD\AMDCompress\CLI
set outputFileExtension=.dds

set batDirectory=%~dp0
set sourcePath=%~1

if [%sourcePath%]==[] (
	echo You did not specify an input file! Try dragging one on this batch script
	goto quit
)

REM Get the filename (without the extension and the rest of the path)
REM http://stackoverflow.com/questions/9252980/how-to-split-the-filename-from-a-full-path-in-batch
REM http://stackoverflow.com/questions/3215501/batch-remove-file-extension
for %%A in (%sourcePath%) do (
	set sourceFilename=%%~nA
	set sourceFileExtension=%%~xA
)

REM Generate unique filename
REM http://blogs.msdn.com/b/myocom/archive/2005/06/03/creating-unique-filenames-in-a-batch-file.aspx
for /f "delims=:. tokens=1-4" %%t in ("%TIME: =0%") do (
	REM Prepend the bat directory and add the source file's extension to the generated filename
	set tempFilename=TEMP-%%t%%u%%v%%w
	
	REM Here we need delayed expansion since tempFilename is in the same loop
	set tempFilePath=%batDirectory%!tempFilename!%sourceFileExtension%
)

REM Check if the temp file already exists
if exist %tempFilePath% (
	REM If this file already exists (virtually impossible), quit
	echo File %tempFilePath% already exists! Running this batch file again should work.
	goto quit
)

set outputFile=%batDirectory%%sourceFilename%%outputFileExtension%

REM Check if the outputFile already exists
if exist %outputFile% (
	echo Output file '%outputFile%' already exists!
	set /p userInput="Would you like to replace it? (Y/N)"
	
	REM Square brackets [] make sure the strings aren't empty
	if /i [!userInput!]==[Y] (
		echo Ok, continuing...
		echo.
	) else (
		echo Ok, quitting...
		goto quit
	)
)

echo Now compressing '%sourcePath%' using %compressionType%...

REM Create a temporary file to make sure we don't edit the original
copy "%sourcePath%" "%tempFilePath%"

echo.
echo ---- Flipping image

mogrify -flip "%tempFilePath%"

echo.
echo ---- Compressing image to '%outputFile%'
echo.

REM cd to the path since currently (as of 2015) AMDCompressCLI can't be ran from somewhere else
cd "%compressorExeDirectory%"
AMDCompressCLI -fd %compressionType% -CompressionSpeed 0 -mipsize 1 "%tempFilePath%" "%outputFile%"

:clean
del %tempFilePath%

:quit
echo Press any key to quit...
pause >nul
exit