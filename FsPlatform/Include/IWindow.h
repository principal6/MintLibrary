#pragma once


#ifndef FS_I_WINDOW_H
#define FS_I_WINDOW_H


#include <CommonDefinitions.h>

#include <FsMath/Include/Int2.h>
#include <FsMath/Include/Float3.h>


namespace fs
{
	namespace Window
	{
		enum class Style
		{
			Default,
			Clean,
			Resizable,
		};

		enum class CreationError
		{
			None,
			FailedToGetInstanceHandle,
			FailedToCreateWindow,
			NullptrString,
		};

		struct CreationData
		{
			Int2				_size{};
			Int2				_position{ kInt16Min, kInt16Min };
			const wchar_t*		_title{};
			Style				_style{};
			Float3				_bgColor{ 1.0f, 1.0f, 1.0f };
		};

		enum class EventType
		{
			None,
			KeyDown,
			KeyUp,
			KeyStroke,
			KeyStrokeCandidate,
			MouseMove,
			MouseDown,
			MouseUp,
			MouseDoubleClicked,
			MouseWheel,
		};

		struct EventData
		{
			enum class KeyCode : uint64
			{
				NONE,
				Escape,
				Enter,
				Up,
				Down,
				Left,
				Right,
				Delete,
				Home,
				End,
				Shift,
			};

			class EventValue
			{
			public:
											EventValue();
											EventValue(const EventValue& rhs);
											~EventValue() = default;

			public:
				void						setKeyCode(const EventData::KeyCode keyCode) noexcept;
				const EventData::KeyCode	getKeyCode() const noexcept;

				void						setMousePosition(const fs::Float2& mousePosition) noexcept;
				const fs::Float2&			getMousePosition() const noexcept;

				void						setMouseWheel(const float mouseWheel) noexcept;
				const float					getMouseWheel() const noexcept;

				void						setInputWchar(const wchar_t inputWchar) noexcept;
				const wchar_t				getInputWchar() const noexcept;

			private:
				union
				{
					uint64					_raw[2]{};
					struct
					{
						fs::Float2			_mousePosition;
						int32				_mouseInfoI;
						float				_mouseInfoF;
					};
					struct
					{
						KeyCode				_keyCode;
						//int32				_reserved;
						//int32				_reserved1;
					};
					struct
					{
						wchar_t				_inputWchar;
					};
				};
			};

			EventType			_type{ EventType::None };
			EventValue			_value{};
		};

		enum class CursorType
		{
			Arrow,
			SizeVert,
			SizeHorz,
			SizeLeftTilted,
			SizeRightTilted,

			COUNT
		};

		class IWindow abstract
		{
		public:
											IWindow() = default;
			virtual							~IWindow() = default;

		public:
			virtual bool					create(const CreationData& creationData) noexcept abstract;
			virtual void					destroy() noexcept { _isRunning = false; }

		public:
			virtual bool					isRunning() noexcept { return _isRunning; }
			bool							hasEvent() const noexcept { return (0 < _eventQueue.size()); }
			EventData						popEvent()
			{
				EventData event = _eventQueue.front();
				_eventQueue.pop();
				return event;
			}
			const EventData&				peekEvent()
			{
				return _eventQueue.front();
			}

		public:
			CreationError					getCreationError() const noexcept { return _creationError; }

		public:
			virtual void					setSize(const Int2& newSize) abstract;
			const Int2&						getSize() const noexcept { return _creationData._size; }
			
			const Int2&						getEntireSize() const noexcept { return _entireSize; }

			virtual void					setPosition(const Int2& newPosition) abstract;
			const Int2&						getPosition() const noexcept { return _creationData._position; }
			
			const fs::Float3&				getBackgroundColor() const noexcept { return _creationData._bgColor; }

			virtual void					setCursorType(const CursorType cursorType) noexcept { _currentCursorType = cursorType; }
			const CursorType				getCursorType() const noexcept { return _currentCursorType; }

			virtual const uint32			getCaretBlinkIntervalMs() const noexcept abstract;

			virtual const bool				isKeyDown(const EventData::KeyCode keyCode) const noexcept abstract;

		protected:
			bool							_isRunning{ false };
			CreationData					_creationData{};
			fs::Int2						_entireSize{};
			CreationError					_creationError{};
			std::queue<EventData>			_eventQueue{};
		
		protected:
			CursorType						_currentCursorType;
		};
	}
}


#endif // !FS_I_WINDOW_H
