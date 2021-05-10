@echo off

call ctime -begin ceomhar.time
set application_name=Ceomhar
set build_options= -DCEOMHAR_SLOW=1 -DCEOMHAR_INTERNAL=1  -DCEOMHAR_WIN32=1 
set compile_flags=-nologo -FC -Gm- -MT -GR- -EHa- -Oi -Zi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4996 
set common_link_flags= opengl32.lib -opt:ref -incremental:no 
set platform_link_flags= user32.lib gdi32.lib -subsystem:windows -ENTRY:mainCRTStartup %common_link_flags%

if not defined DevEnvDir call vcvarsall x64
if not exist .\build mkdir .\build 
pushd .\build
echo **	Building App	**
cl -DCEOMHAR_APP=1 %build_options% ..\\code\\app_ceomhar.cpp  %compile_flags% /LD /link /DLL  /out:%application_name%_app.dll
echo **	Building Platform	**
cl %build_options% ..\\code\\win32_ceomhar.cpp  %compile_flags% /link %platform_link_flags%  /out:%application_name%.exe
echo **	Building Datagen	**
cl -DCEOMHAR_DATAGEN=1 ..\\code\\debug_serial_data_gen\\win32_datagen.cpp  %compile_flags% /link user32.lib /out:debug_datagen.exe
echo **	Building Server	**
cl -DCEOMHAR_SERVER=1 ..\\code\\server\\win32_ceomhar_server.cpp  %compile_flags% /link user32.lib ws2_32.lib /out:ceomhar_server.exe
popd
call ctime -end ceomhar.time