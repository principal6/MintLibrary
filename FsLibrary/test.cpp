#include <Container/StaticArray.h>
#include <Platform/WindowsWindow.h>
#include <Container/StackHolder.hpp>
#include <Container/ScopeString.hpp>


//#define FS_TEST_FAILURES


void testScopeString()
{
	using namespace fs;

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

void testStackHolder()
{
	using namespace fs;
	{
		StackHolder<16, 6> sh;

		byte* shTestA = sh.registerSpace(2);
		memcpy((char*)(shTestA + 16 * 0), "1__abcdefghijk__", 16);
		memcpy((char*)(shTestA + 16 * 1), "2__lmnopqrstuv__", 16);

		byte* shTestB = sh.registerSpace(1);
		memcpy((char*)(shTestB + 16 * 0), "3__wxyzabcdefg__", 16);

		byte* shTestC = sh.registerSpace(1);
		memcpy((char*)(shTestC + 16 * 0), "4__helloMyFrie__", 16);

		sh.deregisterSpace(shTestB);

		byte* shTestD = sh.registerSpace(2);
		memcpy((char*)(shTestD + 16 * 0), "5__nd!!IAmLege__", 16);
	}
	{
		StackHolder<8, 12> sh;

		byte* shTestA = sh.registerSpace(7);
		memcpy((char*)(shTestA + 8 * 0), "01_abcd_", 8);
		memcpy((char*)(shTestA + 8 * 1), "02_efgh_", 8);
		memcpy((char*)(shTestA + 8 * 2), "03_ijkl_", 8);
		memcpy((char*)(shTestA + 8 * 3), "04_mnop_", 8);
		memcpy((char*)(shTestA + 8 * 4), "05_qrst_", 8);
		memcpy((char*)(shTestA + 8 * 5), "06_uvwx_", 8);
		memcpy((char*)(shTestA + 8 * 6), "07_yzab_", 8);

		byte* shTestB = sh.registerSpace(3);
		memcpy((char*)(shTestB + 8 * 7), "08_cdef_", 8);
		memcpy((char*)(shTestB + 8 * 8), "09_ghij_", 8);
		memcpy((char*)(shTestB + 8 * 9), "10_klmn_", 8);

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

void testIntTypes()
{
	using namespace fs;
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
	auto sizeFloat3 = sizeof(Float3);
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
	}
	return true;
}


int main()
{
	testFloatTypes();

	testWindow();

	return 0;
}