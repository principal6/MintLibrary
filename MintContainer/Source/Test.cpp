#include <MintContainer/Include/Test.h>

#include <MintContainer/Include/AllHeaders.h>
#include <MintContainer/Include/AllHpps.h>

#include <MintCommon/Include/ScopedCPUProfiler.h>


#pragma optimize("", off)

//#define MINT_TEST_PERFORMANCE


namespace mint
{
	template <typename T>
	class Teller
	{
		static constexpr uint32 kClassNameBufferSize = 260;

	public:
		Teller(const char* const className) : _className{}, _value{} { ::strcpy_s(_className, className); MINT_LOG_UNTAGGED("%s - %s", _className, "Ctor [default]"); }
		Teller(const char* const className, const T& value) : _className{}, _value{ value } { ::strcpy_s(_className, className); MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Ctor [with initialization]", value); }
		Teller(const Teller& rhs) { ::strcpy_s(_className, rhs._className); _value = rhs._value; MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Ctor [copy]", _value); }
		Teller(Teller&& rhs) noexcept { ::strcpy_s(_className, rhs._className); _value = std::move(rhs._value); MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Ctor [move]", _value); }
		~Teller() { MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Dtor", _value); ::memset(_className, 0, kClassNameBufferSize); }

	public:
		Teller& operator=(const Teller& rhs)
		{
			if (this != &rhs)
			{
				::strcpy_s(_className, rhs._className);
				_value = rhs._value;
				MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Assign [Copy]", _value);
			}
			return *this;
		}
		Teller& operator=(Teller&& rhs) noexcept
		{
			if (this != &rhs)
			{
				::strcpy_s(_className, rhs._className);
				_value = std::move(rhs._value);
				MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Assign [Move]", _value);
			}
			return *this;
		}

	private:
		char    _className[kClassNameBufferSize];
		T       _value;
	};


	template <typename T>
	class Notable
	{
		static constexpr const char* const kClassName = "Notable";

	public:
		Notable() : _value{}, _teller{ kClassName, _value } { __noop; }
		Notable(const T value) : _value{ value }, _teller{ kClassName, _value } { __noop; }
		Notable(const Notable& rhs) = default;
		Notable(Notable&& rhs) noexcept = default;
		~Notable() = default;

	public:
		Notable& operator=(const Notable& rhs) = default;
		Notable& operator=(Notable&& rhs) noexcept = default;

	private:
		T           _value;
		Teller<T>   _teller;
	};


	template <typename T>
	class Uncopiable
	{
		static constexpr const char* const kClassName = "Uncopiable";

	public:
		Uncopiable() : _teller{ kClassName } { __noop; }
		Uncopiable(const Notable<T> notable) : _notable{ notable }, _teller{ kClassName, notable } { __noop; }
		Uncopiable(const Uncopiable& rhs) = delete;
		Uncopiable(Uncopiable&& rhs) noexcept = default;
		~Uncopiable() = default;

	public:
		Uncopiable& operator=(const Uncopiable& rhs) = delete;
		Uncopiable& operator=(Uncopiable&& rhs) noexcept = default;

	private:
		Teller<T>   _teller;
		Notable<T>  _notable;
	};


	template <typename T>
	class Unmovable
	{
		static constexpr const char* const kClassName = "Unmovable";

	public:
		Unmovable() : _teller{ kClassName } { __noop; }
		Unmovable(const Notable<T> notable) : _notable{ notable }, _teller{ kClassName, notable } { __noop; }
		Unmovable(const Unmovable& rhs) = default;
		Unmovable(Unmovable&& rhs) noexcept = delete;
		~Unmovable() = default;

	public:
		Unmovable& operator=(const Unmovable& rhs) = default;
		Unmovable& operator=(Unmovable&& rhs) noexcept = delete;

	private:
		Teller<T>   _teller;
		Notable<T>  _notable;
	};


	namespace TestContainers
	{
		bool test()
		{
			MINT_ASSURE(test_Array());
			MINT_ASSURE(test_BitArray());
			MINT_ASSURE(test_StackHolder());
			MINT_ASSURE(test_BitVector());
			MINT_ASSURE(test_Vector());
			MINT_ASSURE(test_HashMap());
			MINT_ASSURE(test_StringTypes());
			MINT_ASSURE(test_StringUtil());
			MINT_ASSURE(test_Queue());
			MINT_ASSURE(test_Tree());
			return true;
		}

		bool test_Array()
		{
			constexpr Array<int32, 3> arr{ 4, 5, 999 };
			return true;
		}

		bool test_BitArray()
		{
			BitArray<3> bitArray0(true);
			MINT_ASSURE(bitArray0.getBitCount() == 3);
			MINT_ASSURE(bitArray0.getByteCount() == 1);
			bitArray0.setByte(0, 0xFF);
			bitArray0.setAll(false);
			bitArray0.set(0, true);
			bitArray0.set(3, true);
			MINT_ASSURE(bitArray0.get(3) == false);
			bitArray0.setUnsafe(5, true);
			MINT_ASSURE(bitArray0.getUnsafe(5) == true);
			MINT_ASSURE(bitArray0.get(5) == false);

			BitArray<33> bitArray1(true);
			MINT_ASSURE(bitArray1.getBitCount() == 33);
			MINT_ASSURE(bitArray1.getByteCount() == 5);
			return true;
		}

		bool test_BitVector()
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
					Profiler::ScopedCPUProfiler profiler{ "1) byte vector" };
					for (uint32 i = 0; i < kCount; ++i)
					{
						byteVector[i] = sourceData[i];
					}
				}

				{
					Profiler::ScopedCPUProfiler profiler{ "1) byte vector copy" };
					for (uint32 i = 0; i < kCount; ++i)
					{
						byteVectorCopy[i] = byteVector[i];
					}
				}

				{
					Profiler::ScopedCPUProfiler profiler{ "2) bool vector" };
					for (uint32 i = 0; i < kCount; ++i)
					{
						boolVector[i] = sourceData[i];
					}
				}

				{
					Profiler::ScopedCPUProfiler profiler{ "2) bool vector copy" };
					for (uint32 i = 0; i < kCount; ++i)
					{
						boolVectorCopy[i] = boolVector[i];
					}
				}

				{
					Profiler::ScopedCPUProfiler profiler{ "3) bit vector raw" };
					for (uint32 i = 0; i < kCount; ++i)
					{
						bitVector.set(i, sourceData[i]);
					}
				}

				{
					Profiler::ScopedCPUProfiler profiler{ "3) bit vector raw copy" };
					for (uint32 i = 0; i < kCount; ++i)
					{
						bitVectorCopy.set(i, bitVector.get(i));
					}
				}

				{
					Profiler::ScopedCPUProfiler profiler{ "3) bit vector per byte #1" };
					const uint32 kByteCount = BitVector::computeByteCount(kCount);
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
					Profiler::ScopedCPUProfiler profiler{ "3) bit vector per byte #1 copy" };
					const uint32 kByteCount = BitVector::computeByteCount(kCount);
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
					Profiler::ScopedCPUProfiler profiler{ "3) bit vector per byte #2" };
					const uint32 kByteCount = BitVector::computeByteCount(kCount);
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
					Profiler::ScopedCPUProfiler profiler{ "3) bit vector per byte #2 copy per bit" };
					const uint32 kByteCount = BitVector::computeByteCount(kCount);
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
					Profiler::ScopedCPUProfiler profiler{ "3) bit vector per byte #2 copy per byte" };
					const uint32 kByteCount = BitVector::computeByteCount(kCount);
					for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
					{
						const uint8 byteData = bitVector.getByte(byteAt);
						bitVectorCopy.setByte(byteAt, byteData);
					}
				}

				std::vector<Profiler::ScopedCPUProfiler::Log> logArray = Profiler::ScopedCPUProfiler::getEntireLogArray();
				const bool isEmpty = logArray.empty();
			}
#endif
			return true;
		}

		bool test_StackHolder()
		{
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

		bool test_Vector()
		{
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

			// Move semantic 점검!
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

		bool test_HashMap()
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

		bool test_StringTypes()
		{
#pragma region StackString
			StackStringA<256> ss0{ "abcd" };
			StackStringA<256> ss1 = ss0;
			{
				ss1 += ss1;
				ss1 += ss1;
				ss1 += ss1;
				ss1 += "test";
				ss1 = "abcdefgh";
				ss0 = ss1.substr(3);
				MINT_ASSURE(ss1.rfind("def") == 3);
				MINT_ASSURE(ss1.rfind("fgh") == 5);
				MINT_ASSURE(ss1.rfind("ghi") == kStringNPos);
				MINT_ASSURE(ss1.rfind("abc") == 0);
				MINT_ASSURE(ss1.rfind("abcdc") == kStringNPos);
				MINT_ASSURE(ss1.rfind("zab") == kStringNPos);
				MINT_ASSURE(ss1 == "abcdefgh");
				ss0 = ss1;
				MINT_ASSURE(ss0 == ss1);
			}
			//StackStringU8<256> ss2{ u8"abc가나다123!@#" };
			//MINT_ASSURE(ss2.rfind(u8"abc") == 11);
#pragma endregion

#if defined MINT_TEST_PERFORMANCE
			static constexpr uint32 kCount = 20'000;
			{
				Profiler::ScopedCPUProfiler profiler{ "Vector<std::string>" };

				Vector<std::string> dnsArray;
				dnsArray.resize(kCount);
				for (uint32 i = 0; i < kCount; ++i)
				{
					dnsArray[i] = "abcdefg";
				}
			}

			{
				Profiler::ScopedCPUProfiler profiler{ "Vector<std::string>" };

				Vector<std::string> dnsArray;
				dnsArray.resize(kCount);
				for (uint32 i = 0; i < kCount; ++i)
				{
					dnsArray[i] = "abcdefg";
				}
			}

			{
				Profiler::ScopedCPUProfiler profiler{ "Vector<std::string>" };

				Vector<std::string> dnsArray;
				dnsArray.resize(kCount);
				for (uint32 i = 0; i < kCount; ++i)
				{
					dnsArray[i] = "abcdefg";
				}
			}

			{
				Profiler::ScopedCPUProfiler profiler{ "Vector<std::string>" };

				Vector<std::string> sArray;
				sArray.resize(kCount);
				for (uint32 i = 0; i < kCount; ++i)
				{
					sArray[i] = "abcdefg";
				}
			}

			auto logArray = Profiler::ScopedCPUProfiler::getEntireLogArray();
			const bool isEmpty = logArray.empty();
#endif
#pragma endregion

#pragma region String
			StringA sa0 = StringA("Hello, there! My friend!!!");
			sa0 += "I'm testing...";

			StringA saSmall0 = StringA("abcde");
			saSmall0.resize(3);
			saSmall0 += "+testing...";
			saSmall0 += "+testing..!";
			saSmall0.clear();

			StringA sa1 = StringA("Hello, there! My friend!!!") + " This is test for append function!";

			StringW sw0 = StringW(L"Hello, there!");
			sw0 += L" I'm testing...";

			StringW sw1 = StringW(L"TestStringW1");
			sw1 += L" ";
			sw1 += sw0;

			StringW sw2 = sw1.substr(4, 6);

			const uint32 found = sw1.find(L"Str", 3);
			sw1.resize(3);
			sw1.resize(10, 'z');
			const uint64 hash = sw1.computeHash();

			const bool cmp0 = (sw2 == sw1);
			const bool cmp1 = (sw2 == StringW(L"String"));
#pragma endregion

#pragma region StringView
			StringView sv0{ ss0 };
			StringView sv1{ sa0 };
			StringView sv2{ sw0 };
#pragma endregion

			{
				String<char> string{ "abc" };
				StackString<char, 256> stackString{ "abc" };
				StringReference<char> literalString{ "abc" };

				MutableString<char>& a = string;
				MutableString<char>& b = stackString;
				StringReference<char>& c = literalString;
				//MINT_LOG("a: %s, capacity: %d", a.c_str(), a.capacity());
				//MINT_LOG("b: %s, capacity: %d", b.c_str(), b.capacity());
				//MINT_LOG("c: %s", c.c_str());
				MINT_ASSURE(a == b);
				MINT_ASSURE(a == c);
				MINT_ASSURE(b == c);

				a.clear();
				b.clear();
				MINT_ASSURE(a.length() == 0);
				MINT_ASSURE(b.length() == 0);
				MINT_ASSURE(a == b);

				a.assign("efg");
				b.assign("hij");
				MINT_ASSURE(a != b);

				a.append("life!");
				b.append("life?");
				MINT_ASSURE(a != b);
				//MINT_LOG("a: %s, capacity: %d", a.c_str(), a.capacity());
				//MINT_LOG("b: %s, capacity: %d", b.c_str(), b.capacity());

				MINT_ASSURE(a.contains("life!!!") == false);
				MINT_ASSURE(a.contains("life!") == true);
				MINT_ASSURE(a.contains("fe!") == true);
				MINT_ASSURE(b.contains("life???") == false);
				MINT_ASSURE(b.contains("life?") == true);
				MINT_ASSURE(b.contains("fe?") == true);

				//MINT_LOG("hash of a: %llX", a.computeHash());
				//MINT_LOG("hash of b: %llX", b.computeHash());
			}

			return true;
		}

		bool test_StringUtil()
		{
			{
				const std::string testA{ "ab c   def g" };
				Vector<std::string> testATokens;
				StringUtil::tokenize(testA, ' ', testATokens);

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
				const Vector<char> delimiters{ ' ', '\t', '\n' };
				Vector<std::string> testBTokens;
				StringUtil::tokenize(testB, delimiters, testBTokens);
			}

			{
				static_assert(StringUtil::length("abc") == 3);
				static_assert(StringUtil::length("가나다라") == 8);
				static_assert(StringUtil::length("韓國") == 4);

				static_assert(StringUtil::length(L"abc") == 3);
				static_assert(StringUtil::length(L"가나다라") == 4);
				static_assert(StringUtil::length(L"韓國") == 2);

				static_assert(StringUtil::length(u8"abc") == 3);
				static_assert(StringUtil::length(u8"가나다라") == 12);
				static_assert(StringUtil::length(u8"韓國") == 6);
			}

			{
				static_assert(StringUtil::countChars("abc") == 3);
				static_assert(StringUtil::countChars("가나다라") == 4);
				static_assert(StringUtil::countChars("韓國") == 2);

				static_assert(StringUtil::countChars(L"abc") == 3);
				static_assert(StringUtil::countChars(L"가나다라") == 4);
				static_assert(StringUtil::countChars(L"韓國") == 2);

				static_assert(StringUtil::countChars(u8"abc") == 3);
				static_assert(StringUtil::countChars(u8"가나다라") == 4);
				static_assert(StringUtil::countChars(u8"韓國") == 2);
			}

			static_assert(StringUtil::countBytesInCharCode(0b11111111) == 4);
			static_assert(StringUtil::countBytesInCharCode(0b11110000) == 4);
			static_assert(StringUtil::countBytesInCharCode(0b11101111) == 3);
			static_assert(StringUtil::countBytesInCharCode(0b11100000) == 3);
			static_assert(StringUtil::countBytesInCharCode(0b11011111) == 2);
			static_assert(StringUtil::countBytesInCharCode(0b11000000) == 2);
			static_assert(StringUtil::countBytesInCharCode(0b01111111) == 1);
			static_assert(StringUtil::countBytesInCharCode(0b00000000) == 1);

			{
				static_assert(StringUtil::find("abcde", "abc") != kStringNPos);
				static_assert(StringUtil::find("abc가나다라def", "나다") != kStringNPos);
				static_assert(StringUtil::find("abc가나다라def", "나다", 4) != kStringNPos);
				static_assert(StringUtil::find("abc가나다라def", "나다", 6) == kStringNPos);
				static_assert(StringUtil::find("abc가나다라def", "나다", 100) == kStringNPos);

				static_assert(StringUtil::find(L"abcde", L"abc") != kStringNPos);
				static_assert(StringUtil::find(L"abc가나다라def", L"나다") != kStringNPos);
				static_assert(StringUtil::find(L"abc가나다라def", L"나다", 4) != kStringNPos);
				static_assert(StringUtil::find(L"abc가나다라def", L"나다", 5) == kStringNPos);
				static_assert(StringUtil::find(L"abc가나다라def", L"나다", 100) == kStringNPos);

				static_assert(StringUtil::find(u8"abcde", u8"abc") != kStringNPos);
				static_assert(StringUtil::find(u8"abc가나다라def", u8"나다") != kStringNPos);
				static_assert(StringUtil::find(u8"abc가나다라def", u8"나다", 4) != kStringNPos);
				static_assert(StringUtil::find(u8"abc가나다라def", u8"나다", 7) == kStringNPos);
				static_assert(StringUtil::find(u8"abc가나다라def", u8"나다", 100) == kStringNPos);
			}

			{
				constexpr const char* a = nullptr;
				constexpr const char* b = nullptr;
				static_assert(StringUtil::compare(a, b));
				static_assert(StringUtil::compare(a, "abc") == false);
				static_assert(StringUtil::compare("abc", b) == false);
				static_assert(StringUtil::compare("abc", "abc"));
				static_assert(StringUtil::compare("abc", "abcd") == false);
				static_assert(StringUtil::compare("abcd", "abc") == false);

				constexpr const wchar_t* c = nullptr;
				constexpr const wchar_t* d = nullptr;
				static_assert(StringUtil::compare(c, d));
				static_assert(StringUtil::compare(c, L"abc") == false);
				static_assert(StringUtil::compare(L"abc", d) == false);
				static_assert(StringUtil::compare(L"abc", L"abc"));
				static_assert(StringUtil::compare(L"abc", L"abcd") == false);
				static_assert(StringUtil::compare(L"abcd", L"abc") == false);

				constexpr const char8_t* e = nullptr;
				constexpr const char8_t* f = nullptr;
				static_assert(StringUtil::compare(e, f));
				static_assert(StringUtil::compare(e, u8"abc") == false);
				static_assert(StringUtil::compare(u8"abc", f) == false);
				static_assert(StringUtil::compare(u8"abc", u8"abc"));
				static_assert(StringUtil::compare(u8"abc", u8"abcd") == false);
				static_assert(StringUtil::compare(u8"abcd", u8"abc") == false);
			}

			{
				constexpr uint32 kSize = 256;
				char bufferA[kSize]{};
				wchar_t bufferB[kSize]{};
				char8_t bufferC[kSize]{};
				StringUtil::copy(bufferA, "가나다라");
				StringUtil::copy(bufferA, nullptr);
				StringUtil::copy(bufferB, L"가나다라");
				StringUtil::copy(bufferB, nullptr);
				StringUtil::copy(bufferC, u8"가나다라");
				StringUtil::copy(bufferC, nullptr);

				char bufferD[4]{};
				//StringUtil::copy(bufferD, "가나다라");
				wchar_t bufferE[4]{};
				//StringUtil::copy(bufferE, L"가나다라");
				char8_t bufferF[4]{};
				StringUtil::copy(bufferF, u8"가나다라");
			}

			static_assert(StringUtil::is7BitASCII(u8"abc"));
			static_assert(StringUtil::is7BitASCII(u8"0.125f"));
			static_assert(StringUtil::is7BitASCII(u8"가나다") == false);
			static_assert(StringUtil::is7BitASCII(u8"韓國") == false);
			{
				static_assert(StringUtil::countChars(u8"가나다abc") == 6);
				U8CharCodeViewer u8CharCodeViewer(u8"가나다");
				for (auto charCode : u8CharCodeViewer)
				{
					const StringU8 u8String = StringUtil::decode(charCode);
					const StringW wideString = StringUtil::convertUTF8ToWideString(u8String);
					StringA string;
					StringUtil::convertWideStringToString(wideString, string);
					MINT_LOG("CONVERTED: %s", string.c_str());
				}
			}

			{
				constexpr uint32 kSize = 256;
				char stringA[kSize]{ "테스트!" };
				wchar_t stringB[kSize]{ L"테스트!" };
				char8_t stringC[kSize]{ u8"테스트!" };
				StringA stringD{ "테스트!" };
				StringW stringE{ L"테스트!" };
				StackStringA<kSize> stringF{ "테스트!" };
				StackStringW<kSize> stringG{ L"테스트!" };
				StackStringU8<kSize> stringH{ u8"테스트!" };

				StringViewA stringViewA{ stringA };
				StringViewW stringViewB{ stringB };
				StringViewU8 stringViewC{ stringC };
				StringViewA stringViewD{ stringD };
				StringViewW stringViewE{ stringE };
				StringViewA stringViewF{ stringF };
				StringViewW stringViewG{ stringG };
				StringViewU8 stringViewH{ stringH };
				MINT_LOG("StrnigH Length:%d, CharCount:%d", stringH.length(), stringH.countChars());
			}

			{
				StackStringA<256> ssa0;
				StackStringW<256> ssw0;
				StackStringU8<256> ssu0;
				StringA stra0;
				StringW strw0;
				StringU8 stru0;
				StringUtil::toString(1234, ssa0);
				StringUtil::toString(1234, stra0);
				StringUtil::toString(1234, ssw0);
				StringUtil::toString(1234, strw0);
				StringUtil::toString(1234, ssu0);
				StringUtil::toString(1234, stru0);

				StringUtil::toString(12.34f, ssa0);
				StringUtil::toString(12.34f, stra0);
				StringUtil::toString(12.34f, ssw0);
				StringUtil::toString(12.34f, strw0);
				StringUtil::toString(12.34f, ssu0);
				StringUtil::toString(12.34f, stru0);

				const int32 i32_0 = StringUtil::stringToInt32(StringReference("12345"));
				const int64 i64_0 = StringUtil::stringToInt64(StringReference("1234567890"));
				const float f0 = StringUtil::stringToFloat(StringReference("12345"));
				const double d0 = StringUtil::stringToDouble(StringReference("1234567890"));
			}

			return true;
		}

		bool test_Queue()
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

		bool test_Tree()
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
	}
}
