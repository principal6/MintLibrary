#pragma once


#ifndef MINT_STRING_UTIL_H
#define MINT_STRING_UTIL_H


#include <string>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/ScopeString.h>


namespace mint
{
    struct StringRange final
    {
                        StringRange();
                        StringRange(const uint32 offset);
                        StringRange(const uint32 offset, const uint32 length);
                        StringRange(const uint64 offset, const uint32 length);
                        StringRange(const uint64 offset, const uint64 length);

    public:
        const bool      isLengthSet() const noexcept;

    public:
        uint32          _offset;
        uint32          _length;
    };


    template <uint32 BufferSize>
    void                formatString(char(&buffer)[BufferSize], const char* format, ...);
    void                formatString(char* const buffer, const uint32 bufferSize, const char* format, ...);
    void                formatString(StringA& buffer, const uint32 bufferSize, const char* format, ...);
    template <uint32 BufferSize>
    void                formatString(ScopeStringA<BufferSize>& buffer, const char* format, ...);
    
    template <uint32 BufferSize>
    void                formatString(wchar_t(&buffer)[BufferSize], const wchar_t* format, ...);
    void                formatString(wchar_t* const buffer, const uint32 bufferSize, const wchar_t* format, ...);
    void                formatString(StringW& buffer, const uint32 bufferSize, const wchar_t* format, ...);
    template <uint32 BufferSize>
    void                formatString(ScopeStringW<BufferSize>& buffer, const wchar_t* format, ...);


    namespace StringUtil
    {
        bool            isNullOrEmpty(const char* const rawString);
        bool            isNullOrEmpty(const wchar_t* const rawWideString);
        
        uint32          length(const char* const rawString);
        uint32          length(const wchar_t* const rawWideString);

        template <typename T>
        uint32          find(const T* const source, const T* const target, const uint32 offset = 0);

        bool            compare(const char* const a, const char* const b);
        bool            compare(const wchar_t* const a, const wchar_t* const b);
        
        template <uint32 DestSize>
        void            copy(char(&dest)[DestSize], const char* const source);
        template <uint32 DestSize>
        void            copy(wchar_t(&dest)[DestSize], const wchar_t* const source);

        void            convertWideStringToString(const std::wstring& source, std::string& destination);
        void            convertStringToWideString(const std::string& source, std::wstring& destination);
        
        void            convertStringAToStringW(const StringA& source, StringW& destination) noexcept;
        
        template <uint32 BufferSize>
        void            convertScopeStringAToScopeStringW(const ScopeStringA<BufferSize>& source, ScopeStringW<BufferSize>& destination) noexcept;
        
        void            excludeExtension(std::string& inoutText);

        static void     tokenize(const std::string& inputString, const char delimiter, Vector<std::string>& outArray);
        static void     tokenize(const std::string& inputString, const Vector<char>& delimiterArray, Vector<std::string>& outArray);
        static void     tokenize(const std::string& inputString, const std::string& delimiterString, Vector<std::string>& outArray);

        template <typename T>
        std::enable_if_t<std::is_integral_v<T>, StringA>        convertToStringA(const T& rhs);
        template <typename T>
        std::enable_if_t<std::is_floating_point_v<T>, StringA>  convertToStringA(const T& rhs);

        template <typename T>
        std::enable_if_t<std::is_integral_v<T>, StringW>        convertToStringW(const T& rhs);
        template <typename T>
        std::enable_if_t<std::is_floating_point_v<T>, StringW>  convertToStringW(const T& rhs);

        float           convertStringWToFloat(const StringW& rhs);
    }
}


#include <MintContainer/Include/StringUtil.inl>


#endif // !MINT_STRING_UTIL_H
