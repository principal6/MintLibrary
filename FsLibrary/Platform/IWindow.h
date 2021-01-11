#pragma once


#ifndef FS_I_WINDOW_H
#define FS_I_WINDOW_H


#include <FsLibrary/CommonDefinitions.h>

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
					_raw = rhs._raw;
				}
				~Data()
				{
					__noop;
				}
				uint64		_raw{};
				Int2		_mousePosition;
				KeyCode		_keyCode;
			};

			EventType		_type{ EventType::None };
			Data			_data{};
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

		public:
			CreationError					getCreationError() const noexcept { return _creationError; }

		public:
			virtual void					size(const Int2& newSize) abstract;
			virtual const Int2&				size() const noexcept { return _creationData._size; }
			
			virtual const Int2&				entireSize() const noexcept { return _entireSize; }

			virtual void					position(const Int2& newPosition) abstract;
			virtual const Int2&				position() const noexcept { return _creationData._position; }

		protected:
			bool							_isRunning{ false };
			CreationData					_creationData{};
			fs::Int2						_entireSize{};
			CreationError					_creationError{};
			std::queue<EventData>			_eventQueue{};
		};
	}
}


#endif // !FS_I_WINDOW_H
