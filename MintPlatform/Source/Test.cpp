#include <MintPlatform/Include/Test.h>

#include <MintPlatform/Include/AllHeaders.h>
#include <MintPlatform/Include/AllHpps.h>


namespace mint
{
	namespace TestPlatform
	{
		bool Test()
		{
			MINT_ASSURE(Test_files());
			return true;
		}

		bool Test_files()
		{
			using namespace mint;

			static constexpr const char* const kFileName = "MintLibraryTest/test.bin";
			static constexpr const char* const kRawString = "abc";
			BinaryFileWriter bfw;
			bfw.Write(3.14f);
			bfw.Write(true);
			bfw.Write(static_cast<uint16>(0xABCD));
			bfw.Write("hello");
			bfw.Write(kRawString);
			bfw.Save(kFileName);

			BinaryFileReader bfr;
			bfr.Open(kFileName);
			if (bfr.IsOpen() == true)
			{
				auto a = bfr.Read<float>();
				auto b = bfr.Read<bool>();
				auto c = bfr.Read<uint16>();
				auto d = bfr.Read<char>(6);
				auto e = bfr.Read<char>(4);
				printf("File[%s] %s %s\n", kFileName, d, e);
			}

			TextFileReader tfr;
			tfr.Open("MintLibraryTest/test.cpp");
			return true;
		}
	}
}
