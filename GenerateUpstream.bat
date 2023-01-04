@echo off
echo Generate /Upstream directory, containing files for importing MintLibrary

: Project Directories
robocopy ./ Upstream/MintLibrary stdafx.h
robocopy MintCommon Upstream/MintLibrary/MintCommon /S
robocopy MintContainer Upstream/MintLibrary/MintContainer /S
robocopy MintGame Upstream/MintLibrary/MintGame /S
robocopy MintLanguage Upstream/MintLibrary/MintLanguage /S
robocopy MintLibrary Upstream/MintLibrary/MintLibrary /S
robocopy MintMath Upstream/MintLibrary/MintMath /S
robocopy MintPhysics Upstream/MintLibrary/MintPhysics /S
robocopy MintPlatform Upstream/MintLibrary/MintPlatform /S
robocopy MintReflection Upstream/MintLibrary/MintReflection /S
robocopy MintRendering Upstream/MintLibrary/MintRendering /S
robocopy MintRenderingBase Upstream/MintLibrary/MintRenderingBase /S

pause
