@echo off
setlocal
cd /d "%~dp0"
set "action=%~1"
if "%action%"=="" set "action=build"
if not defined CMAKE set "CMAKE=cmake"
if not defined JOBS set "JOBS=4"
if "%action%"=="clean" goto clean
if "%action%"=="fmt" goto fmt
if "%action%"=="build-release" goto release
if "%action%"=="build" goto debug
if "%action%"=="test" goto debug
echo Unknown operation. Use build, build-release, test, fmt, or clean.
exit /b 2
:debug
set "config=Debug"
set "directory=build/debug"
goto build
:release
set "config=Release"
set "directory=build/release"
:build
"%CMAKE%" -S . -B "%directory%" -DCMAKE_BUILD_TYPE=%config% %CMAKE_FLAGS%
if errorlevel 1 exit /b 1
"%CMAKE%" --build "%directory%" --config %config% --parallel %JOBS%
if errorlevel 1 exit /b 1
"%CMAKE%" -DROOT=. -DBUILD_DIR="%directory%" -P cmake/CompileCommands.cmake
if errorlevel 1 exit /b 1
if not "%action%"=="test" exit /b 0
ctest --test-dir "%directory%" -C %config% --output-on-failure --timeout 30
exit /b %errorlevel%
:clean
"%CMAKE%" -DROOT=. -P cmake/Clean.cmake
exit /b %errorlevel%
:fmt
"%CMAKE%" -DROOT=. -P cmake/Format.cmake
exit /b %errorlevel%
