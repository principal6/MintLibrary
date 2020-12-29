#include <FsLibrary.h>


#ifdef FS_DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif


#pragma comment(lib, "FsLibrary.lib")


//#define FS_TEST_FAILURES
//#define FS_TEST_PERFORMANCE


void testIntTypes()
{
	using namespace fs;
	Int2 ni;
	Int2 a{ 1, 2 };
	Int2 b{ 3, 4 };
	Int2 c = a + b;
	Int2 d;
	d = a;
}

void testFloatTypes()
{
	using namespace fs;
	float a = 15.000001f;
	float b = 15.000002f;
	float c = 15000.0001f;
	float d = 15000.0005f;
	float e = 15000.0015f;

	struct
	{
		Float3 _a;
		Float3 _b;
	} st;
	auto sizeFloat2 = sizeof(Float2);
	auto sizeFloat3 = sizeof(Float3);
	auto sizeFloat4 = sizeof(Float4);
	auto sizeSt = sizeof(st);
	Float3 p{ 1, 0, 0 };
	Float3 q{ 0, 1, 0 };
	Float3 r = Float3::cross(p, q);
}

void testStaticArray()
{
	using namespace fs;
	constexpr StaticArray<int32, 3> arr{ 4, 5, 999 };
}

void testStackHolder()
{
	using namespace fs;
	{
		StackHolder<16, 6> sh;

		byte* shTestA = sh.registerSpace(2);
		memcpy(shTestA + (16 * 0), "1__abcdefghijk__", 16);
		memcpy(shTestA + (16 * 1), "2__lmnopqrstuv__", 16);

		byte* shTestB = sh.registerSpace(1);
		memcpy(shTestB + (16 * 0), "3__wxyzabcdefg__", 16);

		byte* shTestC = sh.registerSpace(1);
		memcpy(shTestC + (16 * 0), "4__helloMyFrie__", 16);

		sh.deregisterSpace(shTestB);

		byte* shTestD = sh.registerSpace(2);
		memcpy(shTestD + (16 * 0), "5__nd!!IAmLege__", 16);
	}
	{
		StackHolder<8, 12> sh;

		byte* shTestA = sh.registerSpace(7);
		memcpy(shTestA + (8 * 0), "01_abcd_", 8);
		memcpy(shTestA + (8 * 1), "02_efgh_", 8);
		memcpy(shTestA + (8 * 2), "03_ijkl_", 8);
		memcpy(shTestA + (8 * 3), "04_mnop_", 8);
		memcpy(shTestA + (8 * 4), "05_qrst_", 8);
		memcpy(shTestA + (8 * 5), "06_uvwx_", 8);
		memcpy(shTestA + (8 * 6), "07_yzab_", 8);

		byte* shTestB = sh.registerSpace(3);
		memcpy(shTestB + (8 * 0), "08_cdef_", 8);
		memcpy(shTestB + (8 * 1), "09_ghij_", 8);
		memcpy(shTestB + (8 * 2), "10_klmn_", 8);

		sh.deregisterSpace(shTestB);
	}
#ifdef FS_TEST_FAILURES
	{
		StackHolder<0, 0> shA; // THIS MUST FAIL!
		StackHolder<1, 0> shB; // THIS MUST FAIL!
		StackHolder<0, 1> shC; // THIS MUST FAIL!
	}
	{
		StackHolder<32, 16> sh;

		byte* shTestA = sh.registerSpace(16);
		memcpy((char*)(shTestA), "01_abcd_", 8);

		byte* shTestB = sh.registerSpace(8); // THIS MUST FAIL!
		sh.deregisterSpace(shTestB); // THIS MUST FAIL!
	}
#endif
}

const bool testBitVector()
{
	using fs::BitVector;

	BitVector a;
	a.reserveByteCapacity(4);
	a.push_back(true);
	a.push_back(false);
	a.push_back(true);
	a.push_back(false);
	a.push_back(true);
	a.push_back(true);
	a.push_back(true);
	a.push_back(false);
	a.push_back(true);
	a.set(1, true);
	a.set(7, true);
#ifdef FS_TEST_FAILURES
	a.set(10, true);
	a.set(16, true);
#endif
	const bool popped = a.pop_back();
	const bool valueAt2 = a.get(2);
	const bool valueAt3 = a.get(3);
	const bool valueAt4 = a.get(4);
#ifdef FS_TEST_FAILURES
	const bool valueAt5 = a.get(5);
#endif
	
#if defined FS_TEST_PERFORMANCE
	{
#if defined FS_DEBUG
		static constexpr uint32 kCount = 20'000'000;
#else
		static constexpr uint32 kCount = 200'000'000;
#endif

		std::vector<uint8> sourceData;
		sourceData.resize(kCount);

		std::vector<uint8> byteVector;
		std::vector<uint8> byteVectorCopy;
		byteVector.resize(kCount);
		byteVectorCopy.resize(kCount);

		std::vector<bool> boolVector;
		std::vector<bool> boolVectorCopy;
		boolVector.resize(kCount);
		boolVectorCopy.resize(kCount);

		BitVector bitVector;
		BitVector bitVectorCopy;
		bitVector.resizeBitCount(kCount);
		bitVectorCopy.resizeBitCount(kCount);

		{
			for (uint32 i = 0; i < kCount; ++i)
			{
				sourceData[i] = i % 2;
			}
		}
		
		{
			fs::Profiler::ScopedCpuProfiler profiler{ "1) byte vector" };
			for (uint32 i = 0; i < kCount; ++i)
			{
				byteVector[i] = sourceData[i];
			}
		}

		{
			fs::Profiler::ScopedCpuProfiler profiler{ "1) byte vector copy" };
			for (uint32 i = 0; i < kCount; ++i)
			{
				byteVectorCopy[i] = byteVector[i];
			}
		}
		
		{
			fs::Profiler::ScopedCpuProfiler profiler{ "2) bool vector" };
			for (uint32 i = 0; i < kCount; ++i)
			{
				boolVector[i] = sourceData[i];
			}
		}

		{
			fs::Profiler::ScopedCpuProfiler profiler{ "2) bool vector copy" };
			for (uint32 i = 0; i < kCount; ++i)
			{
				boolVectorCopy[i] = boolVector[i];
			}
		}

		{
			fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector raw" };
			for (uint32 i = 0; i < kCount; ++i)
			{
				bitVector.set(i, sourceData[i]);
			}
		}

		{
			fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector raw copy" };
			for (uint32 i = 0; i < kCount; ++i)
			{
				bitVectorCopy.set(i, bitVector.get(i));
			}
		}

		{
			fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #1" };
			const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
			for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
			{
				const uint32 sourceAt = byteAt * kBitsPerByte;
				for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
				{
					bitVector.set(byteAt, bitOffset, sourceData[sourceAt + bitOffset]);
				}
			}
		}

		{
			fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #1 copy" };
			const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
			for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
			{
				const uint32 sourceAt = byteAt * kBitsPerByte;
				for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
				{
					bitVectorCopy.set(byteAt, bitOffset, bitVector.get(sourceAt + bitOffset));
				}
			}
		}

		{
			fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2" };
			const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
			for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
			{
				const uint32 sourceAt = byteAt * kBitsPerByte;
				uint8 destByteData = bitVector.getByte(byteAt);
				for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
				{
					BitVector::setBit(destByteData, bitOffset, sourceData[sourceAt + bitOffset]);
				}
				bitVector.setByte(byteAt, destByteData);
			}
		}

		{
			fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2 copy per bit" };
			const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
			for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
			{
				const uint8 srcByteData = bitVector.getByte(byteAt);
				uint8 destByteData = bitVectorCopy.getByte(byteAt);
				for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
				{
					BitVector::setBit(destByteData, bitOffset, BitVector::getBit(srcByteData, bitOffset));
				}
				bitVectorCopy.setByte(byteAt, srcByteData);
			}
		}

		{
			fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2 copy per byte" };
			const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
			for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
			{
				const uint8 byteData = bitVector.getByte(byteAt);
				bitVectorCopy.setByte(byteAt, byteData);
			}
		}

		std::vector<fs::Profiler::ScopedCpuProfiler::Log> logArray = fs::Profiler::ScopedCpuProfiler::getEntireLogArray();
		const bool isEmpty = logArray.empty();
	}
#endif

	return true;
}

const bool testMemoryAllocator()
{
	{
		struct ForTest
		{
			ForTest() : _name{ '\0' }, _value{ 0 } { __noop; }
			ForTest(const char name, const int16 value) :_name{ name }, _value{ value } { printf("ForTest[%c] ctor\n", _name); }
			~ForTest() { printf("ForTest[%c] dtor\n", _name); }
			char	_name;
			int16	_value;
		};

		fs::Memory::Allocator<ForTest>& memoryAllocator2 = fs::Memory::Allocator<ForTest>::getInstance();
		fs::Memory::Accessor maa = memoryAllocator2.allocate('a', 1);
		const bool isMaaValid0 = maa.isValid();
		fs::Memory::Accessor mab = memoryAllocator2.allocate('b', 2);
		{
			const fs::Memory::Accessor mab1 = mab;
			auto mab1raw = mab1.getMemory();
			maa.setMemory(mab1raw);
		}
		memoryAllocator2.deallocate(maa);
		memoryAllocator2.deallocate(mab);
		const bool isMaaValid1 = maa.isValid();
		fs::Memory::Accessor mac = memoryAllocator2.allocate('c', 3);
		fs::Memory::Accessor mad = memoryAllocator2.allocateArray(5, 'd', 4);
		memoryAllocator2.reallocateArray(mad, 5, true);
	}

	{
		fs::Memory::Allocator<char>& memoryAllocator2 = fs::Memory::Allocator<char>::getInstance();
		fs::Memory::Accessor a = memoryAllocator2.allocateArray(5);
		a.setMemory("abcd", 5);
	}

#if defined FS_TEST_PERFORMANCE
	{
#if defined FS_DEBUG
		static constexpr uint32 kCount = 10'000;
#else
		static constexpr uint32 kCount = 30'000;
#endif
		fs::Profiler::ScopedCpuProfiler profiler{ "Vector of DynamicStringA" };
		
		fs::Vector<fs::DynamicStringA> vec;
		for (uint32 i = 0; i < kCount; ++i)
		{
			vec.push_back("abcd");
		}
	}

	auto logArray = fs::Profiler::ScopedCpuProfiler::getEntireLogArray();
	const bool isEmpty = logArray.empty();
#endif

	return true;
}

const bool testStringTypes()
{
#pragma region ScopeString
	using fs::ScopeStringA;
	{
		ScopeStringA<256> a{ "abcd" };
		ScopeStringA<256> b = a;
		b += b;
		b += b;
		b += b;
		b = "abcdefgh";
		a = b.substr(3);
		const uint32 found0 = b.rfind("def");
		const uint32 found1 = b.rfind("fgh");
		const uint32 found2 = b.rfind("ghi");
		const uint32 found3 = b.rfind("abc");
		const uint32 found4 = b.rfind("abcdc");
		const uint32 found5 = b.rfind("zab");
		if (b == "abcdefgh")
		{
			a = b;
		}
		if (a == b)
		{
			b = "YEAH!";
		}
	}
#pragma endregion

#pragma region UniqueString
	using fs::UniqueStringA;
	using fs::UniqueStringAId;
	{
		UniqueStringA a{ "ab" };
		UniqueStringA b{ "cdef" };
		UniqueStringA c{ "ab" };
		UniqueStringA d{ "" };
		UniqueStringA e{ nullptr };
		UniqueStringA f;
		UniqueStringA g = d;
		UniqueStringA h{ b };
		g = a;
		a = b;
		const bool cmp0 = (a == b);
	}
#pragma endregion

#pragma region DynamicString
	using fs::DynamicStringA;
	{
		DynamicStringA a;
		a.append("abcdefg hijklmnopqrst");
		a.append("HELLO!!!!?");

		DynamicStringA b = "ABCDEFG!";
		b.assign("haha..");
		a = b;
		const bool cmp0 = (a == b);
		a = a.substr(0, 5);
		const bool cmp1 = (a == b);
		a.assign("AGAIN");
		b.clear();
		b.append("Hello World!");
		const bool cmp2 = b.compare("llo", fs::StringRange(2, 3));

		DynamicStringA c = b.substr(100);
		const bool cEmpty = c.empty();
		const uint32 foundO0 = b.find("o", 6);
		const uint32 foundO1 = b.find("o", 20);
		const uint32 foundO2 = b.rfind("o", 6);
		const uint32 foundH0 = b.find("H", 3);
		const uint32 foundH1 = b.rfind("H", 11);
		const uint32 foundD0 = b.find("d");
		const uint32 foundD1 = b.rfind("d");
		const uint64 hashA = a.hash();
		const uint64 hashB = b.hash();
		const uint64 hashC = c.hash();
		c.assign("wow");
		c.setChar(0, 'k');
		c.setChar(1, 'j');
		const char getChar = c.getChar(10);

		DynamicStringA from_value0 = DynamicStringA::from_value<float>(1.23f);
		DynamicStringA from_value1 = DynamicStringA::from_value<bool>(true);
		DynamicStringA from_value2 = DynamicStringA::from_value<uint32>(3294967295);

		const float to_value0 = DynamicStringA::to_float(from_value0);
		const bool to_value1 = DynamicStringA::to_bool(from_value1);
		const uint32 to_value2 = DynamicStringA::to_uint32(from_value2);
	}

#if defined FS_TEST_PERFORMANCE
	static constexpr uint32 kCount = 20'000;
	{
		fs::Profiler::ScopedCpuProfiler profiler{ "fs::Vector<fs::DynamicStringA>" };

		fs::Vector<fs::DynamicStringA> dnsArray;
		dnsArray.resize(kCount);
		for (uint32 i = 0; i < kCount; ++i)
		{
			dnsArray.set(i, "abcdefg");
		}
	}

	{
		fs::Profiler::ScopedCpuProfiler profiler{ "std::vector<fs::DynamicStringA>" };

		std::vector<fs::DynamicStringA> dnsArray;
		dnsArray.resize(kCount);
		for (uint32 i = 0; i < kCount; ++i)
		{
			dnsArray[i] = "abcdefg";
		}
	}

	{
		fs::Profiler::ScopedCpuProfiler profiler{ "fs::Vector<std::string>" };

		fs::Vector<std::string> dnsArray;
		dnsArray.resize(kCount);
		for (uint32 i = 0; i < kCount; ++i)
		{
			dnsArray.set(i, "abcdefg");
		}
	}

	{
		fs::Profiler::ScopedCpuProfiler profiler{ "std::vector<std::string>" };

		std::vector<std::string> sArray;
		sArray.resize(kCount);
		for (uint32 i = 0; i < kCount; ++i)
		{
			sArray[i] = "abcdefg";
		}
	}

	auto logArray = fs::Profiler::ScopedCpuProfiler::getEntireLogArray();
	const bool isEmpty = logArray.empty();
#endif
#pragma endregion

	return true;
}

const bool testVector()
{
	fs::Vector<uint32> a(5);
	a.push_back(1);
	a.push_back(2);
	a.push_back(3);

#if defined FS_TEST_FAILURES
	a.set(12, 3);
#endif

	a.insert(2, 5);
	a.erase(1);
	fs::Vector<uint32> b(20);
	b.push_back(9);
	return true;
}

const bool testStringUtil()
{
	const fs::DynamicStringA testA{ "ab c   def g" };
	fs::Vector<fs::DynamicStringA> testATokenized;
	fs::StringUtil::tokenize(testA, ' ', testATokenized);
	
	fs::DynamicStringA testB{
		R"(
			#include <ShaderStructDefinitions>
			#include <VsConstantBuffers>

			VS_OUTPUT main(VS_INPUT input)
			{
				VS_OUTPUT result;
				result._position	= mul(float4(input._position.xyz, 1.0), _cbProjectionMatrix);
				result._color		= input._color;
				result._texCoord	= input._texCoord;
				result._flag		= input._flag;
				return result;
			}
		)"
	};
	const fs::Vector<char> delimiterArray{ ' ', '\t', '\n' };
	fs::Vector<fs::DynamicStringA> testBTokenized;
	fs::StringUtil::tokenize(testB, delimiterArray, testBTokenized);

	return true;
}

const bool testTree()
{
	fs::Tree<fs::DynamicStringA> stringTree;
	fs::TreeNodeAccessor rootNode = stringTree.createRootNode("ROOT");
	
	fs::TreeNodeAccessor a = rootNode.insertChildNode("A");
	const fs::DynamicStringA& aData = a.getNodeData();
	
	fs::TreeNodeAccessor b = a.insertChildNode("b");
	fs::TreeNodeAccessor c = a.insertChildNode("c");

	fs::TreeNodeAccessor d = rootNode.insertChildNode("D");

	//stringTree.swapNodeData(a, b);
	b.moveToParent(rootNode);
	fs::TreeNodeAccessor bParent = b.getParentNode();
	const uint32 aChildCount = a.getChildNodeCount();
	a.clearChildNodes();
#if defined FS_TEST_FAILURES
	fs::TreeNodeAccessor aInvalidChild = a.getChildNode(10);
#endif

	fs::TreeNodeAccessor found = stringTree.findNode(rootNode, "A");

	//stringTree.eraseChildNode(rootNode, a);
	//stringTree.clearChildren(rootNode);
	stringTree.destroyRootNode();

#if defined FS_TEST_FAILURES
	stringTree.moveToParent(rootNode, d);
#endif

	return true;
}

const bool testMemoryAllocator2()
{
	{
		struct TestStruct
		{
			int32				_id = 0;
			fs::Vector<float>	_vec;
		};

		fs::Vector<TestStruct> a;
		a.resize(10);
	}

	{
		fs::Memory::Viewer<fs::DynamicStringA> viewer;
		fs::Memory::Viewer<fs::DynamicStringA> viewer1;
		{
			fs::Memory::Owner<fs::DynamicStringA> abc{ "ABC" };
			viewer = abc;
			
			fs::Memory::ScopedViewer scopedViewer = viewer.viewDataSafe();
			const fs::DynamicStringA& viewerData = *scopedViewer;

			const bool viewerAlive = viewer.isAlive();
			FS_LOG("김장원", (true == viewerAlive) ? "true" : "false");

			viewer1 = viewer;
			fs::Memory::Viewer<fs::DynamicStringA> viewer2 = viewer1;
		}
		const bool viewerAlive1 = viewer.isAlive();
		FS_LOG("김장원", (true == viewerAlive1) ? "true" : "false");
	}

	{
		fs::Memory::Owner<fs::DynamicStringA> abc = fs::DynamicStringA("ABC");

		fs::Memory::Viewer<fs::DynamicStringA> viewer;
		{
			viewer = abc;
			const fs::DynamicStringA& viewerData = viewer.viewData();

			fs::Memory::Viewer<fs::DynamicStringA> viewer1 = viewer;
		}
	}

	{
		fs::Memory::Owner<int32> a;
#if defined FS_TEST_FAILURES
		a.accessData() = 5;
#else
		if (a.isValid() == false)
		{
			a = fs::Memory::Owner<int32>(5);
			int32 aData = a.viewData();
			int32& aData1 = a.accessData();
			aData1 = 7;
		}
#endif

		fs::Memory::Owner<int32> b = 11;
	}

	{
		fs::Memory::Viewer<fs::DynamicStringA> viewer;
		fs::Memory::Owner<fs::DynamicStringA> ownerCopy;
		{
			fs::Memory::Owner<fs::DynamicStringA> owner{ "ABC" };
			viewer = owner;

			fs::Memory::ScopedViewer scopedViewer = viewer.viewDataSafe();
			owner.~Owner();

			ownerCopy = fs::DynamicStringA(scopedViewer.viewData());
		}
	}

	if (false)
	{
		std::chrono::steady_clock clock;
		const uint64 startTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch()).count();
		fs::Vector<fs::DynamicStringA> dsv;
		for (uint32 i = 0; i < 10'000; ++i)
		{
			dsv.push_back(fs::DynamicStringA("abcd"));
		}
		const uint64 endTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch()).count();
		const uint64 duration = endTime - startTime;
		FS_LOG("김장원", "fs::Vector<fs::DynamicString>::push_back() X 10,000 times - duration: %llu ms", duration);
	}

	return true;
}

const bool testFiles()
{
	static constexpr const char* const kFileName = "FsLibraryTest/test.bin";
	static constexpr const char* const kRawString = "abc";
	fs::BinaryFileWriter bfw;
	bfw.write(3.14f);
	bfw.write(true);
	bfw.write(static_cast<uint16>(0xABCD));
	bfw.write("hello");
	bfw.write(kRawString);
	bfw.save(kFileName);

	fs::BinaryFileReader bfr;
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

	fs::TextFileReader tfr;
	tfr.open("FsLibraryTest/test.cpp");

	return true;
}

const bool testLanguage()
{
	using namespace fs;
	
	TextFileReader textFileReader;
	textFileReader.open("FsLibrary/SimpleRendering/CppHlslStructs.h");
	
	Language::CppHlslLexer cppHlslLexer{ textFileReader.get() };
	cppHlslLexer.execute();
	
	Language::CppHlslParser cppHlslParser{ cppHlslLexer };
	cppHlslParser.execute();

	struct TestStruct
	{
		fs::float1 _a			= 1.0f;								// v[0]
		fs::float1 _b			= 2.0f;
		fs::float2 _padding0	= fs::float2(4.0f, 8.0f);
		fs::float3 _c			= fs::float3(16.0f, 32.0f, 64.0f);	// v[1]
		fs::float1 _padding1	= 128.0f;
	};
	fs::CppHlsl::VS_INPUT vsInput;
	uint64 a = sizeof(fs::CppHlsl::VS_INPUT);
	uint64 b = sizeof(fs::CppHlsl::VS_OUTPUT);
	uint64 c = sizeof(fs::CppHlsl::CB_Transforms);
	TestStruct ts;
	uint64 tss = sizeof(TestStruct);

	std::string syntaxTreeString = cppHlslParser.getSyntaxTreeString();
	const fs::Language::CppHlslTypeInfo& typeInfo0 = cppHlslParser.getTypeInfo("VS_INPUT");
	const fs::Language::CppHlslTypeInfo& typeInfo1 = cppHlslParser.getTypeInfo(1);
	const fs::Language::CppHlslTypeInfo& typeInfo2 = cppHlslParser.getTypeInfo(2);
	return true;
}

const bool testWindow()
{
	using namespace fs;
	using namespace fs::Window;

	CreationData windowCreationData;
	windowCreationData._style = Style::Default;
	windowCreationData._position.set(200, 100);
	windowCreationData._size.set(800, 600);
	windowCreationData._title = L"HI";
	windowCreationData._bgColor.set(0.4f, 0.6f, 1.0f);

	WindowsWindow window;
	if (window.create(windowCreationData) == false)
	{
		CreationError error = window.getCreationError();
		return false;
	}

	SimpleRendering::GraphicDevice graphicDevice;
	graphicDevice.initialize(&window);

	while (window.isRunning() == true)
	{
		if (window.hasEvent() == true)
		{
			EventData event = window.popEvent();
			if (event._type == EventType::KeyDown)
			{
				if (event._data._keyCode == EventData::KeyCode::Escape)
				{
					window.destroy();
				}
				else if (event._data._keyCode == EventData::KeyCode::Left)
				{
					const Int2& pos = window.position();
					window.position(pos - Int2(5, 0));
				}
				else if (event._data._keyCode == EventData::KeyCode::Right)
				{
					const Int2& pos = window.position();
					window.position(pos + Int2(5, 0));
				}
			}
		}

		graphicDevice.beginRendering();
		{
			fs::SimpleRendering::RectangleRenderer& rectangleRenderer = graphicDevice.getRectangleRenderer();

			rectangleRenderer.setPosition(fs::Float3(0, 0, 0));
			rectangleRenderer.setSize(fs::Float2(400.0f, 300.0f));
			rectangleRenderer.setColor({ fs::Float4(0.5f, 0.5f, 0.5f, 1.0f), fs::Float4(0.5f, 1.0f, 0.5f, 1.0f) });
			rectangleRenderer.drawColored();

			rectangleRenderer.setPosition(fs::Float3(400.f, 300.f, 0));
			rectangleRenderer.setSize(fs::Float2(400.f, 300.f));
			rectangleRenderer.setColor(fs::Float4(1, 0, 0, 1));
			rectangleRenderer.drawColoredTextured(fs::Float2(0, 0), fs::Float2(1, 1));
		}
		{
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = graphicDevice.getShapeRenderer();
			
			shapeRenderer.setPosition(fs::Float3(400.0f, 300.0f, 0.0f));
			
			shapeRenderer.setColor(fs::Float4(0.0f, 0.5f, 0.5f, 1.0f));
			shapeRenderer.drawRoundedRectangle(fs::Int2(210, 110), 0.5f);

			shapeRenderer.setColor(fs::Float4(0.0f, 0.25f, 0.5f, 1.0f));
			shapeRenderer.drawRoundedRectangle(fs::Int2(200, 100), 0.5f);
		}
		graphicDevice.endRendering();
	}
	return true;
}

const bool testAll()
{
	//fs::Logger::setOutputFileName("LOG.txt");

/*
*/
	testIntTypes();

	testFloatTypes();

	testStaticArray();

	testStackHolder();

	testBitVector();

	testMemoryAllocator();


	/*
	*/
	testStringTypes();

	testVector();

	testStringUtil();

	testTree();

	testMemoryAllocator2();

	FS_LOG("김장원", "LOG %d", 1234);
	//FS_ASSERT("김장원", false, "ASSERTION");
	//FS_LOG_ERROR("김장원", "ERROR");

	/*
	*/
	testFiles();

	testLanguage();

	return true;
}

int main()
{
#ifdef FS_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#if defined FS_TEST_PERFORMANCE
	//testMemoryAllocator();
	testBitVector();
	//testStringTypes();
#else
	testAll();
#endif
	
	testWindow();

	return 0;
}
