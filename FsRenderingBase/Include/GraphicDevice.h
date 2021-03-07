#pragma once


#ifndef FS_GRAPHIC_DEVICE_H
#define FS_GRAPHIC_DEVICE_H


#include <CommonDefinitions.h>

#include <FsContainer/Include/StaticArray.h>
#include <FsContainer/Include/ScopeString.h>
#include <FsContainer/Include/UniqueString.h>

#include <FsRenderingBase/Include/DxShaderHeaderMemory.h>
#include <FsRenderingBase/Include/DxShader.h>
#include <FsRenderingBase/Include/DxResource.h>
#include <FsRenderingBase/Include/TriangleRenderer.h>
#include <FsRenderingBase/Include/RectangleRendererContext.h>
#include <FsRenderingBase/Include/ShapeRendererContext.h>
#include <FsRenderingBase/Include/FontRendererContext.h>

#include <FsRenderingBase/Include/GuiContext.h>

#include <FsRenderingBase/Include/Language/CppHlsl.h>

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

	namespace RenderingBase
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
			void														useScissorRectanglesWithMultipleViewports() noexcept;
			void														useFullScreenViewport() noexcept;
			const D3D11_VIEWPORT&										getFullScreenViewport() const noexcept;

		public:
			fs::RenderingBase::DxShaderPool&							getShaderPool() noexcept;
			fs::RenderingBase::DxResourcePool&							getResourcePool() noexcept;
			fs::RenderingBase::RectangleRendererContext&				getRectangleRendererContext() noexcept;
			fs::RenderingBase::ShapeRendererContext&					getShapeRendererContext() noexcept;
			fs::RenderingBase::FontRendererContext&						getFontRendererContext() noexcept;
			fs::Gui::GuiContext&										getGuiContext() noexcept;
			const fs::Language::CppHlsl&								getCppHlslSteamData() const noexcept;
			const fs::Language::CppHlsl&								getCppHlslConstantBuffers() const noexcept;

		public:
			fs::RenderingBase::CB_View&									getCbViewData() noexcept;
			DxObjectId													getCbViewId() const noexcept;

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
			ComPtr<ID3D11RasterizerState>								_rasterizerStateDefault;
			ComPtr<ID3D11RasterizerState>								_rasterizerStateScissorRectangles;
			D3D11_VIEWPORT												_fullScreenViewport;

		private:
			DxShaderHeaderMemory										_shaderHeaderMemory;
			DxShaderPool												_shaderPool;
			DxResourcePool												_resourcePool;
		
		private:
			fs::RenderingBase::CB_View									_cbViewData;
			DxObjectId													_cbViewId;

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
			fs::RenderingBase::RectangleRendererContext					_rectangleRendererContext;
			fs::RenderingBase::ShapeRendererContext						_shapeRendererContext;
			fs::RenderingBase::FontRendererContext						_fontRendererContext;
		
		private:
			fs::Gui::GuiContext											_guiContext;
		};
	}
}


#include <FsRenderingBase/Include/GraphicDevice.inl>


#endif // !FS_GRAPHIC_DEVICE_H
