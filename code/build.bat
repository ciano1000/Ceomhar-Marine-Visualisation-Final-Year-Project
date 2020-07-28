@echo off

set application_name=NanoTests
rem these will need to be set later
rem set build_options= -DHANDMADE_SLOW=1 -DHANDMADE_INTERNAL=1  -DHANDMADE_WIN32=1
set compile_flags=-nologo -FC -Gm- -MT -GR- -EHa- -Oi -Zi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4996 
set common_link_flags= opengl32.lib -opt:ref -incremental:no 
set platform_link_flags= user32.lib gdi32.lib -subsystem:windows -ENTRY:mainCRTStartup %common_link_flags%

if not defined DevEnvDir call .\\code\\vcvars.bat
if not exist .\build mkdir .\build 
pushd .\build
cl %build_options% ..\\code\\win32_nano.cpp %compile_flags% /link %platform_link_flags%
popd