@echo off
echo Generate /Upstream directory, containing files for importing MintLibrary

: Project Directories
robocopy ./ Upstream/MintLibrary stdafx.h
robocopy MintApp Upstream/MintLibrary/MintApp /S
robocopy MintAudio Upstream/MintLibrary/MintAudio /S
robocopy MintCommon Upstream/MintLibrary/MintCommon /S
robocopy MintContainer Upstream/MintLibrary/MintContainer /S
robocopy MintECS Upstream/MintLibrary/MintECS /S
robocopy MintGame Upstream/MintLibrary/MintGame /S
robocopy MintGUI Upstream/MintLibrary/MintGUI /S
robocopy MintLanguage Upstream/MintLibrary/MintLanguage /S
robocopy MintLibrary Upstream/MintLibrary/MintLibrary /S
robocopy MintMath Upstream/MintLibrary/MintMath /S
robocopy MintPhysics Upstream/MintLibrary/MintPhysics /S
robocopy MintPlatform Upstream/MintLibrary/MintPlatform /S
robocopy MintReflection Upstream/MintLibrary/MintReflection /S
robocopy MintRendering Upstream/MintLibrary/MintRendering /S

pause
