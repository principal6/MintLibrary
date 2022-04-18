#pragma once


#ifndef MINT_TEXT_FILE_H
#define MINT_TEXT_FILE_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintPlatform/Include/IFile.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
    enum class TextFileEncoding
    {
        ASCII, // ANSI
        UTF8_BOM
    };


    class TextFileReader final : public IFileReader
    {
    public:
                                TextFileReader() = default;
        virtual                 ~TextFileReader() = default;

    public:
        virtual bool            open(const char* const fileName) override;
        virtual bool            isOpen() const noexcept override;
        virtual uint32          getFileSize() const noexcept override;

    public:
        char                    get(const uint32 at) const noexcept;
        const char*             get() const noexcept;

    private:
        Vector<byte>            _byteArray;
        TextFileEncoding        _encoding = TextFileEncoding::ASCII;
    };


    // TODO: Encoding
    class TextFileWriter final : public IFileWriter
    {
    public:
                                TextFileWriter() = default;
                                TextFileWriter(TextFileEncoding encoding) : _encoding{ encoding } { __noop; }
        virtual                 ~TextFileWriter() = default;

    public:
        virtual bool            save(const char* const fileName) override;

    public:
        void                    clear();

    public:
        void                    write(const char ch) noexcept;
        void                    write(const char* const text) noexcept;

    private:
        Vector<byte>            _byteArray;
        TextFileEncoding        _encoding = TextFileEncoding::ASCII;
    };
}


#endif // !MINT_TEXT_FILE_H
