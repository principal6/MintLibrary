#include <MintLibrary/Include/AllHeaders.h>
#include <MintPlatform/Include/FileUtil.hpp>


namespace mint
{
    inline constexpr uint16 kMintLibraryVersionMajor  = 1;
    inline constexpr uint16 kMintLibraryVersionMinor  = 0;

    void Library::initialize() noexcept
    {
        // #1(개발용): MintLibrary 솔루션에서 MintLibraryTest 가 시작 프로젝트이고, Working Directory 가 $(ProjectDir) 일 경우
        if (mint::FileUtil::exists("../Assets"))
        {
            mint::Path::setAssetDirectory("../Assets/");
            mint::Path::setIncludeAssetDirectory("../Assets/Include/");
        }
        // #2(배포용): 신규 솔루션에서 Working Directory 가 $(ProjectDir) 일 경우
        else if (mint::FileUtil::exists("../MintLibrary/Include/Assets"))
        {
            mint::Path::setAssetDirectory("../MintLibrary/Include/Assets/");
            mint::Path::setIncludeAssetDirectory("../MintLibrary/Include/Assets/Include/");
        }
        // #3(Fallback): 기본 세팅
        else
        {
            mint::Path::setAssetDirectory("Assets/");
            mint::Path::setIncludeAssetDirectory("Assets/Include/");
        }

        printVersion();
    }

    uint16 Library::getVersionMajor() noexcept
    {
        return kMintLibraryVersionMajor;
    }

    uint16 Library::getVersionMinor() noexcept
    {
        return kMintLibraryVersionMinor;
    }

    void Library::printVersion() noexcept
    {
        MINT_LOG_UNTAGGED("> This is MintLibrary");
        MINT_LOG_UNTAGGED("> Version %d.%02d\n", Library::getVersionMajor(), Library::getVersionMinor());
    }

    void Library::printCoordinateSystem() noexcept
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
