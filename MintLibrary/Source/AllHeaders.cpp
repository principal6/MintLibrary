#include <stdafx.h>
#include <MintLibrary/Include/AllHeaders.h>


namespace mint
{
    inline constexpr uint16 kMintLibraryVersionMajor  = 1;
    inline constexpr uint16 kMintLibraryVersionMinor  = 0;

    const uint16 MintLibraryVersion::getVersionMajor() noexcept
    {
        return kMintLibraryVersionMajor;
    }

    const uint16 MintLibraryVersion::getVersionMinor() noexcept
    {
        return kMintLibraryVersionMinor;
    }

    void MintLibraryVersion::printVersion() noexcept
    {
        MINT_LOG_UNTAGGED("MintLibrary", "> This is MintLibrary");
        MINT_LOG_UNTAGGED("MintLibrary", "> Version %d.%02d\n", mint::MintLibraryVersion::getVersionMajor(), mint::MintLibraryVersion::getVersionMinor());
    }
}
