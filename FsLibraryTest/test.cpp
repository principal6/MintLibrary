#include <FsLibrary/Include/FsLibrary.h>


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
    
#if defined FS_TEST_FAILURES
    Float2 t;
    t[3] = 1.0f;
#endif
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
            char    _name;
            int16    _value;
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
        fs::Profiler::ScopedCpuProfiler profiler{ "ContiguousVector of ContiguousStringA" };
        
        fs::ContiguousVector<fs::ContiguousStringA> vec;
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
    using fs::ContiguousStringA;
    {
        ContiguousStringA a;
        a.append("abcdefg hijklmnopqrst");
        a.append("HELLO!!!!?");

        ContiguousStringA b = "ABCDEFG!";
        b.assign("haha..");
        a = b;
        const bool cmp0 = (a == b);
        a = a.substr(0, 5);
        const bool cmp1 = (a == b);
        a.assign("AGAIN");
        b.clear();
        b.append("Hello World!");
        const bool cmp2 = b.compare("llo", fs::StringRange(2, 3));

        ContiguousStringA c = b.substr(100);
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

        ContiguousStringA from_value0 = ContiguousStringA::from_value<float>(1.23f);
        ContiguousStringA from_value1 = ContiguousStringA::from_value<bool>(true);
        ContiguousStringA from_value2 = ContiguousStringA::from_value<uint32>(3294967295);

        const float to_value0 = ContiguousStringA::to_float(from_value0);
        const bool to_value1 = ContiguousStringA::to_bool(from_value1);
        const uint32 to_value2 = ContiguousStringA::to_uint32(from_value2);
    }

#if defined FS_TEST_PERFORMANCE
    static constexpr uint32 kCount = 20'000;
    {
        fs::Profiler::ScopedCpuProfiler profiler{ "fs::ContiguousVector<fs::ContiguousStringA>" };

        fs::ContiguousVector<fs::ContiguousStringA> dnsArray;
        dnsArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            dnsArray.set(i, "abcdefg");
        }
    }

    {
        fs::Profiler::ScopedCpuProfiler profiler{ "std::vector<fs::ContiguousStringA>" };

        std::vector<fs::ContiguousStringA> dnsArray;
        dnsArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            dnsArray[i] = "abcdefg";
        }
    }

    {
        fs::Profiler::ScopedCpuProfiler profiler{ "fs::ContiguousVector<std::string>" };

        fs::ContiguousVector<std::string> dnsArray;
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
    fs::ContiguousVector<uint32> a(5);
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);

#if defined FS_TEST_FAILURES
    a.set(12, 3);
#endif

    a.insert(2, 5);
    a.erase(1);
    fs::ContiguousVector<uint32> b(20);
    b.push_back(9);
    return true;
}

const bool testStringUtil()
{
    const fs::ContiguousStringA testA{ "ab c   def g" };
    fs::ContiguousVector<fs::ContiguousStringA> testATokenized;
    fs::StringUtil::tokenize(testA, ' ', testATokenized);
    
    fs::ContiguousStringA testB{
        R"(
            #include <ShaderStructDefinitions>
            #include <VsConstantBuffers>

            VS_OUTPUT_COLOR main(VS_INPUT_COLOR input)
            {
                VS_OUTPUT_COLOR result = (VS_OUTPUT_COLOR)0;
                result._position    = mul(float4(input._position.xyz, 1.0), _cb2DProjectionMatrix);
                result._color       = input._color;
                result._texCoord    = input._texCoord;
                result._flag        = input._flag;
                return result;
            }
        )"
    };
    const fs::ContiguousVector<char> delimiterArray{ ' ', '\t', '\n' };
    fs::ContiguousVector<fs::ContiguousStringA> testBTokenized;
    fs::StringUtil::tokenize(testB, delimiterArray, testBTokenized);

    return true;
}

const bool testTree()
{
    fs::Tree<fs::ContiguousStringA> stringTree;
    fs::TreeNodeAccessor rootNode = stringTree.createRootNode("ROOT");
    
    fs::TreeNodeAccessor a = rootNode.insertChildNode("A");
    const fs::ContiguousStringA& aData = a.getNodeData();
    
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
            int32                           _id = 0;
            fs::ContiguousVector<float>     _vec;
        };

        fs::ContiguousVector<TestStruct>    a;
        a.resize(10);
    }

    {
        fs::Memory::Viewer<fs::ContiguousStringA> viewer;
        fs::Memory::Viewer<fs::ContiguousStringA> viewer1;
        {
            fs::Memory::Owner<fs::ContiguousStringA> abc{ "ABC" };
            viewer = abc;
            
            fs::Memory::ScopedViewer scopedViewer = viewer.viewDataSafe();
            const fs::ContiguousStringA& viewerData = *scopedViewer;

            const bool viewerAlive = viewer.isAlive();
            FS_LOG("김장원", (true == viewerAlive) ? "true" : "false");

            viewer1 = viewer;
            fs::Memory::Viewer<fs::ContiguousStringA> viewer2 = viewer1;
        }
        const bool viewerAlive1 = viewer.isAlive();
        FS_LOG("김장원", (true == viewerAlive1) ? "true" : "false");
    }

    {
        fs::Memory::Owner<fs::ContiguousStringA> abc = fs::ContiguousStringA("ABC");

        fs::Memory::Viewer<fs::ContiguousStringA> viewer;
        {
            viewer = abc;
            const fs::ContiguousStringA& viewerData = viewer.viewData();

            fs::Memory::Viewer<fs::ContiguousStringA> viewer1 = viewer;
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
        fs::Memory::Viewer<fs::ContiguousStringA> viewer;
        fs::Memory::Owner<fs::ContiguousStringA> ownerCopy;
        {
            fs::Memory::Owner<fs::ContiguousStringA> owner{ "ABC" };
            viewer = owner;

            fs::Memory::ScopedViewer scopedViewer = viewer.viewDataSafe();
            owner.~Owner();

            ownerCopy = fs::ContiguousStringA(scopedViewer.viewData());
        }
    }

    if (false)
    {
        std::chrono::steady_clock clock;
        const uint64 startTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch()).count();
        fs::ContiguousVector<fs::ContiguousStringA> dsv;
        for (uint32 i = 0; i < 10'000; ++i)
        {
            dsv.push_back(fs::ContiguousStringA("abcd"));
        }
        const uint64 endTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch()).count();
        const uint64 duration = endTime - startTime;
        FS_LOG("김장원", "fs::ContiguousVector<fs::DynamicString>::push_back() X 10,000 times - duration: %llu ms", duration);
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
    textFileReader.open("Assets/CppHlsl/CppHlslStreamData.h");
    
    Language::CppHlslLexer cppHlslLexer{ textFileReader.get() };
    cppHlslLexer.execute();
    
    Language::CppHlslParser cppHlslParser{ cppHlslLexer };
    cppHlslParser.execute();

    struct TestStruct
    {
        fs::float1 _a           = 1.0f;                             // v[0]
        fs::float1 _b           = 2.0f;
        fs::float2 _padding0    = fs::float2(4.0f, 8.0f);
        fs::float3 _c           = fs::float3(16.0f, 32.0f, 64.0f);  // v[1]
        fs::float1 _padding1    = 128.0f;
    };
    fs::RenderingBase::VS_INPUT_SHAPE vsInput;
    uint64 a = sizeof(fs::RenderingBase::VS_INPUT_SHAPE);
    uint64 b = sizeof(fs::RenderingBase::VS_OUTPUT_SHAPE);
    uint64 c = sizeof(fs::RenderingBase::CB_View);
    TestStruct ts;
    uint64 tss = sizeof(TestStruct);

    std::string syntaxTreeString = cppHlslParser.getSyntaxTreeString();
    const fs::Language::CppHlslTypeInfo& typeInfo0 = cppHlslParser.getTypeInfo("VS_INPUT_SHAPE");
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
    windowCreationData._size.set(1024, 768);
    windowCreationData._title = L"HI";
    windowCreationData._bgColor.set(0.875f, 0.875f, 0.875f);

    WindowsWindow window;
    if (window.create(windowCreationData) == false)
    {
        CreationError error = window.getCreationError();
        return false;
    }

    RenderingBase::GraphicDevice graphicDevice;
    graphicDevice.initialize(&window);

    Rendering::MeshRenderer meshRenderer{ &graphicDevice };
    meshRenderer.initialize();
    
    Rendering::ObjectPool objectPool;
    Rendering::Object* const testObject = objectPool.createObject();
    Rendering::CameraObject* const testCameraObject = objectPool.createCameraObject();
    fs::Float2 windowSize = graphicDevice.getWindowSizeFloat2();
    testCameraObject->setPerspectiveScreenRatio(windowSize._x / windowSize._y);
    {
        testObject->attachComponent(objectPool.createMeshComponent());

        fs::Rendering::TransformComponent* transformComponent = static_cast<fs::Rendering::TransformComponent*>(testObject->getComponent(fs::Rendering::ObjectComponentType::TransformComponent));
        transformComponent->_srt._translation._z = 4.0f;
        //transformComponent->_srt._rotation.setAxisAngle(fs::Float3(1.0f, 1.0f, 0.0f), fs::Math::kPiOverEight);
    }
    testCameraObject->rotatePitch(0.125f);

    uint64 previousFrameTimeMs = 0;
    while (window.isRunning() == true)
    {
        const uint64 loopStartTimeMs = fs::Profiler::getCurrentTimeMs();
        fs::Gui::GuiContext& guiContext = graphicDevice.getGuiContext();

        // Events
        {
            guiContext.handleEvents(&window);

            if (window.hasEvent() == true)
            {
                EventData event = window.popEvent();
                if (event._type == EventType::KeyDown)
                {
                    if (event._value.isKeyCode(EventData::KeyCode::Enter) == true)
                    {
                        graphicDevice.getShaderPool().recompileAllShaders();
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Num1) == true)
                    {
                        graphicDevice.useSolidCullBackRasterizer();
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Num2) == true)
                    {
                        graphicDevice.useWireFrameCullBackRasterizer();
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Num3) == true)
                    {
                        graphicDevice.useWireFrameNoCullingRasterizer();
                    }
                }
                else if (event._type == EventType::MouseMoveDelta)
                {
                    if (window.isMouseDown(fs::Window::EventData::MouseButton::Right) == true)
                    {
                        const fs::Float2& mouseDeltaPosition = event._value.getAndClearMouseDeltaPosition();
                        testCameraObject->rotatePitch(mouseDeltaPosition._y);
                        testCameraObject->rotateYaw(mouseDeltaPosition._x);
                    }
                }
                else if (event._type == EventType::WindowResized)
                {
                    graphicDevice.updateScreenSize();
                    guiContext.updateScreenSize(graphicDevice.getWindowSizeFloat2());
                    objectPool.updateScreenSize(graphicDevice.getWindowSizeFloat2());
                }
            }
        }

        // Dynamic Keyboard Inputs
        if (guiContext.isFocusedControlTextBox() == false)
        {
            if (window.isKeyDown(EventData::KeyCode::W) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Forward);
            }

            if (window.isKeyDown(EventData::KeyCode::S) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Backward);
            }

            if (window.isKeyDown(EventData::KeyCode::A) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Leftward);
            }

            if (window.isKeyDown(EventData::KeyCode::D) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Rightward);
            }
        }

        // Rendering
        {
            graphicDevice.beginRendering();
            {
                static fs::Gui::VisibleState testWindowVisibleState = fs::Gui::VisibleState::Invisible;
                guiContext.testWindow(testWindowVisibleState);
                if (guiContext.beginMenuBar(L"MainMenuBar") == true)
                {
                    if (guiContext.beginMenuBarItem(L"파일") == true)
                    {
                        if (guiContext.beginMenuItem(L"종료") == true)
                        {
                            if (guiContext.isControlPressed() == true)
                            {
                                window.destroy();
                            }
                            guiContext.endMenuItem();
                        }
                        guiContext.endMenuBarItem();
                    }

                    if (guiContext.beginMenuBarItem(L"윈도우") == true)
                    {
                        if (guiContext.beginMenuItem(L"TestWindow") == true)
                        {
                            if (guiContext.isControlPressed() == true)
                            {
                                testWindowVisibleState = fs::Gui::VisibleState::VisibleOpen;
                            }
                            
                            guiContext.endMenuItem();
                        }

                        guiContext.endMenuBarItem();
                    }

                    guiContext.endMenuBar();
                }

                fs::Gui::WindowParam inspectorWindowParam;
                inspectorWindowParam._size = fs::Float2(300.0f, 400.0f);
                inspectorWindowParam._position = fs::Float2(20.0f, 50.0f);
                inspectorWindowParam._initialDockingMethod = fs::Gui::DockingMethod::RightSide;
                inspectorWindowParam._initialDockingSize._x = 240.0f;
                static fs::Gui::VisibleState inspectorVisibleState;
                if (guiContext.beginWindow(L"Inspector", inspectorWindowParam, inspectorVisibleState) == true)
                {
                    fs::Gui::LabelParam fpsLabelParam;
                    fpsLabelParam._fontColor = fs::RenderingBase::Color(200, 220, 255, 255);
                    fpsLabelParam._alignmentHorz = fs::Gui::TextAlignmentHorz::Left;
                    guiContext.pushLabel(L"FPS_Label", (L" FPS: " + std::to_wstring(fs::Profiler::FpsCounter::getFps())).c_str(), fpsLabelParam);
                    guiContext.pushLabel(L"CPU_Label", (L" CPU: " + std::to_wstring(previousFrameTimeMs) + L" ms").c_str(), fpsLabelParam);
                
                    guiContext.endWindow();
                }
            }

            const uint64 loopEndTimeMs = fs::Profiler::getCurrentTimeMs();
            previousFrameTimeMs = loopEndTimeMs - loopStartTimeMs;

            graphicDevice.setViewMatrix(testCameraObject->getViewMatrix());
            graphicDevice.setProjectionMatrix(testCameraObject->getProjectionMatrix());
            graphicDevice.updateViewProjectionMatrix();

            meshRenderer.render(objectPool);

            graphicDevice.endRendering();
        }
        
        fs::Profiler::FpsCounter::count();
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

    fs::FsLibraryVersion::printVersion();

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
