#include <stdafx.h>
#include <MintLibrary/Include/AllHeaders.h>


namespace mint
{
    inline constexpr uint16 kMintLibraryVersionMajor  = 1;
    inline constexpr uint16 kMintLibraryVersionMinor  = 0;

    const uint16 LibraryInfo::getVersionMajor() noexcept
    {
        return kMintLibraryVersionMajor;
    }

    const uint16 LibraryInfo::getVersionMinor() noexcept
    {
        return kMintLibraryVersionMinor;
    }

    void LibraryInfo::printVersion() noexcept
    {
        MINT_LOG_UNTAGGED("> This is MintLibrary");
        MINT_LOG_UNTAGGED("> Version %d.%02d\n", LibraryInfo::getVersionMajor(), LibraryInfo::getVersionMinor());
    }

    void LibraryInfo::printCoordinateSystem() noexcept
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
