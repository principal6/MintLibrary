#pragma once


namespace mint
{
    namespace FileUtil
    {
        const bool  isFile(const char* const fileName) noexcept;
        const bool  isDirectory(const char* const fileName) noexcept;
        const bool  isReadOnly(const char* const fileName) noexcept;
        void        unsetReadOnly(const char* const fileName) noexcept;
        const bool  exists(const char* const path) noexcept;
        const bool  createDirectory(const char* const directoryName) noexcept;
        const bool  deleteFile(const char* const fileName) noexcept;
    }
}
