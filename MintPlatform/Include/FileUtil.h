#pragma once


#ifndef __MINT_PLATFORM_FILE_UTIL_H__
#define __MINT_PLATFORM_FILE_UTIL_H__


namespace mint
{
    namespace FileUtil
    {
        bool    isFile(const char* const fileName) noexcept;
        bool    isDirectory(const char* const fileName) noexcept;
        bool    isReadOnly(const char* const fileName) noexcept;
        void    unsetReadOnly(const char* const fileName) noexcept;
        bool    exists(const char* const path) noexcept;
        bool    createDirectory(const char* const directoryName) noexcept;
        bool    deleteFile(const char* const fileName) noexcept;
    }
}


#endif // !__MINT_PLATFORM_FILE_UTIL_H__
