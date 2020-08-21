#include <Container/StaticArray.h>
#include <Platform/WindowsWindow.h>


int main()
{
	using namespace fs;
	using namespace fs::Window;

#pragma region Integer Test
	Int2 a{ 1, 2 };
	Int2 b{ 3, 4 };
	Int2 c = a + b;
#pragma endregion

#pragma region Float Test
	Float3 p{ 1, 0, 0 };
	Float3 q{ 0, 1, 0 };
	Float3 r = Float3::cross(p, q);
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