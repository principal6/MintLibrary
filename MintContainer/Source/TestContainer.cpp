#include <MintContainer/Include/TestContainer.h>

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
		bool Test()
		{
			MINT_ASSURE(Test_Array());
			MINT_ASSURE(Test_BitArray());
			MINT_ASSURE(Test_StackHolder());
			MINT_ASSURE(Test_BitVector());
			MINT_ASSURE(Test_Vector());
			MINT_ASSURE(Test_HashMap());
			MINT_ASSURE(Test_SharedPtr());
			MINT_ASSURE(Test_StringTypes());
			MINT_ASSURE(Test_StringUtil());
			MINT_ASSURE(Test_Queue());
			MINT_ASSURE(Test_Tree());
			return true;
		}

		bool Test_Array()
		{
			constexpr Array<int32, 3> arr{ 4, 5, 999 };
			return true;
		}

		bool Test_BitArray()
		{
			BitArray<3> bitArray0(true);
			MINT_ASSURE(bitArray0.GetBitCount() == 3);
			MINT_ASSURE(bitArray0.GetByteCount() == 1);
			bitArray0.SetByte(0, 0xFF);
			bitArray0.SetAll(false);
			bitArray0.Set(0, true);
			bitArray0.Set(3, true);
			MINT_ASSURE(bitArray0.Get(3) == false);
			bitArray0.SetUnsafe(5, true);
			MINT_ASSURE(bitArray0.GetUnsafe(5) == true);
			MINT_ASSURE(bitArray0.Get(5) == false);

			BitArray<33> bitArray1(true);
			MINT_ASSURE(bitArray1.GetBitCount() == 33);
			MINT_ASSURE(bitArray1.GetByteCount() == 5);
			return true;
		}

		bool Test_BitVector()
		{
			BitVector a;
			a.ReserveByteCapacity(4);
			a.PushBack(true);
			a.PushBack(false);
			a.PushBack(true);
			a.PushBack(false);
			a.PushBack(true);
			a.PushBack(true);
			a.PushBack(true);
			a.PushBack(false);
			a.PushBack(true);
			a.Set(1, true);
			a.Set(7, true);
#ifdef MINT_TEST_FAILURES
			a.set(10, true);
			a.set(16, true);
#endif
			const bool popped = a.PopBack();
			const bool valueAt2 = a.Get(2);
			const bool valueAt3 = a.Get(3);
			const bool valueAt4 = a.Get(4);
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
				bitVector.ResizeBitCount(kCount);
				bitVectorCopy.ResizeBitCount(kCount);

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
					const uint32 kByteCount = BitVector::ComputeByteCount(kCount);
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
					const uint32 kByteCount = BitVector::ComputeByteCount(kCount);
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
					const uint32 kByteCount = BitVector::ComputeByteCount(kCount);
					for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
					{
						const uint32 sourceAt = byteAt * kBitsPerByte;
						uint8 destByteData = bitVector.GetByte(byteAt);
						for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
						{
							BitVector::SetBit(destByteData, bitOffset, sourceData[sourceAt + bitOffset]);
						}
						bitVector.SetByte(byteAt, destByteData);
					}
				}

				{
					Profiler::ScopedCPUProfiler profiler{ "3) bit vector per byte #2 copy per bit" };
					const uint32 kByteCount = BitVector::ComputeByteCount(kCount);
					for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
					{
						const uint8 srcByteData = bitVector.GetByte(byteAt);
						uint8 destByteData = bitVectorCopy.GetByte(byteAt);
						for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
						{
							BitVector::SetBit(destByteData, bitOffset, BitVector::GetBit(srcByteData, bitOffset));
						}
						bitVectorCopy.SetByte(byteAt, srcByteData);
					}
				}

				{
					Profiler::ScopedCPUProfiler profiler{ "3) bit vector per byte #2 copy per byte" };
					const uint32 kByteCount = BitVector::ComputeByteCount(kCount);
					for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
					{
						const uint8 byteData = bitVector.GetByte(byteAt);
						bitVectorCopy.SetByte(byteAt, byteData);
					}
				}

				std::vector<Profiler::ScopedCPUProfiler::Log> logArray = Profiler::ScopedCPUProfiler::GetEntireLogData();
				const bool IsEmpty = logArray.empty();
			}
#endif
			return true;
		}

		bool Test_StackHolder()
		{
			{
				StackHolder<16, 6> sh;

				byte* shTestA = sh.RegisterSpace(2);
				memcpy(shTestA + (16 * 0), "1__abcdefghijk__", 16);
				memcpy(shTestA + (16 * 1), "2__lmnopqrstuv__", 16);

				byte* shTestB = sh.RegisterSpace(1);
				memcpy(shTestB + (16 * 0), "3__wxyzabcdefg__", 16);

				byte* shTestC = sh.RegisterSpace(1);
				memcpy(shTestC + (16 * 0), "4__helloMyFrie__", 16);

				sh.DeregisterSpace(shTestB);

				byte* shTestD = sh.RegisterSpace(2);
				memcpy(shTestD + (16 * 0), "5__nd!!IAmLege__", 16);
			}
			{
				StackHolder<8, 12> sh;

				byte* shTestA = sh.RegisterSpace(7);
				memcpy(shTestA + (8 * 0), "01_abcd_", 8);
				memcpy(shTestA + (8 * 1), "02_efgh_", 8);
				memcpy(shTestA + (8 * 2), "03_ijkl_", 8);
				memcpy(shTestA + (8 * 3), "04_mnop_", 8);
				memcpy(shTestA + (8 * 4), "05_qrst_", 8);
				memcpy(shTestA + (8 * 5), "06_uvwx_", 8);
				memcpy(shTestA + (8 * 6), "07_yzab_", 8);

				byte* shTestB = sh.RegisterSpace(3);
				memcpy(shTestB + (8 * 0), "08_cdef_", 8);
				memcpy(shTestB + (8 * 1), "09_ghij_", 8);
				memcpy(shTestB + (8 * 2), "10_klmn_", 8);

				sh.DeregisterSpace(shTestB);
			}
#ifdef MINT_TEST_FAILURES
			{
				StackHolder<0, 0> shA; // THIS MUST FAIL!
				StackHolder<1, 0> shB; // THIS MUST FAIL!
				StackHolder<0, 1> shC; // THIS MUST FAIL!
			}
			{
				StackHolder<32, 16> sh;

				byte* shTestA = sh.RegisterSpace(16);
				memcpy((char*)(shTestA), "01_abcd_", 8);

				byte* shTestB = sh.RegisterSpace(8); // THIS MUST FAIL!
				sh.DeregisterSpace(shTestB); // THIS MUST FAIL!
			}
#endif
			return true;
		}

		bool Test_Vector()
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
				mintVector.Reserve(5);
				mintVector.PushBack(0);
				mintVector.PushBack(1);
				mintVector.PushBack(2);
				mintVector.PushBack(3);
				//mintVector.erase(1);
				mintVector.Insert(4, 1);
			}


			Vector<uint32> a(5);
			a.PushBack(1);
			a.PushBack(2);
			a.PushBack(3);
			a.Insert(2, 5);
			a.Erase(1);

			Vector<Type> resizeTest;
			resizeTest.PushBack(1);
			resizeTest.PushBack(2);
			resizeTest.PushBack(3);
			resizeTest.PushBack(4);
			resizeTest.PushBack(5);
			resizeTest.PushBack(6);
			resizeTest.PushBack(7);
			resizeTest.Resize(1);
			MINT_ASSURE(resizeTest.Size() == 1);

			Vector<uint32> b(20);
			b.PushBack(9);

			// Move semantic 점검!
			std::swap(a, b);

			Vector<uint32> c(3);
			c.Insert(3, 10);
			c.Insert(2, 0);
			c.Insert(99, 1);
			c.Insert(1, 0);
			c.Insert(0, 100);
			c.Erase(100);
			c.Erase(2);
			c.ShrinkToFit();

			return true;
		}

		bool Test_HashMap()
		{
			HashMap<StringA, StringA> hashMap;
			hashMap.Insert("1", "a");
			hashMap.Insert("5", "b");
			hashMap.Insert("11", "c");
			hashMap.Insert("21", "d");
			hashMap.Insert("33", "e");
			hashMap.Insert("41", "f");

			KeyValuePair keyValuePair0 = hashMap.Find("1");

			hashMap.Erase("21");

			KeyValuePair keyValuePair1 = hashMap.Find("21");

			return true;
		}

		bool Test_SharedPtr()
		{
			SharedPtr<StringA> sp0;
			sp0 = MakeShared<StringA>();
			sp0 = MakeShared<StringA>("abc");
			sp0 = MakeShared<StringA>("abcasad sads dsa qew w eq d asd s z");
			return true;
		}

		bool Test_StringTypes()
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
				ss0 = ss1.Substring(3);
				MINT_ASSURE(ss1.RFind("def") == 3);
				MINT_ASSURE(ss1.RFind("fgh") == 5);
				MINT_ASSURE(ss1.RFind("ghi") == kStringNPos);
				MINT_ASSURE(ss1.RFind("abc") == 0);
				MINT_ASSURE(ss1.RFind("abcdc") == kStringNPos);
				MINT_ASSURE(ss1.RFind("zab") == kStringNPos);
				MINT_ASSURE(ss1 == "abcdefgh");
				ss0 = ss1;
				MINT_ASSURE(ss0 == ss1);
			}
			//StackStringU8<256> ss2{ u8"abc가나다123!@#" };
			//MINT_ASSURE(ss2.RFind(u8"abc") == 11);
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

			auto logArray = Profiler::ScopedCPUProfiler::GetEntireLogData();
			const bool IsEmpty = logArray.empty();
#endif
#pragma endregion

#pragma region String
			StringA sa0 = StringA("Hello, there! My friend!!!");
			MINT_ASSURE(sa0.Capacity() == sa0.Size());
			sa0 += "I'm testing...";

			StringA saSmall0 = StringA("abcde");
			MINT_ASSURE(sa0.Capacity() > sa0.Size()); // SSO
			saSmall0.Resize(3);
			MINT_ASSURE(sa0.Capacity() > sa0.Size()); // SSO
			saSmall0 += "+testing...";
			saSmall0 += "+testing..!";
			saSmall0.Clear();

			StringA sa1 = StringA("Hello, there! My friend!!!") + " This is test for append function!";

			StringW sw0 = StringW(L"Hello, there!");
			sw0 += L" I'm testing...";

			StringW sw1 = StringW(L"TestStringW1");
			sw1 += L" ";
			sw1 += sw0;

			StringW sw2 = sw1.Substring(4, 6);

			const uint32 found = sw1.Find(L"Str", 3);
			sw1.Resize(3);
			sw1.Resize(10, 'z');
			const uint64 hash = sw1.ComputeHash();

			const bool cmp0 = (sw2 == sw1);
			const bool cmp1 = (sw2 == StringW(L"String"));

			// small string optimization
			StringA sa2;
			for (uint32 i = 0; i < 5; i++)
			{
				sa2 += "[[Repeating string content...]]";
			}
			MINT_ASSERT(sa2.IsShortString() == false, "%s must be long string!!!", MINT_VARNAME(sa2));
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

				a.Clear();
				b.Clear();
				MINT_ASSURE(a.Length() == 0);
				MINT_ASSURE(b.Length() == 0);
				MINT_ASSURE(a == b);

				a.Assign("efg");
				b.Assign("hij");
				MINT_ASSURE(a != b);

				a.Append("life!");
				b.Append("life?");
				MINT_ASSURE(a != b);
				//MINT_LOG("a: %s, capacity: %d", a.c_str(), a.capacity());
				//MINT_LOG("b: %s, capacity: %d", b.c_str(), b.capacity());

				MINT_ASSURE(a.Contains("life!!!") == false);
				MINT_ASSURE(a.Contains("life!") == true);
				MINT_ASSURE(a.Contains("fe!") == true);
				MINT_ASSURE(b.Contains("life???") == false);
				MINT_ASSURE(b.Contains("life?") == true);
				MINT_ASSURE(b.Contains("fe?") == true);

				//MINT_LOG("hash of a: %llX", a.ComputeHash());
				//MINT_LOG("hash of b: %llX", b.ComputeHash());
			}

			return true;
		}

		bool Test_StringUtil()
		{
			{
				const std::string testA{ "ab c   def g" };
				Vector<std::string> testATokens;
				StringUtil::Tokenize(testA, ' ', testATokens);

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
				StringUtil::Tokenize(testB, delimiters, testBTokens);
			}

			{
				static_assert(StringUtil::Length("abc") == 3);
				static_assert(StringUtil::Length("가나다라") == 8);
				static_assert(StringUtil::Length("韓國") == 4);

				static_assert(StringUtil::Length(L"abc") == 3);
				static_assert(StringUtil::Length(L"가나다라") == 4);
				static_assert(StringUtil::Length(L"韓國") == 2);

				static_assert(StringUtil::Length(u8"abc") == 3);
				static_assert(StringUtil::Length(u8"가나다라") == 12);
				static_assert(StringUtil::Length(u8"韓國") == 6);
			}

			{
				static_assert(StringUtil::CountChars("abc") == 3);
				static_assert(StringUtil::CountChars("가나다라") == 4);
				static_assert(StringUtil::CountChars("韓國") == 2);

				static_assert(StringUtil::CountChars(L"abc") == 3);
				static_assert(StringUtil::CountChars(L"가나다라") == 4);
				static_assert(StringUtil::CountChars(L"韓國") == 2);

				static_assert(StringUtil::CountChars(u8"abc") == 3);
				static_assert(StringUtil::CountChars(u8"가나다라") == 4);
				static_assert(StringUtil::CountChars(u8"韓國") == 2);
			}

			static_assert(StringUtil::CountBytesInCharCode(0b11111111) == 4);
			static_assert(StringUtil::CountBytesInCharCode(0b11110000) == 4);
			static_assert(StringUtil::CountBytesInCharCode(0b11101111) == 3);
			static_assert(StringUtil::CountBytesInCharCode(0b11100000) == 3);
			static_assert(StringUtil::CountBytesInCharCode(0b11011111) == 2);
			static_assert(StringUtil::CountBytesInCharCode(0b11000000) == 2);
			static_assert(StringUtil::CountBytesInCharCode(0b01111111) == 1);
			static_assert(StringUtil::CountBytesInCharCode(0b00000000) == 1);

			{
				static_assert(StringUtil::Find("abcdeabcde", "abc") == 0);
				static_assert(StringUtil::Find("abcdeabcde", "abc", 1) == 5);
				static_assert(StringUtil::Find("abcde", "cde") == 2);
				static_assert(StringUtil::Find("abcde", "def") == kStringNPos);
				static_assert(StringUtil::Find("abc가나다라def", "다라") == 7);
				static_assert(StringUtil::Find("abc가나다라def", "다라", 7) != kStringNPos);
				static_assert(StringUtil::Find("abc가나다라def", "다라", 8) == kStringNPos);
				static_assert(StringUtil::Find("abc가나다라def", "다라", 100) == kStringNPos);

				static_assert(StringUtil::Find(L"abcdeabcde", L"abc") == 0);
				static_assert(StringUtil::Find(L"abcdeabcde", L"abc", 1) == 5);
				static_assert(StringUtil::Find(L"abcde", L"cde") == 2);
				static_assert(StringUtil::Find(L"abcde", L"def") == kStringNPos);
				static_assert(StringUtil::Find(L"abc가나다라def", L"다라") == 5);
				static_assert(StringUtil::Find(L"abc가나다라def", L"다라", 5) != kStringNPos);
				static_assert(StringUtil::Find(L"abc가나다라def", L"다라", 6) == kStringNPos);
				static_assert(StringUtil::Find(L"abc가나다라def", L"다라", 100) == kStringNPos);

				static_assert(StringUtil::Find(u8"abcdeabcde", u8"abc") == 0);
				static_assert(StringUtil::Find(u8"abcdeabcde", u8"abc", 1) == 5);
				static_assert(StringUtil::Find(u8"abcde", u8"cde") == 2);
				static_assert(StringUtil::Find(u8"abcde", u8"def") == kStringNPos);
				static_assert(StringUtil::Find(u8"abc가나다라def", u8"다라") == 9);
				static_assert(StringUtil::Find(u8"abc가나다라def", u8"다라", 9) != kStringNPos);
				static_assert(StringUtil::Find(u8"abc가나다라def", u8"다라", 10) == kStringNPos);
				static_assert(StringUtil::Find(u8"abc가나다라def", u8"다라", 100) == kStringNPos);
			}

			{
				static_assert(StringUtil::FindLastOf("abcdeabcde", "abc") == 7);
				static_assert(StringUtil::FindLastOf("abcdeabcde", "abc", 5) == 7);
				static_assert(StringUtil::FindLastOf("abcdeabcde", "abc", 6) == kStringNPos);
				static_assert(StringUtil::FindLastOf("abcdeabcde", "cde") == 9);
				static_assert(StringUtil::FindLastOf("abcdeabcde", "def") == kStringNPos);
				static_assert(StringUtil::FindLastOf("abc가나다라def", "다라") == 10);
				static_assert(StringUtil::FindLastOf("abc가나다라def", "다라", 7) != kStringNPos);
				static_assert(StringUtil::FindLastOf("abc가나다라def", "다라", 8) == kStringNPos);
				static_assert(StringUtil::FindLastOf("abc가나다라def", "다라", 100) == kStringNPos);
			}

			{
				constexpr const char* a = nullptr;
				constexpr const char* b = nullptr;
				static_assert(StringUtil::Equals(a, b));
				static_assert(StringUtil::Equals(a, "abc") == false);
				static_assert(StringUtil::Equals("abc", b) == false);
				static_assert(StringUtil::Equals("abc", "abc"));
				static_assert(StringUtil::Equals("abc", "abcd") == false);
				static_assert(StringUtil::Equals("abcd", "abc") == false);

				constexpr const wchar_t* c = nullptr;
				constexpr const wchar_t* d = nullptr;
				static_assert(StringUtil::Equals(c, d));
				static_assert(StringUtil::Equals(c, L"abc") == false);
				static_assert(StringUtil::Equals(L"abc", d) == false);
				static_assert(StringUtil::Equals(L"abc", L"abc"));
				static_assert(StringUtil::Equals(L"abc", L"abcd") == false);
				static_assert(StringUtil::Equals(L"abcd", L"abc") == false);

				constexpr const char8_t* e = nullptr;
				constexpr const char8_t* f = nullptr;
				static_assert(StringUtil::Equals(e, f));
				static_assert(StringUtil::Equals(e, u8"abc") == false);
				static_assert(StringUtil::Equals(u8"abc", f) == false);
				static_assert(StringUtil::Equals(u8"abc", u8"abc"));
				static_assert(StringUtil::Equals(u8"abc", u8"abcd") == false);
				static_assert(StringUtil::Equals(u8"abcd", u8"abc") == false);
			}

			{
				constexpr uint32 kSize = 256;
				char bufferA[kSize]{};
				wchar_t bufferB[kSize]{};
				char8_t bufferC[kSize]{};
				StringUtil::Copy(bufferA, "가나다라");
				StringUtil::Copy(bufferA, nullptr);
				StringUtil::Copy(bufferB, L"가나다라");
				StringUtil::Copy(bufferB, nullptr);
				StringUtil::Copy(bufferC, u8"가나다라");
				StringUtil::Copy(bufferC, nullptr);

				char bufferD[4]{};
				//StringUtil::Copy(bufferD, "가나다라");
				wchar_t bufferE[4]{};
				//StringUtil::Copy(bufferE, L"가나다라");
				char8_t bufferF[4]{};
				StringUtil::Copy(bufferF, u8"가나다라");
			}

			static_assert(StringUtil::Is7BitASCII(u8"abc"));
			static_assert(StringUtil::Is7BitASCII(u8"0.125f"));
			static_assert(StringUtil::Is7BitASCII(u8"가나다") == false);
			static_assert(StringUtil::Is7BitASCII(u8"韓國") == false);
			{
				static_assert(StringUtil::CountChars(u8"가나다abc") == 6);
				U8CharCodeViewer u8CharCodeViewer(u8"가나다");
				for (auto charCode : u8CharCodeViewer)
				{
					const StringU8 u8String = StringUtil::Decode(charCode);
					const StringW wideString = StringUtil::ConvertUTF8ToWideString(u8String);
					StringA string;
					StringUtil::ConvertWideStringToString(wideString, string);
					MINT_LOG("CONVERTED: %s", string.CString());
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
				MINT_LOG("StrnigH Length:%d, CharCount:%d", stringH.Length(), stringH.CountChars());
			}

			{
				StackStringA<256> ssa0;
				StackStringW<256> ssw0;
				StackStringU8<256> ssu0;
				StringA sa0 = StringUtil::ToStringA(1234);
				StringW sw0 = StringUtil::ToStringW(1234);
				StringU8 su0 = StringUtil::ToStringU8(1234);
				const int64 sa0i = StringUtil::StringToInt64(sa0);
				const int64 sw0i = StringUtil::StringToInt64(sw0);
				const int64 su0i = StringUtil::StringToInt64(su0);
				StringUtil::ToString(1234, ssa0);
				StringUtil::ToString(1234, sa0);
				StringUtil::ToString(1234, ssw0);
				StringUtil::ToString(1234, sw0);
				StringUtil::ToString(1234, ssu0);
				StringUtil::ToString(1234, su0);

				StringUtil::ToString(12.34f, ssa0);
				StringUtil::ToString(12.34f, sa0);
				StringUtil::ToString(12.34f, ssw0);
				StringUtil::ToString(12.34f, sw0);
				StringUtil::ToString(12.34f, ssu0);
				StringUtil::ToString(12.34f, su0);

				const int32 i32_0 = StringUtil::StringToInt32(StringReference("12345"));
				const int64 i64_0 = StringUtil::StringToInt64(StringReference("1234567890"));
				const float f0 = StringUtil::StringToFloat(StringReference("12345"));
				const double d0 = StringUtil::StringToDouble(StringReference("1234567890"));
			}

			return true;
		}

		bool Test_Queue()
		{
			using Type = int32;
			Queue<Type> queue;
			queue.Reserve(32);

			queue.Push(0);
			queue.Push(1);
			queue.Push(2);
			queue.Push(3);
			queue.Push(4);
			queue.Push(5);
			queue.Push(6);
			queue.Push(7);

			queue.Pop();

			queue.Push(8);

			queue.Pop();
			queue.Pop();

			queue.Push(9);
			for (size_t i = 0; i < 25; i++)
			{
				queue.Push(1);
			}
			queue.Push(2);
			queue.Flush();
			return true;
		}

		bool Test_Tree()
		{
			Tree<std::string> stringTree;
			TreeNodeAccessor rootNode = stringTree.CreateRootNode("ROOT");

			TreeNodeAccessor a = rootNode.InsertChildNode("A");
			const std::string& aData = a.GetNodeData();

			TreeNodeAccessor b = a.InsertChildNode("b");
			TreeNodeAccessor c = a.InsertChildNode("c");

			TreeNodeAccessor d = rootNode.InsertChildNode("D");

			//stringTree.SwapNodeData(a, b);
			b.MoveToParent(rootNode);
			TreeNodeAccessor bParent = b.GetParentNode();
			const uint32 aChildCount = a.GetChildNodeCount();
			a.ClearChildNodes();
#if defined MINT_TEST_FAILURES
			TreeNodeAccessor aInvalidChild = a.GetChildNode(10);
#endif

			TreeNodeAccessor found = stringTree.FindNode(rootNode, "A");

			//stringTree.EraseChildNode(rootNode, a);
			//stringTree.clearChildren(rootNode);
			stringTree.DestroyRootNode();

#if defined MINT_TEST_FAILURES
			stringTree.MoveToParent(rootNode, d);
#endif
			return true;
		}
	}
}
