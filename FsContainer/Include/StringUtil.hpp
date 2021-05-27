#pragma once


#ifndef FS_STRING_UTIL_HPP
#define FS_STRING_UTIL_HPP


#include <FsContainer/Include/StringUtil.h>

#include <FsContainer/Include/Vector.hpp>
#include <FsContainer/Include/Tree.hpp>


namespace fs
{
    template <uint32 Size>
    FS_INLINE void formatString(char(&buffer)[Size], const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vsprintf_s(buffer, format, args);
        va_end(args);
    }

    FS_INLINE void formatString(char* const buffer, const uint32 bufferSize, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vsprintf_s(buffer, bufferSize, format, args);
        va_end(args);
    }

    template <uint32 Size>
    FS_INLINE void formatString(wchar_t(&buffer)[Size], const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vswprintf_s(buffer, format, args);
        va_end(args);
    }

    FS_INLINE void formatString(wchar_t* const buffer, const uint32 bufferSize, const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vswprintf_s(buffer, bufferSize, format, args);
        va_end(args);
    }


    inline StringRange::StringRange()
        : _offset{ 0 }
        , _length{ kUint32Max }
    {
        __noop;
    }

    inline StringRange::StringRange(const uint32 offset)
        : _offset{ offset }
        , _length{ kUint32Max }
    {
        __noop;
    }

    inline StringRange::StringRange(const uint32 offset, const uint32 length)
        : _offset{ offset }
        , _length{ length }
    {
        __noop;
    }

    FS_INLINE const bool StringRange::isLengthSet() const noexcept
    {
        return _length;
    }


    namespace StringUtil
    {
        FS_INLINE void convertWideStringToString(const std::wstring& source, std::string& destination)
        {
            destination.resize(source.length());
            ::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()), nullptr, nullptr);
        }

        FS_INLINE void convertStringToWideString(const std::string& source, std::wstring& destination)
        {
            destination.resize(source.length());
            ::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()));
        }

        FS_INLINE const bool hasExtension(std::string& inoutText)
        {
            const size_t found = inoutText.find('.');
            return (found != std::string::npos);
        }

        FS_INLINE void excludeExtension(std::string& inoutText)
        {
            const size_t found = inoutText.find('.');
            if (found != std::string::npos)
            {
                inoutText = inoutText.substr(0, found);
            }
        }

        inline void tokenize(const std::string& inputString, const char delimiter, fs::Vector<std::string>& outArray)
        {
            if (inputString.empty() == true)
            {
                return;
            }

            outArray.clear();

            uint32 prevAt = 0;
            const uint32 length = static_cast<uint32>(inputString.length());
            for (uint32 at = 0; at < length; ++at)
            {
                if (inputString.at(at) == delimiter)
                {
                    if (prevAt < at)
                    {
                        outArray.push_back(inputString.substr(prevAt, at - prevAt));
                    }

                    prevAt = at + 1;
                }
            }

            if (prevAt < length)
            {
                outArray.push_back(inputString.substr(prevAt, length - prevAt));
            }
        }

        inline void tokenize(const std::string& inputString, const fs::Vector<char>& delimiterArray, fs::Vector<std::string>& outArray)
        {
            if (inputString.empty() == true)
            {
                return;
            }

            outArray.clear();

            uint32 prevAt = 0;
            const uint32 delimiterCount = static_cast<uint32>(delimiterArray.size());
            const uint32 length = static_cast<uint32>(inputString.length());
            for (uint32 at = 0; at < length; ++at)
            {
                for (uint32 delimiterIndex = 0; delimiterIndex < delimiterCount; ++delimiterIndex)
                {
                    if (inputString.at(at) == delimiterArray.at(delimiterIndex))
                    {
                        if (prevAt < at)
                        {
                            outArray.push_back(inputString.substr(prevAt, at - prevAt));
                        }

                        prevAt = at + 1;
                    }
                }
            }

            if (prevAt < length)
            {
                outArray.push_back(inputString.substr(prevAt, length - prevAt));
            }
        }

        inline void tokenize(const std::string& inputString, const std::string& delimiterString, fs::Vector<std::string>& outArray)
        {
            if (inputString.empty() == true || delimiterString.empty() == true)
            {
                return;
            }

            outArray.clear();

            const uint64 length = inputString.length();
            const uint64 delimiterLength = delimiterString.length();
            uint64 at = 0;
            uint64 prevAt = 0;
            while (at < length)
            {
                if (0 == inputString.compare(at, delimiterLength, delimiterString))
                {
                    if (prevAt < at)
                    {
                        outArray.push_back(inputString.substr(prevAt, at - prevAt));
                    }

                    at += delimiterLength;
                    prevAt = at;
                }
                else
                {
                    ++at;
                }
            }

            if (prevAt < length)
            {
                outArray.push_back(inputString.substr(prevAt, length - prevAt));
            }
        }
    }
}


#endif // !FS_STRING_UTIL_HPP
