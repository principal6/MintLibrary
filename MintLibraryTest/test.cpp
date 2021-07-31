#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>


#ifdef MINT_DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif


#pragma comment(lib, "MintLibrary.lib")


//#define MINT_TEST_FAILURES
//#define MINT_TEST_PERFORMANCE


void testIntTypes()
{
    using namespace mint;
    Int2 ni;
    Int2 a{ 1, 2 };
    Int2 b{ 3, 4 };
    Int2 c = a + b;
    Int2 d;
    d = a;
}

void testFloatTypes()
{
    using namespace mint;
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
    
#if defined MINT_TEST_FAILURES
    Float2 t;
    t[3] = 1.0f;
#endif
}

void testStaticArray()
{
    using namespace mint;
    constexpr StaticArray<int32, 3> arr{ 4, 5, 999 };

    StaticBitArray<3> ba(true);
    const uint32 bitCount = ba.getBitCount();
    const uint32 byteCount = ba.getByteCount();
    ba.setByte(0, 0xFF);
    ba.setAll(false);
    ba.set(0, true);
    ba.set(3, true);
    ba.setUnsafe(5, true);
    const bool test0 = ba.get(0);
    const bool test1 = ba.get(1);
    const bool test3 = ba.get(3);
    const bool test5 = ba.getUnsafe(5);

    return;
}

void testStackHolder()
{
    using namespace mint;
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
#ifdef MINT_TEST_FAILURES
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
    using mint::BitVector;

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
#ifdef MINT_TEST_FAILURES
    a.set(10, true);
    a.set(16, true);
#endif
    const bool popped = a.pop_back();
    const bool valueAt2 = a.get(2);
    const bool valueAt3 = a.get(3);
    const bool valueAt4 = a.get(4);
#ifdef MINT_TEST_FAILURES
    const bool valueAt5 = a.get(5);
#endif
    
#if defined MINT_TEST_PERFORMANCE
    {
#if defined MINT_DEBUG
        static constexpr uint32 kCount = 20'000'000;
#else
        static constexpr uint32 kCount = 200'000'000;
#endif

        mint::Vector<uint8> sourceData;
        sourceData.resize(kCount);

        mint::Vector<uint8> byteVector;
        mint::Vector<uint8> byteVectorCopy;
        byteVector.resize(kCount);
        byteVectorCopy.resize(kCount);

        mint::Vector<bool> boolVector;
        mint::Vector<bool> boolVectorCopy;
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
            mint::Profiler::ScopedCpuProfiler profiler{ "1) byte vector" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                byteVector[i] = sourceData[i];
            }
        }

        {
            mint::Profiler::ScopedCpuProfiler profiler{ "1) byte vector copy" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                byteVectorCopy[i] = byteVector[i];
            }
        }
        
        {
            mint::Profiler::ScopedCpuProfiler profiler{ "2) bool vector" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                boolVector[i] = sourceData[i];
            }
        }

        {
            mint::Profiler::ScopedCpuProfiler profiler{ "2) bool vector copy" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                boolVectorCopy[i] = boolVector[i];
            }
        }

        {
            mint::Profiler::ScopedCpuProfiler profiler{ "3) bit vector raw" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                bitVector.set(i, sourceData[i]);
            }
        }

        {
            mint::Profiler::ScopedCpuProfiler profiler{ "3) bit vector raw copy" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                bitVectorCopy.set(i, bitVector.get(i));
            }
        }

        {
            mint::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #1" };
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
            mint::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #1 copy" };
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
            mint::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2" };
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
            mint::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2 copy per bit" };
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
            mint::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2 copy per byte" };
            const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
            for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
            {
                const uint8 byteData = bitVector.getByte(byteAt);
                bitVectorCopy.setByte(byteAt, byteData);
            }
        }

        mint::Vector<mint::Profiler::ScopedCpuProfiler::Log> logArray = mint::Profiler::ScopedCpuProfiler::getEntireLogArray();
        const bool isEmpty = logArray.empty();
    }
#endif

    return true;
}

const bool testHashMap()
{
    mint::HashMap<std::string, std::string> hashMap;
    hashMap.insert("1", "a");
    hashMap.insert("5", "b");
    hashMap.insert("11", "c");
    hashMap.insert("21", "d");
    hashMap.insert("33", "e");
    hashMap.insert("41", "f");

    mint::KeyValuePair<std::string, std::string> keyValuePair0 = hashMap.find("1");
    
    hashMap.erase("21");

    mint::KeyValuePair<std::string, std::string> keyValuePair1 = hashMap.find("21");

    return true;
}

const bool testStringTypes()
{
#pragma region ScopeString
    using mint::ScopeStringA;
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
    using mint::UniqueStringA;
    using mint::UniqueStringAId;
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

#if defined MINT_TEST_PERFORMANCE
    static constexpr uint32 kCount = 20'000;
    {
        mint::Profiler::ScopedCpuProfiler profiler{ "mint::Vector<std::string>" };

        mint::Vector<std::string> dnsArray;
        dnsArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            dnsArray.set(i, "abcdefg");
        }
    }

    {
        mint::Profiler::ScopedCpuProfiler profiler{ "mint::Vector<std::string>" };

        mint::Vector<std::string> dnsArray;
        dnsArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            dnsArray[i] = "abcdefg";
        }
    }

    {
        mint::Profiler::ScopedCpuProfiler profiler{ "mint::Vector<std::string>" };

        mint::Vector<std::string> dnsArray;
        dnsArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            dnsArray.set(i, "abcdefg");
        }
    }

    {
        mint::Profiler::ScopedCpuProfiler profiler{ "mint::Vector<std::string>" };

        mint::Vector<std::string> sArray;
        sArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            sArray[i] = "abcdefg";
        }
    }

    auto logArray = mint::Profiler::ScopedCpuProfiler::getEntireLogArray();
    const bool isEmpty = logArray.empty();
#endif
#pragma endregion

    return true;
}

const bool testVector()
{
    mint::Vector<uint32> a(5);
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    a.insert(2, 5);
    a.erase(1);

    mint::Vector<uint32> b(20);
    b.push_back(9);

    // Move semantic 점검!
    std::swap(a, b);

    mint::Vector<uint32> c(3);
    c.insert(3, 10);
    c.insert(2, 0);
    c.insert(99, 1);
    c.insert(1, 0);
    c.insert(0, 100);
    c.erase(100);
    c.erase(2);
    c.shrink_to_fit();

    return true;
}

const bool testStringUtil()
{
    const std::string testA{ "ab c   def g" };
    mint::Vector<std::string> testATokenized;
    mint::StringUtil::tokenize(testA, ' ', testATokenized);
    
    std::string testB{
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
    const mint::Vector<char> delimiterArray{ ' ', '\t', '\n' };
    mint::Vector<std::string> testBTokenized;
    mint::StringUtil::tokenize(testB, delimiterArray, testBTokenized);

    return true;
}

const bool testTree()
{
    mint::Tree<std::string> stringTree;
    mint::TreeNodeAccessor rootNode = stringTree.createRootNode("ROOT");
    
    mint::TreeNodeAccessor a = rootNode.insertChildNode("A");
    const std::string& aData = a.getNodeData();
    
    mint::TreeNodeAccessor b = a.insertChildNode("b");
    mint::TreeNodeAccessor c = a.insertChildNode("c");

    mint::TreeNodeAccessor d = rootNode.insertChildNode("D");

    //stringTree.swapNodeData(a, b);
    b.moveToParent(rootNode);
    mint::TreeNodeAccessor bParent = b.getParentNode();
    const uint32 aChildCount = a.getChildNodeCount();
    a.clearChildNodes();
#if defined MINT_TEST_FAILURES
    mint::TreeNodeAccessor aInvalidChild = a.getChildNode(10);
#endif

    mint::TreeNodeAccessor found = stringTree.findNode(rootNode, "A");

    //stringTree.eraseChildNode(rootNode, a);
    //stringTree.clearChildren(rootNode);
    stringTree.destroyRootNode();

#if defined MINT_TEST_FAILURES
    stringTree.moveToParent(rootNode, d);
#endif

    return true;
}

const bool testFiles()
{
    static constexpr const char* const kFileName = "MintLibraryTest/test.bin";
    static constexpr const char* const kRawString = "abc";
    mint::BinaryFileWriter bfw;
    bfw.write(3.14f);
    bfw.write(true);
    bfw.write(static_cast<uint16>(0xABCD));
    bfw.write("hello");
    bfw.write(kRawString);
    bfw.save(kFileName);

    mint::BinaryFileReader bfr;
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

    mint::TextFileReader tfr;
    tfr.open("MintLibraryTest/test.cpp");

    return true;
}

const bool testLanguage()
{
    using namespace mint;
    
    TextFileReader textFileReader;
    textFileReader.open("Assets/CppHlsl/CppHlslStreamData.h");
    
    CppHlsl::Lexer cppHlslLexer{ textFileReader.get() };
    cppHlslLexer.execute();
    
    CppHlsl::Parser cppHlslParser{ cppHlslLexer };
    cppHlslParser.execute();

    struct TestStruct
    {
        mint::float1 _a           = 1.0f;                             // v[0]
        mint::float1 _b           = 2.0f;
        mint::float2 _padding0    = mint::float2(4.0f, 8.0f);
        mint::float3 _c           = mint::float3(16.0f, 32.0f, 64.0f);  // v[1]
        mint::float1 _padding1    = 128.0f;
    };
    mint::RenderingBase::VS_INPUT_SHAPE vsInput;
    uint64 a = sizeof(mint::RenderingBase::VS_INPUT_SHAPE);
    uint64 b = sizeof(mint::RenderingBase::VS_OUTPUT_SHAPE);
    uint64 c = sizeof(mint::RenderingBase::CB_View);
    TestStruct ts;
    uint64 tss = sizeof(TestStruct);

    //std::string syntaxTreeString = cppHlslParser.getSyntaxTreeString();
    //const mint::CppHlsl::TypeMetaData& typeMetaData0 = cppHlslParser.getTypeMetaData("VS_INPUT_SHAPE");
    //const mint::CppHlsl::TypeMetaData& typeMetaData1 = cppHlslParser.getTypeMetaData(1);
    //const mint::CppHlsl::TypeMetaData& typeMetaData2 = cppHlslParser.getTypeMetaData(2);
    return true;
}

const bool testAlgorithm()
{
    mint::Vector<uint32> a;
    a.push_back(4);
    a.push_back(3);
    a.push_back(0);
    a.push_back(2);
    a.push_back(1);

    mint::quickSort(a, mint::ComparatorAscending<uint32>());
    mint::quickSort(a, mint::ComparatorDescending<uint32>());
    
    return true;
}

//#pragma optimize("", off)
const bool testLinearAlgebra()
{
    mint::Math::VectorR<3> vec0(1.0, 1.0, 0.0);
    vec0 = 5 * vec0;
    mint::Math::VectorR<3> vec1(0.0, 3.0, 0.0);
    mint::Math::VectorR<3> vec2 = vec0.cross(vec1).setNormalized();
    const bool trueValue = vec2.isUnitVector();
    const bool falseValue = mint::Math::equals(1.00002f, 1.0f);
    const double distance = vec1.normalize().distance(vec2);
    const double theta = vec1.angle(vec2);
    const bool orthogonality = vec1.isOrthogonalTo(vec2);

    mint::Math::VectorR<1> vec3(3.0);
    mint::Math::Matrix<1, 3> mat0;
    mat0.setRow(0, mint::Math::VectorR<3>(4.0, 5.0, 6.0));
    constexpr bool isMat0Square = mat0.isSquareMatrix();
    
    mint::Math::Matrix<3, 3> mat1;
    mat1.setRow(0, mint::Math::VectorR<3>(3.0, 0.0, 0.0));
    mat1.setRow(1, mint::Math::VectorR<3>(0.0, 3.0, 0.0));
    mat1.setRow(2, mint::Math::VectorR<3>(0.0, 0.0, 3.0));
    const bool isMat1Scalar = mat1.isScalarMatrix();
    mat1.setIdentity();
    const bool isMat1Identity = mat1.isIdentityMatrix();
    mat1.setZero();
    const bool isMat1Zero = mat1.isZeroMatrix();

    mint::Math::VectorR<3> a = mint::Math::VectorR<3>(1.0, 2.0, 3.0);
    mat1.setRow(0, mint::Math::VectorR<3>(1.0, 2.0, 3.0));
    mat1.setRow(1, mint::Math::VectorR<3>(4.0, 5.0, 6.0));
    mat1.setRow(2, mint::Math::VectorR<3>(7.0, 8.0, 9.0));
    mint::Math::VectorR<3> e1 = mint::Math::VectorR<3>::standardUnitVector(1);
    mint::Math::VectorR<3> row1 = e1 * mat1;
    mint::Math::VectorR<3> col1 = mat1 * e1;
    vec0 = vec3 * mat0;

    mat1.setRow(1, mint::Math::VectorR<3>(2.0, 5.0, 6.0));
    mat1.setRow(2, mint::Math::VectorR<3>(3.0, 6.0, 9.0));
    const bool isMat1Symmetric = mat1.isSymmetricMatrix();

    mat1.setRow(1, mint::Math::VectorR<3>(-2.0, 5.0, 6.0));
    mat1.setRow(2, mint::Math::VectorR<3>(-3.0, -6.0, 9.0));
    const bool isMat1SkewSymmetric = mat1.isSkewSymmetricMatrix();

    mint::Math::Matrix<2, 3> mat2;
    mat2.setRow(0, mint::Math::VectorR<3>(0.0, 1.0, 2.0));
    mat2.setRow(1, mint::Math::VectorR<3>(3.0, 4.0, 5.0));
    mint::Math::Matrix<3, 2> mat2Transpose = mat2.transpose();

    mint::Math::Matrix<2, 2> mat3;
    const bool isMat3Idempotent = mat3.isIdempotentMatrix();

    return true;
}

const bool testWindow()
{
    using namespace mint;
    using namespace mint::Window;

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

    mint::Gui::GuiContext& guiContext = graphicDevice.getGuiContext();

    Rendering::MeshRenderer meshRenderer{ &graphicDevice };
    meshRenderer.initialize();
    
    Rendering::ObjectPool objectPool;
    Rendering::Object* const testObject = objectPool.createObject();
    Rendering::CameraObject* const testCameraObject = objectPool.createCameraObject();
    mint::Float2 windowSize = graphicDevice.getWindowSizeFloat2();
    testCameraObject->setPerspectiveZRange(0.01f, 1000.0f);
    testCameraObject->setPerspectiveScreenRatio(windowSize._x / windowSize._y);
    {
        testObject->attachComponent(objectPool.createMeshComponent());
        testObject->getObjectTransformSrt()._translation._z = 4.0f;
        //testObject->getObjectTransformSrt()._rotation.setAxisAngle(mint::Float3(1.0f, 1.0f, 0.0f), mint::Math::kPiOverEight);
    }
    testCameraObject->rotatePitch(0.125f);
    
    Rendering::InstantRenderer instantRenderer{ &graphicDevice };
    instantRenderer.initialize();

    Game::SkeletonGenerator testSkeletonGenerator;
    mint::Float4x4 testSkeletonWorldMatrix;
    testSkeletonWorldMatrix.setTranslation(1.0f, 0.0f, 4.0f);
    mint::Float4x4 bindPoseLocalMatrix;
    testSkeletonGenerator.createJoint(-1, "Root", bindPoseLocalMatrix);
    bindPoseLocalMatrix.setTranslation(1.0f, 0.0f, 0.0f);
    testSkeletonGenerator.createJoint(0, "Elbow", bindPoseLocalMatrix);
    bindPoseLocalMatrix.setTranslation(1.0f, 0.0f, 0.0f);
    testSkeletonGenerator.createJoint(1, "Tip", bindPoseLocalMatrix);
    testSkeletonGenerator.buildBindPoseModelSpace();
    Game::Skeleton testSkeleton(testSkeletonGenerator);
    
    uint64 previousFrameTimeMs = 0;
    while (window.isRunning() == true)
    {
        objectPool.computeDeltaTime();

        // Events
        {
            guiContext.processEvent(&window);

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
                    else if (event._value.isKeyCode(EventData::KeyCode::Num4) == true)
                    {
                        mint::Rendering::MeshComponent* const meshComponent = static_cast<mint::Rendering::MeshComponent*>(testObject->getComponent(mint::Rendering::ObjectComponentType::MeshComponent));
                        meshComponent->shouldDrawNormals(!meshComponent->shouldDrawNormals());
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Num5) == true)
                    {
                        mint::Rendering::MeshComponent* const meshComponent = static_cast<mint::Rendering::MeshComponent*>(testObject->getComponent(mint::Rendering::ObjectComponentType::MeshComponent));
                        meshComponent->shouldDrawEdges(!meshComponent->shouldDrawEdges());
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Shift) == true)
                    {
                        testCameraObject->setIsBoostMode(true);
                    }
                }
                else if (event._type == EventType::KeyUp)
                {
                    if (event._value.isKeyCode(EventData::KeyCode::Shift) == true)
                    {
                        testCameraObject->setIsBoostMode(false);
                    }
                }
                else if (event._type == EventType::MouseWheel)
                {
                    const float mouseWheel = event._value.getMouseWheel();
                    if (0.0f < mouseWheel)
                    {
                        testCameraObject->increaseMoveSpeed();
                    }
                    else
                    {
                        testCameraObject->decreaseMoveSpeed();
                    }
                }
                else if (event._type == EventType::MouseMoveDelta)
                {
                    if (window.isMouseDown(mint::Window::MouseButton::Right) == true)
                    {
                        const mint::Float2& mouseDeltaPosition = event._value.getAndClearMouseDeltaPosition();
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
        if (guiContext.isFocusedControlInputBox() == false)
        {
            if (window.isKeyDown(EventData::KeyCode::Q) == true)
            {
                testCameraObject->move(mint::Rendering::CameraObject::MoveDirection::Upward);
            }

            if (window.isKeyDown(EventData::KeyCode::E) == true)
            {
                testCameraObject->move(mint::Rendering::CameraObject::MoveDirection::Downward);
            }

            if (window.isKeyDown(EventData::KeyCode::W) == true)
            {
                testCameraObject->move(mint::Rendering::CameraObject::MoveDirection::Forward);
            }

            if (window.isKeyDown(EventData::KeyCode::S) == true)
            {
                testCameraObject->move(mint::Rendering::CameraObject::MoveDirection::Backward);
            }

            if (window.isKeyDown(EventData::KeyCode::A) == true)
            {
                testCameraObject->move(mint::Rendering::CameraObject::MoveDirection::Leftward);
            }

            if (window.isKeyDown(EventData::KeyCode::D) == true)
            {
                testCameraObject->move(mint::Rendering::CameraObject::MoveDirection::Rightward);
            }
        }

        // Rendering
        {
            graphicDevice.beginRendering();

#if 0
            mint::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = graphicDevice.getShapeFontRendererContext();
            mint::Rendering::Plotter plotter(shapeFontRendererContext);
            plotter.xLabel(L"weight");
            plotter.yLabel(L"length");
            
            mint::Vector<float> xData{  1.0f,  2.0f,  4.0f,  8.0f, 16.0f, 32.0f };
            mint::Vector<float> yData{ 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f };
            plotter.plotType(mint::Rendering::Plotter::PlotType::Circle);
            plotter.scatter(xData, yData);

            mint::Vector<float> xData1{ 21.0f, 22.0f, 24.0f, 28.0f, 26.0f, 22.0f };
            mint::Vector<float> yData1{ -2.0f, -3.0f, -8.0f, -1.0f, 50.0f, 30.0f };
            plotter.plotType(mint::Rendering::Plotter::PlotType::Triangle);
            plotter.scatter(xData1, yData1);

            plotter.render();
#endif
#if 1
            {
                static mint::Gui::VisibleState testWindowVisibleState = mint::Gui::VisibleState::Invisible;
                guiContext.testWindow(testWindowVisibleState);
                if (guiContext.beginMenuBar(L"MainMenuBar") == true)
                {
                    if (guiContext.beginMenuBarItem(L"파일") == true)
                    {
                        if (guiContext.beginMenuItem(L"종료") == true)
                        {
                            if (guiContext.isThisControlPressed() == true)
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
                            if (guiContext.isThisControlPressed() == true)
                            {
                                testWindowVisibleState = mint::Gui::VisibleState::VisibleOpen;
                            }
                            
                            guiContext.endMenuItem();
                        }

                        guiContext.endMenuBarItem();
                    }

                    guiContext.endMenuBar();
                }

                mint::Gui::WindowParam inspectorWindowParam;
                inspectorWindowParam._common._size = mint::Float2(320.0f, 400.0f);
                inspectorWindowParam._position = mint::Float2(20.0f, 50.0f);
                inspectorWindowParam._initialDockingMethod = mint::Gui::DockingMethod::RightSide;
                inspectorWindowParam._initialDockingSize._x = 320.0f;
                static mint::Gui::VisibleState inspectorVisibleState;
                if (guiContext.beginWindow(L"Inspector", inspectorWindowParam, inspectorVisibleState) == true)
                {
                    wchar_t tempBuffer[256];
                    mint::Gui::LabelParam labelParam;
                    labelParam._common._fontColor = mint::RenderingBase::Color(200, 220, 255, 255);
                    labelParam._alignmentHorz = mint::Gui::TextAlignmentHorz::Left;
                    
                    mint::formatString(tempBuffer, L" FPS: %d", mint::Profiler::FpsCounter::getFps());
                    guiContext.pushLabel(L"FPS_Label", tempBuffer, labelParam);

                    mint::formatString(tempBuffer, L" CPU: %d ms", mint::Profiler::FpsCounter::getFrameTimeMs());
                    guiContext.pushLabel(L"CPU_Label", tempBuffer, labelParam);
                    
                    mint::Float3& cameraPosition = testCameraObject->getObjectTransformSrt()._translation;
                    mint::formatString(tempBuffer, L" Camera Position:");
                    guiContext.pushLabel(L"Camera Position", tempBuffer, labelParam);
                    
                    {
                        mint::formatString(tempBuffer, L"X");
                        labelParam._alignmentHorz = mint::Gui::TextAlignmentHorz::Center;
                        labelParam._common._backgroundColor.r(1.0f);
                        labelParam._common._backgroundColor.a(0.75f);
                        labelParam._common._fontColor = mint::RenderingBase::Color::kWhite;
                        labelParam._common._size._x = 16.0f;
                        labelParam._common._size._y = 24.0f;
                        labelParam._common._offset._x = 16.0f;
                        guiContext.pushLabel(L"PositionX", tempBuffer, labelParam);

                        guiContext.nextSameLine();
                        guiContext.nextNoInterval();

                        mint::Gui::CommonControlParam commonControlParam;
                        commonControlParam._size._x = 72.0f;
                        commonControlParam._size._y = 24.0f;
                        commonControlParam._offset = labelParam._common._offset;
                        if (guiContext.beginValueSliderFloat(L"PositionX", commonControlParam, 0.0f, 3, cameraPosition._x) == true)
                        {
                            guiContext.endValueSliderFloat();
                        }

                        guiContext.nextSameLine();

                        mint::formatString(tempBuffer, L"Y");
                        labelParam._common._backgroundColor.r(0.0f);
                        labelParam._common._backgroundColor.g(0.875f);
                        guiContext.pushLabel(L"PositionY", tempBuffer, labelParam);

                        guiContext.nextSameLine();
                        guiContext.nextNoInterval();

                        if (guiContext.beginValueSliderFloat(L"PositionY", commonControlParam, 0.0f, 3, cameraPosition._y) == true)
                        {
                            guiContext.endValueSliderFloat();
                        }

                        guiContext.nextSameLine();

                        mint::formatString(tempBuffer, L"Z");
                        labelParam._common._backgroundColor.g(0.0f);
                        labelParam._common._backgroundColor.b(1.0f);
                        guiContext.pushLabel(L"PositionZ", tempBuffer, labelParam);

                        guiContext.nextSameLine();
                        guiContext.nextNoInterval();

                        if (guiContext.beginValueSliderFloat(L"PositionZ", commonControlParam, 0.0f, 3, cameraPosition._z) == true)
                        {
                            guiContext.endValueSliderFloat();
                        }
                    }
                    
                    guiContext.endWindow();
                }
            }

            testSkeleton.renderSkeleton(&instantRenderer, testSkeletonWorldMatrix);

            graphicDevice.setViewMatrix(testCameraObject->getViewMatrix());
            graphicDevice.setProjectionMatrix(testCameraObject->getProjectionMatrix());
            graphicDevice.updateCbView();

            meshRenderer.render(objectPool);
            instantRenderer.render();
#endif

            graphicDevice.endRendering();
        }
        
        mint::Profiler::FpsCounter::count();
    }
    return true;
}

const bool testAll()
{
    //mint::Logger::setOutputFileName("LOG.txt");
/*
*/
    testIntTypes();

    testFloatTypes();

    testStaticArray();

    testStackHolder();

    testBitVector();

    testHashMap();

    /*
    */
    testStringTypes();

    testVector();

    testStringUtil();

    testTree();

    MINT_LOG("김장원", "LOG %d", 1234);
    //MINT_ASSERT("김장원", false, "ASSERTION");
    //MINT_LOG_ERROR("김장원", "ERROR");

    /*
    */
    testFiles();

    testLanguage();

    testAlgorithm();

    testLinearAlgebra();

    return true;
}

int main()
{
#ifdef MINT_DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    mint::MintLibraryVersion::printVersion();

#if defined MINT_DEBUG
    #if defined MINT_TEST_PERFORMANCE
        //testMemoryAllocator();
        testBitVector();
        //testStringTypes();
    #else
        testAll();
        
    #endif
#else
    HWND handleToConsoleWindow = ::GetConsoleWindow();
    ::FreeConsole();
    ::SendMessageW(handleToConsoleWindow, WM_CLOSE, 0, 0);
#endif
    
    testWindow();

    return 0;
}
