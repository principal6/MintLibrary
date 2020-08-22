#include <Container/StaticArray.h>
#include <Platform/WindowsWindow.h>
#include <Container/StackHolder.hpp>


int main()
{
	using namespace fs;
	using namespace fs::Window;


#pragma region StackHolder Test
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
	
#pragma endregion

#pragma region Integer Test
	{
		Int2 a{ 1, 2 };
		Int2 b{ 3, 4 };
		Int2 c = a + b;
	}
#pragma endregion

#pragma region Float Test
	{
		Float3 p{ 1, 0, 0 };
		Float3 q{ 0, 1, 0 };
		Float3 r = Float3::cross(p, q);
	}
#pragma endregion

#pragma region StaticArray Test
	constexpr StaticArray<int32, 3> arr{ 4, 5, 999 };
#pragma endregion

#pragma region WindowsWindow Test
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
		return 0;
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
#pragma endregion

	return 0;
}