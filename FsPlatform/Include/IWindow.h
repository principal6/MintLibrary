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
				Escape,
				Enter,
				Up,
				Down,
				Left,
				Right
			};
			union Data
			{
				Data()
				{
					__noop;
				}
				Data(const Data& rhs)
				{
					_raw[0] = rhs._raw[0];
					_raw[1] = rhs._raw[1];
				}
				~Data()
				{
					__noop;
				}
				uint64		_raw[2]{};
				//
				struct
				{
					Int2		_mousePosition;
					int32		_mouseInfoI;
					float		_mouseInfoF;
				};
				struct
				{
					KeyCode		_keyCode;
					//int32		_reserved;
					//int32		_reserved1;
				};
			};

			EventType		_type{ EventType::None };
			Data			_data{};
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
