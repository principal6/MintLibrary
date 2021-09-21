#pragma once


#ifndef MINT_STRING_UTIL_HPP
#define MINT_STRING_UTIL_HPP


#include <MintContainer/Include/StringUtil.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/ScopeString.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/Tree.hpp>


namespace mint
{
    template <uint32 BufferSize>
    MINT_INLINE void formatString(char(&buffer)[BufferSize], const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vsprintf_s(buffer, format, args);
        va_end(args);
    }

    MINT_INLINE void formatString(char* const buffer, const uint32 bufferSize, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vsprintf_s(buffer, bufferSize, format, args);
        va_end(args);
    }

    MINT_INLINE void formatString(StringA& buffer, const uint32 bufferSize, const char* format, ...)
    {
        buffer.resize(bufferSize);

        va_list args;
        va_start(args, format);
        ::vsprintf_s(&buffer[0], bufferSize, format, args);
        va_end(args);
    }
    
    template <uint32 BufferSize>
    MINT_INLINE void formatString(ScopeStringA<BufferSize>& buffer, const char* format, ...)
    {
        ScopeStringA<BufferSize> temp;

        va_list args;
        va_start(args, format);
        ::vsprintf_s(temp.data(), BufferSize, format, args);
        va_end(args);

        buffer.resize(temp.length());
        buffer.assign(temp);
    }

    template <uint32 BufferSize>
    MINT_INLINE void formatString(wchar_t(&buffer)[BufferSize], const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vswprintf_s(buffer, format, args);
        va_end(args);
    }

    MINT_INLINE void formatString(wchar_t* const buffer, const uint32 bufferSize, const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vswprintf_s(buffer, bufferSize, format, args);
        va_end(args);
    }

    MINT_INLINE void formatString(StringW& buffer, const uint32 bufferSize, const wchar_t* format, ...)
    {
        buffer.resize(bufferSize);

        va_list args;
        va_start(args, format);
        ::vswprintf_s(&buffer[0], bufferSize, format, args);
        va_end(args);
    }

    template <uint32 BufferSize>
    MINT_INLINE void formatString(ScopeStringW<BufferSize>& buffer, const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vswprintf_s(buffer.data(), BufferSize, format, args);
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

    // 나중에 string 구현하고 나면 없앨 것!!!
    inline StringRange::StringRange(const uint64 offset, const uint32 length)
        : _offset{ static_cast<uint32>(offset) }
        , _length{ length }
    {
    }

    // 나중에 string 구현하고 나면 없앨 것!!!
    inline StringRange::StringRange(const uint64 offset, const uint64 length)
        : _offset{ static_cast<uint32>(offset) }
        , _length{ static_cast<uint32>(length) }
    {
        __noop;
    }

    MINT_INLINE const bool StringRange::isLengthSet() const noexcept
    {
        return _length;
    }


    namespace StringUtil
    {
        MINT_INLINE void convertWideStringToString(const std::wstring& source, std::string& destination)
        {
            destination.resize(source.length());
            ::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()), nullptr, nullptr);
        }

        MINT_INLINE void convertStringToWideString(const std::string& source, std::wstring& destination)
        {
            destination.resize(source.length());
            ::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()));
        }

        MINT_INLINE void convertStringAToStringW(const StringA& source, StringW& destination) noexcept
        {
            destination.resize(source.length());
            ::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(source.length()));
        }

        template<uint32 BufferSize>
        inline void convertScopeStringAToScopeStringW(const ScopeStringA<BufferSize>& source, ScopeStringW<BufferSize>& destination) noexcept
        {
            destination.resize(source.length());
            ::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(source.length()));
        }

        MINT_INLINE const bool hasExtension(std::string& inoutText)
        {
            const size_t found = inoutText.find('.');
            return (found != std::string::npos);
        }

        MINT_INLINE void excludeExtension(std::string& inoutText)
        {
            const size_t found = inoutText.find('.');
            if (found != std::string::npos)
            {
                inoutText = inoutText.substr(0, found);
            }
        }

        inline void tokenize(const std::string& inputString, const char delimiter, mint::Vector<std::string>& outArray)
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

        inline void tokenize(const std::string& inputString, const mint::Vector<char>& delimiterArray, mint::Vector<std::string>& outArray)
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

        inline void tokenize(const std::string& inputString, const std::string& delimiterString, mint::Vector<std::string>& outArray)
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

        inline float convertStringWToFloat(const StringW& rhs)
        {
            float result = 0.0f;
            try
            {
                result = std::stof(rhs.c_str());
            }
            catch (std::invalid_argument e)
            {
                throw(e);
            }
            return result;
        }

        template <typename T>
        inline std::enable_if_t<std::is_integral_v<T>, StringW> toStringW(const T& rhs)
        {
            ScopeStringW<256> buffer;
            formatString(buffer, L"%d", rhs);
            return StringW(buffer.c_str());
        }

        template <typename T>
        inline std::enable_if_t<std::is_floating_point_v<T>, StringW> toStringW(const T& rhs)
        {
            ScopeStringW<256> buffer;
            formatString(buffer, L"%f", rhs);
            return StringW(buffer.c_str());
        }
    }
}


#endif // !MINT_STRING_UTIL_HPP
