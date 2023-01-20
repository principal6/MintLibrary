#pragma once


#ifndef _MINT_PLATFORM_BINARY_FILE_H_
#define _MINT_PLATFORM_BINARY_FILE_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>
#include <MintPlatform/Include/IFile.h>


namespace mint
{
	class BinaryFileReader;


	class BinaryPointerReader final
	{
	public:
		BinaryPointerReader();
		BinaryPointerReader(const byte* const bytes, const uint32 byteCount);
		BinaryPointerReader(const BinaryFileReader& binaryFileReader);
		~BinaryPointerReader() = default;

	public:
		void reset(const byte* const bytes, const uint32 byteCount) { _bytes = bytes; _byteCount = byteCount; }

		void goTo(const uint32 at) const;

		bool canRead(const uint32 count) const;
		
		template <typename T>
		const T* const peek() const;

		template <typename T>
		const T* const read() const;

		template <typename T>
		const T* const read(const uint32 count) const;

		void skip(const uint32 count) const;

	private:
		const byte* _bytes;
		uint32 _byteCount;
		mutable uint32 _at{ 0 };
	};


	class BinaryFileReader final : public IFileReader
	{
	public:
		BinaryFileReader() = default;
		virtual ~BinaryFileReader() = default;

	public:
		virtual bool open(const char* const fileName) override;
		virtual bool isOpen() const noexcept override;
		virtual uint32 getFileSize() const noexcept override;

	public:
		void goTo(const uint32 at);

		bool canRead(const uint32 count) const noexcept;

		template <typename T>
		const T* const peek() const noexcept;

		template <typename T>
		const T* const read() noexcept;

		template <typename T>
		const T* const read(const uint32 count) noexcept;

		void skip(const uint32 count) noexcept;

		const Vector<byte>& getBytes() const { return _bytes; }

	private:
		Vector<byte> _bytes;
		BinaryPointerReader _binaryPointerReader;
	};


	class BinaryFileWriter final : public IFileWriter
	{
	public:
		BinaryFileWriter() = default;
		virtual ~BinaryFileWriter() = default;

	public:
		virtual bool save(const char* const fileName) override;

	public:
		void clear();

	public:
		template <typename T>
		void write(const T& in) noexcept;

		template <typename T>
		void write(T&& in) noexcept;

		void write(const char* const in) noexcept;

		void write(const void* const in, const uint32 byteCount) noexcept;

	private:
		void _writeInternal(const void* const in, const uint32 currentSize, const uint32 deltaSize) noexcept;

	private:
		Vector<byte> _bytes;
	};
}


#endif // !_MINT_PLATFORM_BINARY_FILE_H_
