#include <FsPlatform/Include/BinaryFile.h>

#include <fstream>


namespace fs
{
#pragma region Binary File Reader
    const bool BinaryFileReader::open(const char* const fileName)
    {
        std::ifstream ifs{ fileName, std::ifstream::binary };
        if (ifs.is_open() == false)
        {
            _byteArray.clear();
            return false;
        }

        _byteArray.clear();

        ifs.seekg(0, ifs.end);
        const uint64 legth = ifs.tellg();
        ifs.seekg(0, ifs.beg);
        _byteArray.reserve(legth);
        while (ifs.eof() == false)
        {
            byte readByte{ static_cast<byte>(ifs.get()) };
            _byteArray.emplace_back(readByte);
        }
        return true;
    }

    const bool BinaryFileReader::isOpen() const noexcept
    {
        return !_byteArray.empty();
    }

    const uint32 BinaryFileReader::getFileSize() const noexcept
    {
        return static_cast<uint32>(_byteArray.size());
    }
#pragma endregion


#pragma region Binary File Writer
    const bool BinaryFileWriter::save(const char* const fileName)
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

        ofs.write((const char*)&_byteArray[0], _byteArray.size());
        return false;
    }
#pragma endregion
}
