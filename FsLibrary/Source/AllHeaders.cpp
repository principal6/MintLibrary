#include <stdafx.h>
#include <FsLibrary/Include/AllHeaders.h>


namespace fs
{
    inline constexpr uint16 kFsLibraryVersionMajor  = 1;
    inline constexpr uint16 kFsLibraryVersionMinor  = 0;

    const uint16 FsLibraryVersion::getVersionMajor() noexcept
    {
        return kFsLibraryVersionMajor;
    }

    const uint16 FsLibraryVersion::getVersionMinor() noexcept
    {
        return kFsLibraryVersionMinor;
    }

    void FsLibraryVersion::printVersion() noexcept
    {
        FS_LOG_PURE("FsLibrary", "> This is FsLibrary", fs::FsLibraryVersion::getVersionMajor(), fs::FsLibraryVersion::getVersionMinor());
        FS_LOG_PURE("FsLibrary", "> Version %d.%02d\n", fs::FsLibraryVersion::getVersionMajor(), fs::FsLibraryVersion::getVersionMinor());
    }
}
