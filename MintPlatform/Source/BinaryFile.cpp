#include <MintPlatform/Include/BinaryFile.h>

#include <MintContainer/Include/Vector.hpp>

#include <fstream>


namespace mint
{
#pragma region Binary File Reader
    bool BinaryFileReader::open(const char* const fileName)
    {
        _at = 0;
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
        while (true)
        {
            const byte readByte{ static_cast<byte>(ifs.get()) };
            if (ifs.eof() == true)
            {
                break;
            }

            _byteArray.push_back(readByte);
        }
        return true;
    }

    bool BinaryFileReader::isOpen() const noexcept
    {
        return !_byteArray.empty();
    }

    uint32 BinaryFileReader::getFileSize() const noexcept
    {
        return static_cast<uint32>(_byteArray.size());
    }
#pragma endregion


#pragma region Binary File Writer
    bool BinaryFileWriter::save(const char* const fileName)
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
