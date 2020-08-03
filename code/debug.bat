@echo off

if not defined DevEnvDir call .\\code\\vcvars.bat
call devenv .\\build\\win32_nano.exe