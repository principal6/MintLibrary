#include <MintPlatform/Include/Test.h>

#include <MintPlatform/Include/AllHeaders.h>
#include <MintPlatform/Include/AllHpps.h>


namespace mint
{
	namespace TestPlatform
	{
		bool test()
		{
            MINT_ASSURE(test_files());
            return true;
		}

        bool test_files()
        {
            using namespace mint;

            static constexpr const char* const kFileName = "MintLibraryTest/test.bin";
            static constexpr const char* const kRawString = "abc";
            BinaryFileWriter bfw;
            bfw.write(3.14f);
            bfw.write(true);
            bfw.write(static_cast<uint16>(0xABCD));
            bfw.write("hello");
            bfw.write(kRawString);
            bfw.save(kFileName);

            BinaryFileReader bfr;
            bfr.open(kFileName);
            if (bfr.isOpen() == true)
            {
                auto a = bfr.read<float>();
                auto b = bfr.read<bool>();
                auto c = bfr.read<uint16>();
                auto d = bfr.read<char>(6);
                auto e = bfr.read<char>(4);
                printf("File[%s] %s %s\n", kFileName, d, e);
            }

            TextFileReader tfr;
            tfr.open("MintLibraryTest/test.cpp");
            return true;
        }
	}
}
