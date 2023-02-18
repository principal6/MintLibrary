#pragma once


#ifndef _MINT_PLATFORM_I_FILE_H_
#define _MINT_PLATFORM_I_FILE_H_


namespace mint
{
	class IFileReader abstract
	{
	public:
		IFileReader() = default;
		virtual ~IFileReader() = default;

	public:
		virtual bool Open(const char* const fileName) abstract;
		virtual bool IsOpen() const noexcept abstract;
		virtual uint32 GetFileSize() const noexcept abstract;
	};


	class IFileWriter abstract
	{
	public:
		IFileWriter() = default;
		virtual ~IFileWriter() = default;

	public:
		virtual bool Save(const char* const fileName) abstract;
	};
}


#endif // !MINT_IFILE_H
