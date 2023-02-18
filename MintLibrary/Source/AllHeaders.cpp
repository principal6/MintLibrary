#include <MintLibrary/Include/AllHeaders.h>
#include <MintPlatform/Include/FileUtil.hpp>


#ifdef MINT_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


namespace mint
{
	inline constexpr uint16 kMintLibraryVersionMajor = 1;
	inline constexpr uint16 kMintLibraryVersionMinor = 0;

	void Library::Initialize() noexcept
	{
#ifdef MINT_DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

		// #1(개발용): MintLibrary 솔루션에서 MintLibraryTest 가 시작 프로젝트이고, Working Directory 가 $(ProjectDir) 일 경우
		if (mint::FileUtil::exists("../Assets"))
		{
			mint::Path::SetAssetDirectory("../Assets/");
			mint::Path::SetIncludeAssetDirectory("../Assets/Include/");
		}
		// #2(배포용): 신규 솔루션에서 Working Directory 가 $(ProjectDir) 일 경우
		else if (mint::FileUtil::exists("../MintLibrary/Include/Assets"))
		{
			mint::Path::SetAssetDirectory("../MintLibrary/Include/Assets/");
			mint::Path::SetIncludeAssetDirectory("../MintLibrary/Include/Assets/Include/");
		}
		// #3(Fallback): 기본 세팅
		else
		{
			mint::Path::SetAssetDirectory("Assets/");
			mint::Path::SetIncludeAssetDirectory("Assets/Include/");
		}

		PrintVersion();

#if !defined MINT_DEBUG
		HWND handleToConsoleWindow = ::GetConsoleWindow();
		::FreeConsole();
		::SendMessageW(handleToConsoleWindow, WM_CLOSE, 0, 0);
#endif
	}

	uint16 Library::GetVersionMajor() noexcept
	{
		return kMintLibraryVersionMajor;
	}

	uint16 Library::GetVersionMinor() noexcept
	{
		return kMintLibraryVersionMinor;
	}

	void Library::PrintVersion() noexcept
	{
		MINT_LOG_UNTAGGED("> This is MintLibrary");
		MINT_LOG_UNTAGGED("> Version %d.%02d\n", Library::GetVersionMajor(), Library::GetVersionMinor());
	}

	void Library::PrintCoordinateSystem() noexcept
	{
		MINT_LOG_UNTAGGED("> [MintLibrary Coordinate System]");
		MINT_LOG_UNTAGGED("  - MintLibrary uses a right-handed coordinate system");
		MINT_LOG_UNTAGGED("    with +y as world up axis and +z towards the viewer from the screen.");
		MINT_LOG_UNTAGGED("    As for vertex winding, counter-clockwise means front-face.");
		MINT_LOG_UNTAGGED("");
		MINT_LOG_UNTAGGED("  - 2D vertices are stored as follows");
		MINT_LOG_UNTAGGED("    0 -- 1      0 -- 1     0.    ");
		MINT_LOG_UNTAGGED("    |    |  ==   '.  |  +  | `.  ");
		MINT_LOG_UNTAGGED("    2 -- 3         ` 3     2 -- 3");
		MINT_LOG_UNTAGGED("");
		MINT_LOG_UNTAGGED("  - 3D vertices are stored as follows");
		MINT_LOG_UNTAGGED("    0 -- 3      0.         0.-- 3");
		MINT_LOG_UNTAGGED("    |    |  ==  | `.    +    `. |");
		MINT_LOG_UNTAGGED("    1 -- 2      1 --`2         `2");
		MINT_LOG_UNTAGGED("");
	}

	// 2D Tri == { 0, 1, 2 }
	// 2D Quad == { 0, 3, 1 } + { 0, 2, 3 }
}
