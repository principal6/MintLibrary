#pragma once


#ifndef MINT_STRING_UTIL_H
#define MINT_STRING_UTIL_H


#include <string>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/StackString.h>


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
        bool            isLengthSet() const noexcept;

    public:
        uint32          _offset;
        uint32          _length;
    };

    class U8CharCodeViewer
    {
    public:
                            U8CharCodeViewer(const char8_t* const string) : _string{ string }, _byteAt{ 0 } { __noop; }
                            ~U8CharCodeViewer() = default;

    public:
        bool                operator!=(const U8CharCodeViewer& rhs) const;
        U8CharCode          operator*() const noexcept;
        U8CharCodeViewer    operator++();
        U8CharCodeViewer    begin() const;
        U8CharCodeViewer    end() const;

    private:
        const char8_t*  _string;
        uint32          _byteAt;
    };


    template <uint32 BufferSize>
    void                formatString(char(&buffer)[BufferSize], const char* format, ...);
    void                formatString(char* const buffer, const uint32 bufferSize, const char* format, ...);
    void                formatString(StringA& buffer, const uint32 bufferSize, const char* format, ...);
    template <uint32 BufferSize>
    void                formatString(StackStringA<BufferSize>& buffer, const char* format, ...);
    
    template <uint32 BufferSize>
    void                formatString(wchar_t(&buffer)[BufferSize], const wchar_t* format, ...);
    void                formatString(wchar_t* const buffer, const uint32 bufferSize, const wchar_t* format, ...);
    void                formatString(StringW& buffer, const uint32 bufferSize, const wchar_t* format, ...);
    template <uint32 BufferSize>
    void                formatString(StackStringW<BufferSize>& buffer, const wchar_t* format, ...);


    namespace StringUtil
    {
        template<typename T>
        constexpr bool      isNullOrEmpty(const T* const rawString);
        
        // returns the count of bytes in the string
        constexpr uint32    countByte(const char8_t* const string);
        constexpr uint32    countByteInCharCode(const U8CharCode u8CharCode);
        constexpr uint32    countByteInCharCode(const char8_t* const string, const uint32 byteAt);
        uint32              length(const char* const string);
        uint32              length(const wchar_t* const string);
        // returns the count of characters in the string
        constexpr uint32    length(const char8_t* const string);

        template <typename T>
        uint32          find(const T* const source, const T* const target, const uint32 offset = 0);

        bool            compare(const char* const a, const char* const b);
        bool            compare(const wchar_t* const a, const wchar_t* const b);
        
        template <uint32 DestSize>
        void            copy(char8_t(&dest)[DestSize], const char8_t* const source);
        template <uint32 DestSize>
        void            copy(char(&dest)[DestSize], const char* const source);
        template <uint32 DestSize>
        void            copy(wchar_t(&dest)[DestSize], const wchar_t* const source);

        constexpr U8CharCode    encode(const char8_t ch);
        constexpr U8CharCode    encode(const char8_t(&ch)[2]);
        constexpr U8CharCode    encode(const char8_t(&ch)[3]);
        constexpr U8CharCode    encode(const char8_t(&ch)[4]);
        constexpr U8CharCode    encode(const char8_t* const string, const uint32 byteAt);
        std::u8string           decode(const U8CharCode code);

        std::u8string   convertWideStringToUTF8(const std::wstring& source);
        std::wstring    convertUTF8ToWideString(const std::u8string& source);
        void            convertWideStringToString(const std::wstring& source, std::string& destination);
        void            convertStringToWideString(const std::string& source, std::wstring& destination);
        
        void            convertStringAToStringW(const StringA& source, StringW& destination) noexcept;
        
        template <uint32 BufferSize>
        void            convertStackStringAToStackStringW(const StackStringA<BufferSize>& source, StackStringW<BufferSize>& destination) noexcept;
        
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
