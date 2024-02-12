@echo off
echo Generate /Out directory, containing files for importing MintLibrary

: Assets Directory
robocopy Assets Out/Assets /S

: Natvis Files
robocopy MintContainer/ Out/Natvis *.natvis /xd Out
robocopy MintMath/ Out/Natvis *.natvis /xd Out
robocopy MintPhysics/ Out/Natvis *.natvis /xd Out
robocopy MintPlatform/ Out/Natvis *.natvis /xd Out

: Include Directory
robocopy MintApp/Include Out/Include/MintApp/Include /S
robocopy MintAudio/Include Out/Include/MintAudio/Include /S
robocopy MintCommon/Include Out/Include/MintCommon/Include /S
robocopy MintContainer/Include Out/Include/MintContainer/Include /S
robocopy MintGame/Include Out/Include/MintGame/Include /S
robocopy MintGUI/Include Out/Include/MintGUI/Include /S
robocopy MintLanguage/Include Out/Include/MintLanguage/Include /S
robocopy MintLibrary/Include Out/Include/MintLibrary/Include /S
robocopy MintMath/Include Out/Include/MintMath/Include /S
robocopy MintPhysics/Include Out/Include/MintPhysics/Include /S
robocopy MintPlatform/Include Out/Include/MintPlatform/Include /S
robocopy MintReflection/Include Out/Include/MintReflection/Include /S
robocopy MintRendering/Include Out/Include/MintRendering/Include /S
robocopy MintRenderingBase/Include Out/Include/MintRenderingBase/Include /S

: Lib Directory (DEBUG)
robocopy _output/Debug Out/Lib/Debug MintApp.lib
robocopy _output/Debug Out/Lib/Debug MintAudio.lib
robocopy _output/Debug Out/Lib/Debug MintCommon.lib
robocopy _output/Debug Out/Lib/Debug MintContainer.lib
robocopy _output/Debug Out/Lib/Debug MintGame.lib
robocopy _output/Debug Out/Lib/Debug MintGUI.lib
robocopy _output/Debug Out/Lib/Debug MintLanguage.lib
robocopy _output/Debug Out/Lib/Debug MintLibrary.lib
robocopy _output/Debug Out/Lib/Debug MintMath.lib
robocopy _output/Debug Out/Lib/Debug MintPhysics.lib
robocopy _output/Debug Out/Lib/Debug MintPlatform.lib
robocopy _output/Debug Out/Lib/Debug MintReflection.lib
robocopy _output/Debug Out/Lib/Debug MintRendering.lib
robocopy _output/Debug Out/Lib/Debug MintRenderingBase.lib

: Lib Directory (RELEASE)
robocopy _output/Release Out/Lib/Release MintApp.lib
robocopy _output/Release Out/Lib/Release MintAudio.lib
robocopy _output/Release Out/Lib/Release MintCommon.lib
robocopy _output/Release Out/Lib/Release MintContainer.lib
robocopy _output/Release Out/Lib/Release MintGame.lib
robocopy _output/Release Out/Lib/Release MintGUI.lib
robocopy _output/Release Out/Lib/Release MintLanguage.lib
robocopy _output/Release Out/Lib/Release MintLibrary.lib
robocopy _output/Release Out/Lib/Release MintMath.lib
robocopy _output/Release Out/Lib/Release MintPhysics.lib
robocopy _output/Release Out/Lib/Release MintPlatform.lib
robocopy _output/Release Out/Lib/Release MintReflection.lib
robocopy _output/Release Out/Lib/Release MintRendering.lib
robocopy _output/Release Out/Lib/Release MintRenderingBase.lib

pause
