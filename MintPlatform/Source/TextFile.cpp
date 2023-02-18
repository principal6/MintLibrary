#include <MintPlatform/Include/TextFile.h>

#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/Vector.hpp>

#include <fstream>


namespace mint
{
	bool TextFileReader::Open(const char* const fileName)
	{
		std::ifstream ifs{ fileName, std::ifstream::binary };
		if (ifs.is_open() == false)
		{
			_byteArray.Clear();
			return false;
		}

		_byteArray.Clear();

		bool isBomChecked = false;
		while (ifs.eof() == false)
		{
			byte readByte{ static_cast<byte>(ifs.get()) };
			_byteArray.PushBack(readByte);

			// BOM 확인
			if (isBomChecked == false && _byteArray.Size() == 3)
			{
				if (_byteArray.At(0) == 0xEF &&
					_byteArray.At(1) == 0xBB &&
					_byteArray.At(2) == 0xBF)
				{
					// UTF-8 (BOM)
					_encoding = TextFileEncoding::UTF8_BOM;
					_byteArray.Clear();

					isBomChecked = true;
				}
			}
		}

		if (_byteArray.IsEmpty() == false)
		{
			if (_byteArray.Back() == 255)
			{
				_byteArray.Back() = 0;
			}
		}

		return true;
	}

	bool TextFileReader::IsOpen() const noexcept
	{
		return !_byteArray.IsEmpty();
	}

	uint32 TextFileReader::GetFileSize() const noexcept
	{
		return static_cast<uint32>(_byteArray.Size());
	}

	char TextFileReader::Get(const uint32 at) const noexcept
	{
		return static_cast<char>(_byteArray.At(at));
	}

	const char* TextFileReader::Get() const noexcept
	{
		return reinterpret_cast<const char*>(&_byteArray.Front());
	}

	bool TextFileWriter::Save(const char* const fileName)
	{
		std::ofstream ofs{ fileName, std::ofstream::binary };
		if (ofs.is_open() == false)
		{
			return false;
		}
		if (_byteArray.IsEmpty() == true)
		{
			return false;
		}

		if (_encoding == TextFileEncoding::UTF8_BOM)
		{
			bool writeBom = true;
			if (_byteArray.Size() >= 3)
			{
				if (_byteArray.At(0) == 0xEF &&
					_byteArray.At(1) == 0xBB &&
					_byteArray.At(2) == 0xBF)
				{
					writeBom = false;
				}
			}

			// BOM
			if (writeBom == true)
			{
				ofs.put(static_cast<char>(0xEF));
				ofs.put(static_cast<char>(0xBB));
				ofs.put(static_cast<char>(0xBF));
			}
		}

		ofs.write(reinterpret_cast<const char*>(&_byteArray.Front()), _byteArray.Size());
		return false;
	}

	void TextFileWriter::Clear()
	{
		_byteArray.Clear();
	}

	void TextFileWriter::Write(const char ch) noexcept
	{
		_byteArray.PushBack(ch);
	}

	void TextFileWriter::Write(const char* const text) noexcept
	{
		if (text == nullptr)
		{
			return;
		}

		const uint32 length = StringUtil::Length(text);
		for (uint32 at = 0; at < length; ++at)
		{
			_byteArray.PushBack(text[at]);
		}
	}
}
