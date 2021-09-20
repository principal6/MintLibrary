#include <MintPlatform/Include/BinaryFile.h>

#include <MintContainer/Include/Vector.hpp>

#include <fstream>


namespace mint
{
#pragma region Binary File Reader
    const bool BinaryFileReader::open(const char* const fileName)
    {
        _byteArray.clear();
        
        std::ifstream ifs{ fileName, std::ifstream::binary };
        if (ifs.is_open() == false)
        {
            return false;
        }

        ifs.seekg(0, ifs.end);
        const uint64 legth = ifs.tellg();
        ifs.seekg(0, ifs.beg);
        _byteArray.reserve(static_cast<uint32>(legth));
        while (ifs.eof() == false)
        {
            byte readByte{ static_cast<byte>(ifs.get()) };
            _byteArray.push_back(readByte);
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
