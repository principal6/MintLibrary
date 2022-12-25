#pragma once


#ifndef MINT_BINARY_FILE_H
#define MINT_BINARY_FILE_H


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>
#include <MintPlatform/Include/IFile.h>


namespace mint
{
    class BinaryFileReader final : public IFileReader
    {
    public:
                                BinaryFileReader()  = default;
        virtual                 ~BinaryFileReader() = default;

    public:
        virtual bool            open(const char* const fileName) override;
        virtual bool            isOpen() const noexcept override;
        virtual uint32          getFileSize() const noexcept override;

    public:
        template <typename T>
        const T* const          read() const noexcept;

        template <typename T>
        const T* const          read(const uint32 count) const noexcept;

        void                    skip(const uint32 byteCount) const noexcept;

    private:
        bool                    canRead(const uint32 byteCount) const noexcept;

    private:
        Vector<byte>            _byteArray;
        mutable uint32          _at{ 0 };
    };


    class BinaryFileWriter final : public IFileWriter
    {
    public:
                                BinaryFileWriter()  = default;
        virtual                 ~BinaryFileWriter() = default;

    public:
        virtual bool            save(const char* const fileName) override;

    public:
        void                    clear();

    public:
        template <typename T>
        void                    write(const T& in) noexcept;
        
        template <typename T>
        void                    write(T&& in) noexcept;

        void                    write(const char* const in) noexcept;

        void                    write(const void* const in, const uint32 byteCount) noexcept;

    private:
        void                    _writeInternal(const void* const in, const uint32 currentSize, const uint32 deltaSize) noexcept;

    private:
        Vector<byte>            _byteArray;
    };
}


#endif // !MINT_BINARY_FILE_H
