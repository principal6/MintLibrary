@echo off
echo Generate /Out directory, containing files for importing MintLibrary

: Assets Directory
robocopy Assets Out/MintLibrary/Include/Assets /S

: Include Directory
robocopy ./ Out/MintLibrary/Include stdafx.h
robocopy MintAudio/Include Out/MintLibrary/Include/MintAudio/Include /S
robocopy MintCommon/Include Out/MintLibrary/Include/MintCommon/Include /S
robocopy MintContainer/Include Out/MintLibrary/Include/MintContainer/Include /S
robocopy MintGame/Include Out/MintLibrary/Include/MintGame/Include /S
robocopy MintLanguage/Include Out/MintLibrary/Include/MintLanguage/Include /S
robocopy MintLibrary/Include Out/MintLibrary/Include/MintLibrary/Include /S
robocopy MintMath/Include Out/MintLibrary/Include/MintMath/Include /S
robocopy MintPhysics/Include Out/MintLibrary/Include/MintPhysics/Include /S
robocopy MintPlatform/Include Out/MintLibrary/Include/MintPlatform/Include /S
robocopy MintReflection/Include Out/MintLibrary/Include/MintReflection/Include /S
robocopy MintRendering/Include Out/MintLibrary/Include/MintRendering/Include /S
robocopy MintRenderingBase/Include Out/MintLibrary/Include/MintRenderingBase/Include /S

: Lib Directory (DEBUG)
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintAudio.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintCommon.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintContainer.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintGame.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintLanguage.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintLibrary.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintMath.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintPhysics.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintPlatform.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintReflection.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintRendering.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintRenderingBase.lib

: Lib Directory (RELEASE)
robocopy _output/Release Out/MintLibrary/Lib/Release MintAudio.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintCommon.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintContainer.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintGame.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintLanguage.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintLibrary.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintMath.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintPhysics.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintPlatform.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintReflection.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintRendering.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintRenderingBase.lib

pause
