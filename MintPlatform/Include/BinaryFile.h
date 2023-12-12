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
		void Reset(const byte* const bytes, const uint32 byteCount) { _bytes = bytes; _byteCount = byteCount; _at = 0; }

		void GoTo(const uint32 at) const;

		bool CanRead(const uint32 count) const;
		
		template <typename T>
		const T* const Peek() const;

		template <typename T>
		const T* const Read() const;

		template <typename T>
		const T* const Read(const uint32 count) const;

		void Skip(const uint32 count) const;

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
		virtual bool Open(const char* const fileName) override;
		virtual bool IsOpen() const noexcept override;
		virtual uint32 GetFileSize() const noexcept override;

	public:
		void GoTo(const uint32 at);

		template <typename T>
		bool CanRead() const noexcept;

		bool CanRead(const uint32 count) const noexcept;

		template <typename T>
		const T* const Peek() const noexcept;

		template <typename T>
		const T* const Read() noexcept;

		template <typename T>
		const T* const Read(const uint32 count) noexcept;

		void Skip(const uint32 count) noexcept;

		const Vector<byte>& GetBytes() const { return _bytes; }

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
		virtual bool Save(const char* const fileName) override;

	public:
		void Clear();

	public:
		template <typename T>
		void Write(const T& in) noexcept;

		template <typename T>
		void Write(T&& in) noexcept;

		void Write(const char* const in) noexcept;

		void Write(const void* const in, const uint32 byteCount) noexcept;

	private:
		void WriteInternal(const void* const in, const uint32 currentSize, const uint32 deltaSize) noexcept;

	private:
		Vector<byte> _bytes;
	};
}


#endif // !_MINT_PLATFORM_BINARY_FILE_H_
