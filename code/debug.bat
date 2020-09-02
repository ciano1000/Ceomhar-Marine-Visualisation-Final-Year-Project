@echo off
if not defined DevEnvDir call .\\code\\vcvars.bat
devenv .\\build\\Ceomhar.exe