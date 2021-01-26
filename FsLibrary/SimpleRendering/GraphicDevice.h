#pragma once


#ifndef FS_GRAPHIC_DEVICE_H
#define FS_GRAPHIC_DEVICE_H


#include <CommonDefinitions.h>

#include <FsLibrary/Container/StaticArray.h>
#include <FsLibrary/Container/ScopeString.h>
#include <FsLibrary/Container/UniqueString.h>

#include <FsLibrary/SimpleRendering/DxShaderHeaderMemory.h>
#include <FsLibrary/SimpleRendering/DxShader.h>
#include <FsLibrary/SimpleRendering/DxResource.h>
#include <FsLibrary/SimpleRendering/TriangleRenderer.h>
#include <FsLibrary/SimpleRendering/RectangleRenderer.h>
#include <FsLibrary/SimpleRendering/ShapeRenderer.h>
#include <FsLibrary/SimpleRendering/FontRenderer.h>

#include <FsLibrary/Gui/GuiContext.h>

#include <FsLibrary/Language/CppHlsl.h>

#include <FsMath/Include/Float4x4.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>
#include <Assets/CppHlsl/CppHlslConstantBuffers.h>


//#define FS_TEST_MEMORY_FONT_TEXTURE


namespace fs
{
	namespace Window
	{
		class IWindow;
	}

	namespace SimpleRendering
	{
		using Microsoft::WRL::ComPtr;


		class GraphicDevice final
		{
		public:
																		GraphicDevice();
																		~GraphicDevice() = default;

		public:
			void														initialize(fs::Window::IWindow* const window);

		private:
			void														createDxDevice();

		private:
			void														initializeShaderHeaderMemory();
			void														initializeShaders();
			void														initializeSamplerStates();
			void														initializeBlendStates();

#if defined FS_TEST_MEMORY_FONT_TEXTURE
			void														createFontTextureFromMemory();
#endif

		public:
			void														beginRendering();
			void														endRendering();

		public:
			fs::SimpleRendering::DxShaderPool&							getShaderPool() noexcept;
			fs::SimpleRendering::DxResourcePool&						getResourcePool() noexcept;
			fs::SimpleRendering::RectangleRenderer&						getRectangleRenderer() noexcept;
			fs::SimpleRendering::ShapeRenderer&							getShapeRenderer() noexcept;
			fs::SimpleRendering::FontRenderer&							getFontRenderer() noexcept;
			fs::Gui::GuiContext&										getGuiContext() noexcept;
			const fs::Language::CppHlsl&								getCppHlslStructs() const noexcept;

		public:
			ID3D11Device*												getDxDevice() noexcept;
			ID3D11DeviceContext*										getDxDeviceContext() noexcept;
			const fs::Int2&												getWindowSize() const noexcept;
			fs::Window::IWindow*										getWindow() noexcept;

		private:
			fs::Window::IWindow*										_window;

		private:
			float														_clearColor[4];

	#pragma region DirectX
		private:
			ComPtr<IDXGISwapChain>										_swapChain;
			ComPtr<ID3D11Device>										_device;
			ComPtr<ID3D11DeviceContext>									_deviceContext;

		private:
			ComPtr<ID3D11Texture2D>										_backBuffer;
			ComPtr<ID3D11RenderTargetView>								_backBufferRtv;
			ComPtr<ID3D11Texture2D>										_depthStencilBuffer;
			ComPtr<ID3D11DepthStencilView>								_depthStencilView;
			ComPtr<ID3D11DepthStencilState>								_depthStencilStateLessEqual;

		private:
			DxShaderHeaderMemory										_shaderHeaderMemory;
			DxShaderPool												_shaderPool;
			DxResourcePool												_resourcePool;

		private:
			ComPtr<ID3D11SamplerState>									_samplerState;
			ComPtr<ID3D11BlendState>									_blendState;
	#pragma endregion

#if defined FS_TEST_MEMORY_FONT_TEXTURE
		private:
			static constexpr uint32										kFontTextureWidth		= 16 * kBitsPerByte;
			static constexpr uint32										kFontTextureHeight		= 60;
			static constexpr uint32										kFontTexturePixelCount	= kFontTextureWidth * kFontTextureHeight;
			std::vector<uint8>											_fontTextureRaw;
			ComPtr<ID3D11ShaderResourceView>							_fontTextureSrv;
#endif

		private:
			fs::Language::CppHlsl										_cppHlslStreamData;
			fs::Language::CppHlsl										_cppHlslConstantBuffers;
			fs::Language::CppHlsl										_cppHlslStructuredBuffers;

		private:
			fs::SimpleRendering::RectangleRenderer						_rectangleRenderer;
			fs::SimpleRendering::ShapeRenderer							_shapeRenderer;
			fs::SimpleRendering::FontRenderer							_fontRenderer;
		
		private:
			fs::Gui::GuiContext											_guiContext;
		};
	}
}


#include <FsLibrary/SimpleRendering/GraphicDevice.inl>


#endif // !FS_GRAPHIC_DEVICE_H
