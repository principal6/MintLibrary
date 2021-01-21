#pragma once


#ifndef FS_GUI_CONTEXT_H
#define FS_GUI_CONTEXT_H


#include <CommonDefinitions.h>

#include <FsLibrary/SimpleRendering/ShapeRenderer.h>
#include <FsLibrary/SimpleRendering/FontRenderer.h>

#include <FsMath/Include/Float2.h>


namespace fs
{
	namespace SimpleRendering
	{
		class GraphicDevice;
	}

	namespace Window
	{
		class IWindow;
	}


	namespace Gui
	{
		class GuiContext final
		{
			static constexpr int16						kDefaultFontSize = 16;
			static constexpr float						kDefaultIntervalX = 5.0f;
			static constexpr float						kDefaultIntervalY = 5.0f;
			static constexpr float						kDefaultPaddingX = 10.0f;
			static constexpr float						kDefaultPaddingY = 30.0f;
			static constexpr float						kDefaultRoundness = 0.25f;
			static constexpr float						kDefaultControlWidth = 150.0f;

			struct ControlData
			{
			public:
								ControlData();

			public:
				fs::Float2		_innerPadding;
				fs::Float2		_displaySize;
				fs::Float2		_interactionSize;
				fs::Float2		_position; // In screen space, at left-top corner
				fs::Float2		_childAt;
				fs::Float2		_offset;
			};

		public:
														GuiContext(fs::SimpleRendering::GraphicDevice* const graphicDevice);
														~GuiContext();

		public:
			void										initialize(const char* const font);

		public:
			void										handleEvents(fs::Window::IWindow* const window);

		private:
			const bool									isMouseIn(const fs::Float2& mousePosition, const ControlData& controlData) const noexcept;


#pragma region Pre-states
		public:
			void										nextSameLine();
			void										nextControlSize(const fs::Float2& size, const bool force = false);

		private:
			void										resetNextStates();
#pragma endregion


#pragma region Controls
		public:
			// Return 'true' if clicked
			// If this returns 'true', endButton() must be called
			const bool									beginButton(const char* const id, const wchar_t* const text);
			void										endButton();

		private:
			ControlData&								getParentControlDataXXX() noexcept;
			ControlData									createControlData(const fs::Float2& defaultSize) noexcept;
			fs::Float3									getControlCenterPosition(const ControlData& controlData) const noexcept;
#pragma endregion


#pragma region Before drawing controls
		private:
			const bool									processClickControl(const ControlData& controlData, fs::SimpleRendering::Color& outBackgroundColor) const noexcept;
#pragma endregion


		public:
			void										render();

		private:
			fs::SimpleRendering::ShapeRenderer			_shapeRenderer;
			fs::SimpleRendering::FontRenderer			_fontRenderer;

		private:
			const ControlData							kNullControlData;
			std::unordered_map<uint64, uint32>			_idToIndexMap;
			ControlData									_rootControlData;
			std::vector<ControlData>					_controlDataArray;
			std::vector<uint32>							_controlIndexStack;

		private:
			bool										_isSameLine;
			fs::Float2									_nextControlSize;
			bool										_isSizingForced;

#pragma region Mouse states
		private:
			fs::Float2									_mousePosition;
			fs::Float2									_mouseDownPosition;
			fs::Float2									_mouseUpPosition;
			bool										_mouseButtonDown;
			bool										_mouseDownUp;
#pragma endregion

		private:
			fs::SimpleRendering::Color					_normalStateColor;
			fs::SimpleRendering::Color					_hoverStateColor;
			fs::SimpleRendering::Color					_pressedStateColor;

			fs::SimpleRendering::Color					_windowBackgroundColor;

			fs::SimpleRendering::Color					_lightFontColor;
		};
	}
}


#endif // !FS_GUI_CONTEXT_H
