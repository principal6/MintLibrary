#pragma once


#ifndef FS_TEXT_FILE_H
#define FS_TEXT_FILE_H


#include <CommonDefinitions.h>
#include <FsLibrary/File/IFile.h>
#include <FsLibrary/ContiguousContainer/ContiguousVector.h>


namespace fs
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
		virtual						~TextFileReader() = default;

	public:
		virtual const bool			open(const char* const fileName) override;
		virtual const bool			isOpen() const noexcept override;
		virtual const uint32		getFileSize() const noexcept override;

	public:
		const char					get(const uint32 at) const noexcept;
		const char*					get() const noexcept;
		
	private:
		fs::ContiguousVector<byte>	_byteArray;
		TextFileEncoding			_encoding = TextFileEncoding::ASCII;
	};


	// TODO: Encoding
	class TextFileWriter final : public IFileWriter
	{
	public:
									TextFileWriter() = default;
									TextFileWriter(TextFileEncoding encoding) : _encoding{ encoding } { __noop; }
		virtual						~TextFileWriter() = default;

	public:
		virtual const bool			save(const char* const fileName) override;

	public:
		void						clear();

	public:
		void						write(const char ch) noexcept;
		void						write(const char* const text) noexcept;

	private:
		fs::ContiguousVector<byte>	_byteArray;
		TextFileEncoding			_encoding = TextFileEncoding::ASCII;
	};
}


#endif // !FS_TEXT_FILE_H
