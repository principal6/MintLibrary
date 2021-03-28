#pragma once


#ifndef FS_BINARY_FILE_H
#define FS_BINARY_FILE_H


#include <CommonDefinitions.h>

#include <FsPlatform/Include/IFile.h>


namespace fs
{
    class BinaryFileReader final : public IFileReader
    {
    public:
                                BinaryFileReader()  = default;
        virtual                 ~BinaryFileReader() = default;

    public:
        virtual const bool      open(const char* const fileName) override;
        virtual const bool      isOpen() const noexcept override;
        virtual const uint32    getFileSize() const noexcept override;

    public:
        template <typename T>
        const T* const          read() const noexcept;

        template <typename T>
        const T* const          read(const uint32 count) const noexcept;

        void                    skip(const uint32 byteCount) const noexcept;

    private:
        const bool              canRead(const uint32 byteCount) const noexcept;

    private:
        std::vector<byte>       _byteArray;
        mutable uint32          _at{ 0 };
    };


    class BinaryFileWriter final : public IFileWriter
    {
    public:
                                BinaryFileWriter()  = default;
        virtual                 ~BinaryFileWriter() = default;

    public:
        virtual const bool      save(const char* const fileName) override;

    public:
        void                    clear();

    public:
        template <typename T>
        void                    write(const T& in) noexcept;
        
        template <typename T>
        void                    write(T&& in) noexcept;

        void                    write(const char* const in) noexcept;

    private:
        std::vector<byte>       _byteArray;
    };
}


#endif // !FS_BINARY_FILE_H
