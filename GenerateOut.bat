@echo off
echo Generate /Out directory, containing files for importing MintLibrary

: Assets Directory
robocopy Assets Out/Assets /S

: Include Directory
robocopy ./ Out/MintLibrary/Include stdafx.h
robocopy MintCommon/Include Out/MintLibrary/Include/MintCommon/Include /S
robocopy MintContainer/Include Out/MintLibrary/Include/MintContainer/Include /S
robocopy MintLibrary/Include Out/MintLibrary/Include/MintLibrary/Include /S
robocopy MintMath/Include Out/MintLibrary/Include/MintMath/Include /S
robocopy MintPlatform/Include Out/MintLibrary/Include/MintPlatform/Include /S
robocopy MintRendering/Include Out/MintLibrary/Include/MintRendering/Include /S
robocopy MintRenderingBase/Include Out/MintLibrary/Include/MintRenderingBase/Include /S

: Lib Directory (DEBUG)
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintContainer.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintLibrary.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintMath.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintPlatform.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintRendering.lib
robocopy _output/Debug Out/MintLibrary/Lib/Debug MintRenderingBase.lib

: Lib Directory (RELEASE)
robocopy _output/Release Out/MintLibrary/Lib/Release MintContainer.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintLibrary.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintMath.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintPlatform.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintRendering.lib
robocopy _output/Release Out/MintLibrary/Lib/Release MintRenderingBase.lib

pause
