#pragma once


#ifndef _MINT_PLATFORM_INPUT_CONTEXT_H_
#define _MINT_PLATFORM_INPUT_CONTEXT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Queue.h>

#include <MintMath/Include/Float2.h>

#include <MintPlatform/Include/PlatformCommon.h>


namespace mint
{
	class IWindow;


	enum class InputEventType
	{
		Mouse,
		Keyboard,
		COUNT
	};

	enum class InputMouseEventType
	{
		ButtonPressed,
		ButtonReleased,
		ButtonDoubleClicked,
		PointerMoved,
		PointerMovedDelta,
		WheelScrolled,
		COUNT
	};

	enum class InputKeyboardEventType
	{
		KeyPressed,
		KeyReleased,
		CharacterInput,
		CharacterInputCandidate,
		COUNT
	};

	struct InputEventMouseData
	{
		InputMouseEventType _type{ InputMouseEventType::COUNT };
		MouseButton _button{ MouseButton::COUNT };
		Float2 _position;
		float _wheelScroll{ 0.0f };
	};

	struct InputEventKeyboardData
	{
		InputKeyboardEventType _type{ InputKeyboardEventType::COUNT };
		KeyCode _keyCode{ KeyCode::NONE };
		wchar_t _character{};
	};

	struct InputEvent
	{
	public:
		InputEvent();

	public:
		InputEventType _type;
		union
		{
			InputEventMouseData _mouseData;
			InputEventKeyboardData _keyboardData;
		};
		IWindow* _window;
	};

	enum class MouseButtonState
	{
		Up,
		Pressed,
		Down,
		Released, // Click 도 여기
		DoubleClicked,
		COUNT
	};

	enum class KeyState
	{
		Up,
		Pressed,
		Down,
		Released,
		COUNT
	};

	struct MouseState
	{
	public:
		void Process() noexcept;

	public:
		bool IsPointerMoved() const noexcept;
		void SetPosition(const Float2& position) noexcept;
		void SetDeltaPosition(const Float2& deltaPosition) noexcept;
		const Float2& GetPosition() const noexcept;
		const Float2& GetDeltaPosition() const noexcept;
		MouseButtonState GetMouseButtonState(const MouseButton mouseButton) const;

	public:
		MouseButtonState _buttonStates[GetMouseButtonCount()]{};
		float _wheelScroll = 0.0f;

	public:
		MouseButton _pressedButton = MouseButton::COUNT;
		MouseButton _releasedButton = MouseButton::COUNT;
		MouseButton _clickedButton = MouseButton::COUNT;
		MouseButton _doubleClickedButton = MouseButton::COUNT;

	private:
		Float2 _position;
		Float2 _deltaPosition;
		bool _isPointerMoved = false;
	};

	struct KeyboardState
	{
		void Process() noexcept;

		KeyState _keyStates[GetKeyCodeCount()]{};

		KeyCode _pressedKeyCode;
		KeyCode _releasedKeyCode;
		wchar_t _character = L'\0';
		wchar_t _characterCandidate = L'\0';
	};


	class InputContext
	{
	private:
		InputContext();
		InputContext(const InputContext& rhs) = delete;
		InputContext(InputContext&& rhs) = delete;

	public:
		~InputContext();

	public:
		static InputContext& GetInstance() noexcept;

	public:
		void FlushInputEvents() noexcept;
		void PushInputEvent(const InputEvent& inputEvent) noexcept;

	public:
		void ProcessEvents() noexcept;

	public:
		const MouseState& GetMouseState() const noexcept;
		MouseButtonState GetMouseButtonState(const MouseButton mouseButton) const noexcept;
		bool IsMouseButtonPressed() const noexcept;
		bool IsMouseButtonReleased() const noexcept;
		bool IsMouseButtonClicked() const noexcept;
		bool IsMouseButtonDoubleClicked() const noexcept;
		bool IsMouseButtonDown(const MouseButton mouseButton) const noexcept;
		bool IsMouseButtonUp(const MouseButton mouseButton) const noexcept;
		bool IsMousePointerMoved() const noexcept;
		bool IsMouseWheelScrolled() const noexcept;
		Float2 GetMousePosition() const noexcept;
		Float2 GetMouseDeltaPosition() const noexcept;
		float GetMouseWheelScroll() const noexcept;

	public:
		const KeyboardState& GetKeyboardState() const noexcept;
		KeyState GetKeyState(const KeyCode keyCode) const noexcept;
		bool IsKeyPressed() const noexcept;
		bool IsKeyReleased() const noexcept;
		bool IsKeyInputCharacter() const noexcept;
		bool IsKeyInputCharacterCandidate() const noexcept;
		bool IsKeyDown(const KeyCode keyCode) const noexcept;
		bool IsKeyUp(const KeyCode keyCode) const noexcept;

	private:
		Queue<InputEvent> _events;

	private:
		MouseState _mouseState;
		KeyboardState _keyboardState;
	};
}


#endif // !_MINT_PLATFORM_INPUT_CONTEXT_H_
