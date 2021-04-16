#include <FsPlatform/Include/TextFile.h>

#include <FsContainer/Include/StringUtil.hpp>
#include <FsContainer/Include/Vector.hpp>

#include <fstream>


namespace fs
{
    const bool TextFileReader::open(const char* const fileName)
    {
        std::ifstream ifs{ fileName, std::ifstream::binary };
        if (ifs.is_open() == false)
        {
            _byteArray.clear();
            return false;
        }

        _byteArray.clear();

        bool isBomChecked = false;
        while (ifs.eof() == false)
        {
            byte readByte{ static_cast<byte>(ifs.get()) };
            _byteArray.push_back(readByte);

            // BOM »Æ¿Œ
            if (isBomChecked == false && _byteArray.size() == 3)
            {
                if (_byteArray.at(0) == 0xEF &&
                    _byteArray.at(1) == 0xBB &&
                    _byteArray.at(2) == 0xBF)
                {
                    // UTF-8 (BOM)
                    _encoding = TextFileEncoding::UTF8_BOM;
                    _byteArray.clear();

                    isBomChecked = true;
                }
            }
        }

        if (_byteArray.empty() == false)
        {
            if (_byteArray.back() == 255)
            {
                _byteArray.back() = 0;
            }
        }

        return true;
    }

    const bool TextFileReader::isOpen() const noexcept
    {
        return !_byteArray.empty();
    }

    const uint32 TextFileReader::getFileSize() const noexcept
    {
        return static_cast<uint32>(_byteArray.size());
    }

    const char TextFileReader::get(const uint32 at) const noexcept
    {
        return static_cast<char>(_byteArray.at(at));
    }

    const char* TextFileReader::get() const noexcept
    {
        return reinterpret_cast<const char*>(&_byteArray.front());
    }

    const bool TextFileWriter::save(const char* const fileName)
    {
        std::ofstream ofs{ fileName, std::ofstream::binary };
        if (ofs.is_open() == false)
        {
            return false;
        }
        if (_byteArray.empty() == true)
        {
            return false;
        }

        if (_encoding == TextFileEncoding::UTF8_BOM)
        {
            bool writeBom = true;
            if (3 <= _byteArray.size())
            {
                if (_byteArray.at(0) == 0xEF &&
                    _byteArray.at(1) == 0xBB &&
                    _byteArray.at(2) == 0xBF)
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

        ofs.write(reinterpret_cast<const char*>(&_byteArray.front()), _byteArray.size());
        return false;
    }

    void TextFileWriter::clear()
    {
        _byteArray.clear();
    }

    void TextFileWriter::write(const char ch) noexcept
    {
        _byteArray.push_back(ch);
    }

    void TextFileWriter::write(const char* const text) noexcept
    {
        if (text == nullptr)
        {
            return;
        }

        const uint32 length = fs::StringUtil::strlen(text);
        for (uint32 at = 0; at < length; ++at)
        {
            _byteArray.push_back(text[at]);
        }
    }
}
