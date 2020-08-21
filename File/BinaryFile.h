#pragma once


#ifndef FS_BINARY_FILE_H
#define FS_BINARY_FILE_H


#include <CommonDefinitions.h>
#include <File/IFile.h>
#include <vector>
#include <string>


namespace fs
{
	class BinaryFileReader : public IFileReader
	{
	public:
								BinaryFileReader();
								BinaryFileReader(const char* const fileName);
		virtual					~BinaryFileReader();

	public:
		virtual bool			load(const char* const fileName) override;

	public:
		virtual void			skip(const uint32 byteCount) const noexcept;

	public:
		virtual bool			readBool(bool& out) const noexcept;
		virtual bool			readInt8(int8& out) const noexcept;
		virtual bool			readInt16(int16& out) const noexcept;
		virtual bool			readInt32(int32& out) const noexcept;
		virtual bool			readUint8(uint8& out) const noexcept;
		virtual bool			readUint16(uint16& out) const noexcept;
		virtual bool			readUint32(uint32& out) const noexcept;
		virtual bool			readFloat(float& out) const noexcept;
		virtual bool			readString(const uint32 byteCount, std::string& out) const noexcept;

	protected:
		bool					canRead(const uint32 byteCount) const noexcept;

	protected:
		std::vector<byte>		_byteArray;
		mutable uint32			_at;
	};


	class BinaryFileWriter : public IFileWriter
	{
	public:
								BinaryFileWriter();
		virtual					~BinaryFileWriter();

	public:
		virtual bool			save(const char* const fileName) override;

	public:
		virtual void			clear();

	public:
		virtual void			writeBool(bool in) noexcept;
		virtual void			writeInt8(int8 in) noexcept;
		virtual void			writeInt16(int16 in) noexcept;
		virtual void			writeInt32(int32 in) noexcept;
		virtual void			writeUint8(uint8 in) noexcept;
		virtual void			writeUint16(uint16 in) noexcept;
		virtual void			writeUint32(uint32 in) noexcept;
		virtual void			writeFloat(float in) noexcept;
		virtual void			writeString(const std::string& in) noexcept;

	protected:
		std::vector<byte>		_byteArray;
	};
}


#endif // !FS_BINARY_FILE_H
