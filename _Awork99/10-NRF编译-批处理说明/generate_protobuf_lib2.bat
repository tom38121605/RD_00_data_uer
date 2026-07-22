REM pause


@ECHO OFF
setlocal EnableDelayedExpansion

REM 1.取得仓库的绝对路径 C:\workspace\ommo_nrf

for /f "tokens=* usebackq" %%F in (`git rev-parse --show-toplevel`) do (
    SET REPO_FOLDER=%%F
)


REM 2.把"/"换成"\"

SET REPO_FOLDER=!REPO_FOLDER:/=\!
echo REPO_FOLDER= %REPO_FOLDER%


REM 3.取得PROTO_FILE的文件名 C:\workspace\ommo_nrf\ommo_build_common\ommo_proto\resources\ommo_fw.proto

SET PROTO_FOLDER=%REPO_FOLDER%\ommo_build_common\ommo_proto\resources
SET PROTO_FILE=%PROTO_FOLDER%\ommo_fw.proto

echo PROTO_FILE= %PROTO_FILE%


REM 4.取得TEMP_FOLDER

SET OUTPUT_FOLDER=%REPO_FOLDER%\nRF5_SDK_17.1.0_ddde560\components\ommo
SET TEMP_FOLDER=%REPO_FOLDER%\flash_files\temp
echo OUTPUT_FOLDER= %OUTPUT_FOLDER%
echo TEMP_FOLDER= %TEMP_FOLDER%


REM 5.NRF_DFU_PROTO_FILE

SET NRF_DFU_FOLDER=%REPO_FOLDER%\nRF5_SDK_17.1.0_ddde560\components\libraries\bootloader\dfu
SET NRF_DFU_PROTO_FILE=%NRF_DFU_FOLDER%\dfu-cc.proto
echo NRF_DFU_PROTO_FILE= %NRF_DFU_PROTO_FILE%



REM =================================================================================
REM Begin the execution of nanopb
REM execute file
REM -D output dir
REM -I path for .options and .proto files
REM .proto file


REM 6.判断PROTO_FILE文件名是否存在

REM make sure the proto file is at the expected path
if not exist %PROTO_FILE% (
	echo "%PROTO_FILE% - file does not exist!"
        pause
	exit /b 1
)

echo  find PROTO_FILE = %PROTO_FILE%
rem pause



REM Use xcopy to determine if the proto file is newer than the generated c protobuf file
REM xcopy lists the number of files it would copy if the .proto is newer than the .pb.c, search for that

REM 因前者proto 没比后者pb.c 新，xcopy输出0，findstr找1没找到，输出errorlevel 1
xcopy /L /D /Y %PROTO_FILE% %OUTPUT_FOLDER%\ommo_fw.pb.c | findstr /B /C:"1 "

echo  111
rem pause

REM 相当于拆解成下面三句
REM  xcopy /L /D /Y %PROTO_FILE% %OUTPUT_FOLDER%\ommo_fw.pb.c > tmp_log.txt
REM  findstr /B /C:"1 " tmp_log.txt
REM del tmp_log.txt

REM 因findstr找1没找到，输出errorlevel 1，不用运行下面重新生成
if not errorlevel 1 (
	%REPO_FOLDER%\nanopb-0.4.6-windows-x86\generator-bin\nanopb_generator.exe -D %OUTPUT_FOLDER% -I %PROTO_FOLDER% %PROTO_FILE%
)

echo  222
rem pause


REM 同上
xcopy /L /D /Y %NRF_DFU_PROTO_FILE% %OUTPUT_FOLDER%\dfu-cc.pb.c | findstr /B /C:"1 "
if not errorlevel 1 (
	%REPO_FOLDER%\nanopb-0.4.6-windows-x86\generator-bin\nanopb_generator.exe -D %OUTPUT_FOLDER% -I %NRF_DFU_FOLDER% %NRF_DFU_PROTO_FILE%
)

echo  333
rem pause


REM =================================================================================
REM Begin the execution of the Git describe script
echo.

rem 判断目录C:\workspace\ommo_nrf\flash_files\temp是否存在

rem create folder if not exist
if not exist %TEMP_FOLDER% (
    mkdir %TEMP_FOLDER%
)

echo  444
rem pause


set output_file=%TEMP_FOLDER%\git_describe.h
set git_describe_header_file=%OUTPUT_FOLDER%\git_describe.h

echo  555
rem pause



rem 这git describe --dirty：生成版本号，形如v7018-201-g01cba3b67-dirty，带 dirty 代表本地有未提交修改
rem 这for /f 把这条命令输出的整行文字抓出来，存进变量 VERSION
rem 这usebackq：允许`反引号包裹里面的 git 命令，执行并捕获输出
rem 这tokens=*：整行完整读取，不截断空格^
rem 这%% F 是循环临时变量，接收 git 输出内容，VERSION里面只会保留最后一行（这里只有一行）


rem Run git describe --dirty command
for /f "tokens=* usebackq" %%F in (`git describe --dirty`) do (
    set "VERSION=%%F"
)

echo  666
rem pause


rem Check if 'dirty' is present in the version string
rem cmd [find] does not work on segger embuild CLI, need to use [findstr]


echo ------ start ------

rem 找到VERSION， v7018-201-g01cba3b67-dirty
echo VERSION = !VERSION! 

echo !VERSION! | findstr "dirty" > nul

if errorlevel 1 (
    rem 'dirty' not found, no need to replace
) else (
    rem Replace 'dirty' with date in yymmdd format
    rem this line does not work for some reason
    rem set MY_DATE=%DATE:~-2%%DATE:~4,2%%DATE:~7,2%
    set VERSION=!VERSION:dirty=%DATE:~-2%%DATE:~4,2%%DATE:~7,2%!
)

rem 然后VERSION，v7018-201-g01cba3b67-周三/0/2
echo VERSION = !VERSION! 

echo ------ end ------



rem Save the version to a header file
echo #ifndef GIT_DESCRIBE_H > %output_file%
echo #define GIT_DESCRIBE_H >> %output_file%
echo. >> %output_file%
echo   #define GIT_DESCRIBE_VER %VERSION:~1,4% >> %output_file%
echo   #define GIT_DESCRIBE_STR "%VERSION%" >> %output_file%
echo. >> %output_file%
echo #endif >> %output_file%
rem echo. >> %output_file%

rem check describe string, if different update the header file
fc "%output_file%" "%git_describe_header_file%" > nul

if errorlevel 1 (
    echo New git describe info. Updating header file...
    copy /y "%output_file%" "%git_describe_header_file%"
    echo File replaced.
) else (
    echo No need to update git describe info.
)

rem delete the temp file
del %output_file%


endlocal

echo successful
pause



