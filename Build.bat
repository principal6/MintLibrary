@echo off

echo.
(echo ^>^> BUILDING MINT LIBRARY...)
echo.


set COMMON_COMP_OPTS="/I. /IExternals\freetype_2_10_4\include\ /IExternals\stb_image\ /std:c++17 /EHsc /D_UNICODE /DUNICODE /D_CONSOLE"
set COMMON_COMP_OPTS=%COMMON_COMP_OPTS:"=%
:### SELECT BUILD MODE
set /a DEBUG_MODE=0
if %DEBUG_MODE%==1 (
	(echo ^>^>^> BUILDING DEBUG MODE^!^!)
	echo.
	set cl=%COMMON_COMP_OPTS% /Ox /Ob0 /Zi /MDd /D_DEBUG /DDEBUG
	set MODED_LIBS="msvcrtd.lib freetyped.lib"
) else (
	(echo ^>^>^> BUILDING RELEASE MODE^!^!)
	echo.
	(set cl=%COMMON_COMP_OPTS% /O2)
	set MODED_LIBS="msvcrt.lib freetype.lib"
)
set MODED_LIBS=%MODED_LIBS:"=%


:### PREPARE DIRECTORIES
IF NOT EXIST _intermediate mkdir _intermediate
IF NOT EXIST _intermediate\MintAudio mkdir _intermediate\MintAudio
IF NOT EXIST _intermediate\MintCommon mkdir _intermediate\MintCommon
IF NOT EXIST _intermediate\MintContainer mkdir _intermediate\MintContainer
IF NOT EXIST _intermediate\MintGame mkdir _intermediate\MintGame
IF NOT EXIST _intermediate\MintLanguage mkdir _intermediate\MintLanguage
IF NOT EXIST _intermediate\MintLibrary mkdir _intermediate\MintLibrary
IF NOT EXIST _intermediate\MintMath mkdir _intermediate\MintMath
IF NOT EXIST _intermediate\MintPhysics mkdir _intermediate\MintPhysics
IF NOT EXIST _intermediate\MintPlatform mkdir _intermediate\MintPlatform
IF NOT EXIST _intermediate\MintReflection mkdir _intermediate\MintReflection
IF NOT EXIST _intermediate\MintRendering mkdir _intermediate\MintRendering
IF NOT EXIST _intermediate\MintRenderingBase mkdir _intermediate\MintRenderingBase
IF NOT EXIST _output mkdir _output


:### COMPILE
cl MintCommon\Source\CommonDefinitions.cpp /c /Fo_intermediate\MintCommon\ /Fd_output\MintCommon.pdb
cl MintContainer\Source\Test.cpp /c /Fo_intermediate\MintContainer\ /Fd_output\MintContainer.pdb
cl MintMath\Source\_UnityBuild.cpp /c /Fo_intermediate\MintMath\ /Fd_output\MintMath.pdb
cl MintPlatform\Source\_UnityBuild.cpp /c /Fo_intermediate\MintPlatform\ /Fd_output\MintPlatform.pdb
cl MintReflection\Source\_UnityBuild.cpp /c /Fo_intermediate\MintReflection\ /Fd_output\MintReflection.pdb
cl MintLanguage\Source\_UnityBuild.cpp /c /Fo_intermediate\MintLanguage\ /Fd_output\MintLanguage.pdb
cl MintRenderingBase\Source\_UnityBuild.cpp /c /Fo_intermediate\MintRenderingBase\ /Fd_output\MintRenderingBase.pdb
cl MintRendering\Source\_UnityBuild.cpp /c /Fo_intermediate\MintRendering\ /Fd_output\MintRendering.pdb
cl MintAudio\Source\_UnityBuild.cpp /c /Fo_intermediate\MintAudio\ /Fd_output\MintAudio.pdb
cl MintPhysics\Source\_UnityBuild.cpp /c /Fo_intermediate\MintPhysics\ /Fd_output\MintPhysics.pdb
cl MintGame\Source\_UnityBuild.cpp /c /Fo_intermediate\MintGame\ /Fd_output\MintGame.pdb


:### LINK
lib _intermediate\MintCommon\Logger.obj /OUT:_output\MintCommon.lib
lib _intermediate\MintContainer\UniqueString.obj /OUT:_output\MintContainer.lib
lib _intermediate\MintMath\_UnityBuild.obj /OUT:_output\MintMath.lib
lib _intermediate\MintPlatform\_UnityBuild.obj /OUT:_output\MintPlatform.lib
lib _intermediate\MintReflection\_UnityBuild.obj /OUT:_output\MintReflection.lib
lib _intermediate\MintLanguage\_UnityBuild.obj /OUT:_output\MintLanguage.lib
lib _intermediate\MintRenderingBase\_UnityBuild.obj /OUT:_output\MintRenderingBase.lib /LIBPATH:Externals\freetype_2_10_4\lib\ %MODED_LIBS%
lib _intermediate\MintRendering\_UnityBuild.obj /OUT:_output\MintRendering.lib
lib _intermediate\MintAudio\_UnityBuild.obj /OUT:_output\MintAudio.lib
lib _intermediate\MintPhysics\_UnityBuild.obj /OUT:_output\MintPhysics.lib
lib _intermediate\MintGame\_UnityBuild.obj /OUT:_output\MintGame.lib


:### COMPILE AND LINK <MintLibrary>
cl MintLibrary\Source\_UnityBuild.cpp -c /Fo_intermediate\MintLibrary\ /Fd_output
lib _intermediate\MintLibrary\_UnityBuild.obj /OUT:_output\MintLibrary.lib _output\MintCommon.lib _output\MintContainer.lib _output\MintMath.lib _output\MintPlatform.lib _output\MintReflection.lib _output\MintLanguage.lib _output\MintRenderingBase.lib _output\MintRendering.lib _output\MintAudio.lib _output\MintPhysics.lib _output\MintGame.lib


:### COMPILE THE FINAL EXECUTABLE!
cl MintLibraryTest\test.cpp /Fo_output\MintLibraryTest.obj /link /OUT:_output\MintLibraryTest.exe /LIBPATH:_output\ /SUBSYSTEM:CONSOLE kernel32.lib user32.lib gdi32.lib


echo.
(echo ^>^> MINT LIBRARY BUILD COMPLETED^!)
echo.

pause
