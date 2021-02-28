@echo off


call ctime -begin ceomhar.time
set application_name=Ceomhar
set build_options= -DCEOMHAR_SLOW=1 -DCEOMHAR_internal=1  -DCEOMHAR_WIN32=1   
set compile_flags=-nologo -FC -Gm- -MT -GR- -EHa- -Oi -Zi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4996 
set common_link_flags= opengl32.lib -opt:ref -incremental:no 
set platform_link_flags= user32.lib Shcore.lib gdi32.lib -subsystem:windows -ENTRY:mainCRTStartup %common_link_flags%

if not defined DevEnvDir call vcvarsall x64
if not exist .\build mkdir .\build 
pushd .\build
cl %build_options% ..\\code\\app_ceomhar.cpp  %compile_flags% /link /DLL  /out:%application_name%_app.dll
cl %build_options% ..\\code\\win32_ceomhar.cpp  %compile_flags% /link %platform_link_flags%  /out:%application_name%.exe
popd
call ctime -end ceomhar.time