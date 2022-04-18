#pragma once


#ifndef MINT_I_FILE_H
#define MINT_I_FILE_H


namespace mint
{
    class IFileReader abstract
    {
    public:
                                IFileReader()   = default;
        virtual                 ~IFileReader()  = default;

    public:
        virtual bool            open(const char* const fileName) abstract;
        virtual bool            isOpen() const noexcept abstract;
        virtual uint32          getFileSize() const noexcept abstract;
    };


    class IFileWriter abstract
    {
    public:
                                IFileWriter()   = default;
        virtual                 ~IFileWriter()  = default;

    public:
        virtual bool            save(const char* const fileName) abstract;
    };
}


#endif // !MINT_IFILE_H
