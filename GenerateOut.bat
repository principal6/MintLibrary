@echo off
echo Generate /Out directory, containing files for importing FsLibrary

: Assets Directory
robocopy Assets Out/Assets /S

: Include Directory
robocopy ./ Out/FsLibrary/Include stdafx.h
robocopy FsCommon/Include Out/FsLibrary/Include/FsCommon/Include /S
robocopy FsContainer/Include Out/FsLibrary/Include/FsContainer/Include /S
robocopy FsLibrary/Include Out/FsLibrary/Include/FsLibrary/Include /S
robocopy FsMath/Include Out/FsLibrary/Include/FsMath/Include /S
robocopy FsPlatform/Include Out/FsLibrary/Include/FsPlatform/Include /S
robocopy FsRendering/Include Out/FsLibrary/Include/FsRendering/Include /S
robocopy FsRenderingBase/Include Out/FsLibrary/Include/FsRenderingBase/Include /S

: Lib Directory (DEBUG)
robocopy _output/Debug Out/FsLibrary/Lib/Debug FsContainer.lib
robocopy _output/Debug Out/FsLibrary/Lib/Debug FsLibrary.lib
robocopy _output/Debug Out/FsLibrary/Lib/Debug FsMath.lib
robocopy _output/Debug Out/FsLibrary/Lib/Debug FsPlatform.lib
robocopy _output/Debug Out/FsLibrary/Lib/Debug FsRendering.lib
robocopy _output/Debug Out/FsLibrary/Lib/Debug FsRenderingBase.lib

: Lib Directory (RELEASE)
robocopy _output/Release Out/FsLibrary/Lib/Release FsContainer.lib
robocopy _output/Release Out/FsLibrary/Lib/Release FsLibrary.lib
robocopy _output/Release Out/FsLibrary/Lib/Release FsMath.lib
robocopy _output/Release Out/FsLibrary/Lib/Release FsPlatform.lib
robocopy _output/Release Out/FsLibrary/Lib/Release FsRendering.lib
robocopy _output/Release Out/FsLibrary/Lib/Release FsRenderingBase.lib

pause
