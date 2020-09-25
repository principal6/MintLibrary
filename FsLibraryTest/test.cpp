#include <FsLibrary.h>


#pragma comment(lib, "FsLibrary.lib")


//#define FS_TEST_FAILURES


void testIntTypes()
{
	using namespace fs;
	Int2 ni;
	Int2 a{ 1, 2 };
	Int2 b{ 3, 4 };
	Int2 c = a + b;
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

bool testStringTypes()
{
	using fs::ScopeStringA;
	using fs::UniqueStringA;

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
		static constexpr uint32 kUniqueStringHolderCapacity = 256;

		UniqueStringA<kUniqueStringHolderCapacity> a{ "ab" };
		UniqueStringA<kUniqueStringHolderCapacity> b{ "cdef" };
		UniqueStringA<kUniqueStringHolderCapacity> c{ "ab" };
		UniqueStringA<kUniqueStringHolderCapacity> d;
		UniqueStringA<kUniqueStringHolderCapacity> e{ "" };
		UniqueStringA<kUniqueStringHolderCapacity> f{};
		d = b;
		d.assign("haha!");
		const bool cmp0 = (a == b);
		const bool cmp1 = (a == c);
		const bool cmp2 = (e == f);
	}
#pragma endregion

	return true;
}

bool testBitVector()
{
	using fs::BitVector;

	BitVector a;
	a.reserve(4);
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
	return true;
}

bool testWindow()
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

	GraphicDevice graphicDevice;
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
					const Int2 pos = window.position();
					window.position(pos - Int2(5, 0));
				}
				else if (event._data._keyCode == EventData::KeyCode::Right)
				{
					const Int2 pos = window.position();
					window.position(pos + Int2(5, 0));
				}
			}
		}

		graphicDevice.beginRendering();
		{
			fs::RectangleRenderer& rectangleRenderer = graphicDevice.getRectangleRenderer();

			rectangleRenderer.setPosition(fs::Float3(0, 0, 0));
			rectangleRenderer.setSize(fs::Float2(0.5f, 0.5f));
			rectangleRenderer.drawColored(fs::Float4(0.5f, 0.5f, 0.5f, 1.0f));

			rectangleRenderer.setPosition(fs::Float3(0.5f, 0.5f, 0));
			rectangleRenderer.setSize(fs::Float2(0.5f, 0.5f));
			rectangleRenderer.drawColoredTextured(fs::Float4(1, 0, 0, 1), fs::Float2(0, 0), fs::Float2(1, 1));
		}
		graphicDevice.endRendering();
	}
	return true;
}


int main()
{
	testIntTypes();

	testFloatTypes();

	testStaticArray();

	testStackHolder();

	testStringTypes();

	testBitVector();

	FS_LOG("김장원", "Log Test");

	testWindow();
	

	return 0;
}
