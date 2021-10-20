#include <MintLibraryTest/TestContainers.h>

#include <MintContainer/Include/AllHpps.h>


namespace mint
{
    namespace TestContainers
    {
        static const bool testQueue() noexcept
        {
            using Type = int32;
            //using Type = Notable<int32>;

            Queue<Type> queue;
            queue.push(0);
            queue.push(1);
            queue.push(2);
            queue.push(3);
            queue.push(4);
            queue.push(5);
            queue.push(6);
            queue.push(7);
            
            queue.pop();

            queue.push(8);

            queue.pop();
            queue.pop();

            queue.push(9);

            return true;
        }

        static const bool testVector() noexcept
        {
            using namespace TestContainers;

            using Type = int32;
            //using Type = Notable<int32>;
            {
                Type notableA(11);
                Type notableB = std::move(notableA);
            }
            {
                std::vector<Type> strVector;
                strVector.reserve(5);
                strVector.push_back(0);
                strVector.push_back(1);
                strVector.push_back(2);
                strVector.push_back(3);
                //strVector.erase(t.begin() + 1);
                strVector.insert(strVector.begin() + 1, 4);

                Vector<Type> mintVector;
                mintVector.reserve(5);
                mintVector.push_back(0);
                mintVector.push_back(1);
                mintVector.push_back(2);
                mintVector.push_back(3);
                //mintVector.erase(1);
                mintVector.insert(4, 1);
            }


            Vector<uint32> a(5);
            a.push_back(1);
            a.push_back(2);
            a.push_back(3);
            a.insert(2, 5);
            a.erase(1);

            Vector<uint32> b(20);
            b.push_back(9);

            // Move semantic Á¡°Ë!
            std::swap(a, b);

            Vector<uint32> c(3);
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

        static const bool testStaticArray() noexcept
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

            return true;
        }

        static const bool testStackHolder() noexcept
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

            return true;
        }

        static const bool testBitVector() noexcept
        {
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

                Vector<uint8> sourceData;
                sourceData.resize(kCount);

                Vector<uint8> byteVector;
                Vector<uint8> byteVectorCopy;
                byteVector.resize(kCount);
                byteVectorCopy.resize(kCount);

                Vector<bool> boolVector;
                Vector<bool> boolVectorCopy;
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
                    Profiler::ScopedCpuProfiler profiler{ "1) byte vector" };
                    for (uint32 i = 0; i < kCount; ++i)
                    {
                        byteVector[i] = sourceData[i];
                    }
                }

                {
                    Profiler::ScopedCpuProfiler profiler{ "1) byte vector copy" };
                    for (uint32 i = 0; i < kCount; ++i)
                    {
                        byteVectorCopy[i] = byteVector[i];
                    }
                }

                {
                    Profiler::ScopedCpuProfiler profiler{ "2) bool vector" };
                    for (uint32 i = 0; i < kCount; ++i)
                    {
                        boolVector[i] = sourceData[i];
                    }
                }

                {
                    Profiler::ScopedCpuProfiler profiler{ "2) bool vector copy" };
                    for (uint32 i = 0; i < kCount; ++i)
                    {
                        boolVectorCopy[i] = boolVector[i];
                    }
                }

                {
                    Profiler::ScopedCpuProfiler profiler{ "3) bit vector raw" };
                    for (uint32 i = 0; i < kCount; ++i)
                    {
                        bitVector.set(i, sourceData[i]);
                    }
                }

                {
                    Profiler::ScopedCpuProfiler profiler{ "3) bit vector raw copy" };
                    for (uint32 i = 0; i < kCount; ++i)
                    {
                        bitVectorCopy.set(i, bitVector.get(i));
                    }
                }

                {
                    Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #1" };
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
                    Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #1 copy" };
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
                    Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2" };
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
                    Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2 copy per bit" };
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
                    Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2 copy per byte" };
                    const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
                    for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
                    {
                        const uint8 byteData = bitVector.getByte(byteAt);
                        bitVectorCopy.setByte(byteAt, byteData);
                    }
                }

                Vector<Profiler::ScopedCpuProfiler::Log> logArray = Profiler::ScopedCpuProfiler::getEntireLogArray();
                const bool isEmpty = logArray.empty();
            }
#endif

            return true;
        }

        static const bool testHashMap() noexcept
        {
            HashMap<std::string, std::string> hashMap;
            hashMap.insert("1", "a");
            hashMap.insert("5", "b");
            hashMap.insert("11", "c");
            hashMap.insert("21", "d");
            hashMap.insert("33", "e");
            hashMap.insert("41", "f");

            KeyValuePair<std::string, std::string> keyValuePair0 = hashMap.find("1");

            hashMap.erase("21");

            KeyValuePair<std::string, std::string> keyValuePair1 = hashMap.find("21");

            return true;
        }

        static const bool testStringTypes() noexcept
        {
#pragma region ScopeString
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
                Profiler::ScopedCpuProfiler profiler{ "Vector<std::string>" };

                Vector<std::string> dnsArray;
                dnsArray.resize(kCount);
                for (uint32 i = 0; i < kCount; ++i)
                {
                    dnsArray.set(i, "abcdefg");
                }
            }

            {
                Profiler::ScopedCpuProfiler profiler{ "Vector<std::string>" };

                Vector<std::string> dnsArray;
                dnsArray.resize(kCount);
                for (uint32 i = 0; i < kCount; ++i)
                {
                    dnsArray[i] = "abcdefg";
                }
            }

            {
                Profiler::ScopedCpuProfiler profiler{ "Vector<std::string>" };

                Vector<std::string> dnsArray;
                dnsArray.resize(kCount);
                for (uint32 i = 0; i < kCount; ++i)
                {
                    dnsArray.set(i, "abcdefg");
                }
            }

            {
                Profiler::ScopedCpuProfiler profiler{ "Vector<std::string>" };

                Vector<std::string> sArray;
                sArray.resize(kCount);
                for (uint32 i = 0; i < kCount; ++i)
                {
                    sArray[i] = "abcdefg";
                }
            }

            auto logArray = Profiler::ScopedCpuProfiler::getEntireLogArray();
            const bool isEmpty = logArray.empty();
#endif
#pragma endregion

            StringA stringA = StringA("Hello, there! My friend!!!");
            stringA += "I'm testing...";

            StringA stringSmallA = StringA("abcde");
            stringSmallA.resize(3);
            stringSmallA += "+testing...";
            stringSmallA += "+testing..!";
            stringSmallA.clear();

            StringW stringW = StringW(L"Hello, there!");
            stringW += L" I'm testing...";

            StringW stringW1 = StringW(L"TestStringW1");
            stringW1 += L" ";
            stringW1 += stringW;

            StringW ss = stringW1.substr(4, 6);

            const uint32 found = stringW1.find(L"Str", 3);
            stringW1.resize(3);
            stringW1.resize(10, 'z');
            const uint64 hash = stringW1.computeHash();

            const bool cmp0 = (ss == stringW1);
            const bool cmp1 = (ss == StringW(L"String"));

            return true;
        }

        static const bool testStringUtil() noexcept
        {
            const std::string testA{ "ab c   def g" };
            Vector<std::string> testATokenized;
            StringUtil::tokenize(testA, ' ', testATokenized);

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
            const Vector<char> delimiterArray{ ' ', '\t', '\n' };
            Vector<std::string> testBTokenized;
            StringUtil::tokenize(testB, delimiterArray, testBTokenized);

            return true;
        }

        static const bool testTree() noexcept
        {
            Tree<std::string> stringTree;
            TreeNodeAccessor rootNode = stringTree.createRootNode("ROOT");

            TreeNodeAccessor a = rootNode.insertChildNode("A");
            const std::string& aData = a.getNodeData();

            TreeNodeAccessor b = a.insertChildNode("b");
            TreeNodeAccessor c = a.insertChildNode("c");

            TreeNodeAccessor d = rootNode.insertChildNode("D");

            //stringTree.swapNodeData(a, b);
            b.moveToParent(rootNode);
            TreeNodeAccessor bParent = b.getParentNode();
            const uint32 aChildCount = a.getChildNodeCount();
            a.clearChildNodes();
#if defined MINT_TEST_FAILURES
            TreeNodeAccessor aInvalidChild = a.getChildNode(10);
#endif

            TreeNodeAccessor found = stringTree.findNode(rootNode, "A");

            //stringTree.eraseChildNode(rootNode, a);
            //stringTree.clearChildren(rootNode);
            stringTree.destroyRootNode();

#if defined MINT_TEST_FAILURES
            stringTree.moveToParent(rootNode, d);
#endif

            return true;
        }

        const bool testAll() noexcept
        {
            MINT_ASSURE(testStaticArray());
            MINT_ASSURE(testStackHolder());
            MINT_ASSURE(testBitVector());
            MINT_ASSURE(testHashMap());
            MINT_ASSURE(testStringTypes());
            MINT_ASSURE(testVector());
            MINT_ASSURE(testQueue());
            MINT_ASSURE(testStringUtil());
            MINT_ASSURE(testTree());

            return true;
        }
    }
}
