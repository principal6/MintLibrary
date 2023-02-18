#pragma once


#ifndef _MINT_PLATFORM_TEXT_FILE_H_
#define _MINT_PLATFORM_TEXT_FILE_H_


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
		virtual ~TextFileReader() = default;

	public:
		virtual bool Open(const char* const fileName) override;
		virtual bool IsOpen() const noexcept override;
		virtual uint32 GetFileSize() const noexcept override;

	public:
		char Get(const uint32 at) const noexcept;
		const char* Get() const noexcept;

	private:
		Vector<byte> _byteArray;
		TextFileEncoding _encoding = TextFileEncoding::ASCII;
	};


	// TODO: Encoding
	class TextFileWriter final : public IFileWriter
	{
	public:
		TextFileWriter() = default;
		TextFileWriter(TextFileEncoding encoding) : _encoding{ encoding } { __noop; }
		virtual ~TextFileWriter() = default;

	public:
		virtual bool Save(const char* const fileName) override;

	public:
		void Clear();

	public:
		void Write(const char ch) noexcept;
		void Write(const char* const text) noexcept;

	private:
		Vector<byte> _byteArray;
		TextFileEncoding _encoding = TextFileEncoding::ASCII;
	};
}


#endif // !_MINT_PLATFORM_TEXT_FILE_H_
