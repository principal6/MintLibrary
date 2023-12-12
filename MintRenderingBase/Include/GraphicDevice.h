#pragma once


#ifndef _MINT_RENDERING_BASE_GRAPHIC_DEVICE_H_
#define _MINT_RENDERING_BASE_GRAPHIC_DEVICE_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Array.h>
#include <MintContainer/Include/StackString.h>

#include <MintRenderingBase/Include/DxShaderHeaderMemory.h>
#include <MintRenderingBase/Include/DxShader.h>
#include <MintRenderingBase/Include/DxResource.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>
#include <MintRenderingBase/Include/ShapeRendererContext.h>

#include <MintRenderingBase/Include/CppHlsl/Interpreter.h>

#include <MintMath/Include/Float4x4.h>

#include <Assets/Include/CppHlsl/CppHlslStreamData.h>
#include <Assets/Include/CppHlsl/CppHlslConstantBuffers.h>
#include <Assets/Include/CppHlsl/CppHlslStructuredBuffers.h>


namespace mint
{
	class Window;

	namespace Rendering
	{
		using Microsoft::WRL::ComPtr;


		D3D11_RECT RectToD3dRect(const Rect& rect) noexcept;


		class SafeResourceMapper
		{
		public:
			SafeResourceMapper(GraphicDevice& graphicDevice, ID3D11Resource* const resource, const uint32 subresource);
			~SafeResourceMapper();

		public:
			bool IsValid() const noexcept;
			void Set(const void* const data, const uint32 size) noexcept;

		private:
			GraphicDevice& _graphicDevice;
			ID3D11Resource* const _resource;
			const uint32 _subresource;
			D3D11_MAPPED_SUBRESOURCE _mappedSubresource;
		};


		class GraphicDevice final
		{
		public:
			class StateManager
			{
			public:
				StateManager(GraphicDevice& graphicDevice);
				~StateManager() = default;

			public: // IA
				void SetIAInputLayout(ID3D11InputLayout* const iaInputLayout) noexcept;
				void SetIARenderingPrimitive(const RenderingPrimitive renderingPrimitive) noexcept;
				void SetIAVertexBuffers(const int32 bindingStartSlot, const uint32 bufferCount, ID3D11Buffer* const* const buffers, const uint32* const strides, const uint32* const offsets) noexcept;
				void SetIAIndexBuffer(ID3D11Buffer* const buffer, const DXGI_FORMAT format, const uint32 offset) noexcept;

			public: // RS
				void SetRSRasterizerState(ID3D11RasterizerState* const rsRasterizerState) noexcept;
				void SetRSViewport(const D3D11_VIEWPORT rsViewport) noexcept;
				void SetRSScissorRectangle(const D3D11_RECT rsScissorRectangle) noexcept;

			public: // Shader
				void SetVSShader(ID3D11VertexShader* const shader) noexcept;
				void SetGSShader(ID3D11GeometryShader* const shader) noexcept;
				void SetPSShader(ID3D11PixelShader* const shader) noexcept;

			public: // Resources
				void SetVSResources(const DxResource& resource, uint32 bindingSlot) noexcept;
				void SetGSResources(const DxResource& resource, uint32 bindingSlot) noexcept;
				void SetPSResources(const DxResource& resource, uint32 bindingSlot) noexcept;

			public: // Constant Buffers
				void SetVSConstantBuffers(const DxResource& constantBuffer, uint32 bindingSlot);
				void SetGSConstantBuffers(const DxResource& constantBuffer, uint32 bindingSlot);
				void SetPSConstantBuffers(const DxResource& constantBuffer, uint32 bindingSlot);

			private:
				GraphicDevice& _graphicDevice;

			private: // IA
				RenderingPrimitive _iaRenderingPrimitive;
				ID3D11InputLayout* _iaInputLayout;

			private: // RS
				ID3D11RasterizerState* _rsRasterizerState;
				D3D11_VIEWPORT _rsViewport;
				D3D11_RECT _rsScissorRectangle;

			private: // Shaders
				ID3D11VertexShader* _vsShader;
				ID3D11GeometryShader* _gsShader;
				ID3D11PixelShader* _psShader;

			private: // Resources
				Vector<GraphicObjectID> _vsShaderResources;
				Vector<GraphicObjectID> _gsShaderResources;
				Vector<GraphicObjectID> _psShaderResources;

			private: // Constant Buffers
				Vector<GraphicObjectID> _vsConstantBuffers;
				Vector<GraphicObjectID> _gsConstantBuffers;
				Vector<GraphicObjectID> _psConstantBuffers;
			};

			friend SafeResourceMapper;
			friend StateManager;

		public:
			static GraphicDevice& GetInvalidInstance();

		public:
			GraphicDevice(Window& window);
			~GraphicDevice() = default;

		public:
			bool Initialize();
			void UpdateScreenSize();

		private:
			void CreateDxDevice();
			bool LoadFontData();

		private:
			bool CreateSwapChain(const Int2& windowSize, const HWND windowHandle);
			bool InitializeBackBuffer();
			bool InitializeDepthStencilBufferAndView(const Int2& windowSize);
			bool InitializeDepthStencilStates();
			void InitializeShaderHeaderMemory();
			void InitializeShaders();
			void InitializeSamplerStates();
			void InitializeBlendStates();
			void InitializeFullScreenData(const Int2& windowSize);
			void SetDefaultRenderTargetsAndDepthStencil();

		public:
			void BeginRendering();
			void Draw(const uint32 vertexCount, const uint32 vertexOffset) noexcept;
			void DrawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset) noexcept;
			void EndRendering();

		public:
			void UseScissorRectangles() noexcept;
			void UseFullScreenViewport() noexcept;
			void UseWireFrameNoCullingRasterizer() noexcept;
			void UseWireFrameCullBackRasterizer() noexcept;
			void UseSolidCullBackRasterizer() noexcept;
			const Rect& GetFullScreenClipRect() const noexcept;

		public:
			DxShaderPool& GetShaderPool() noexcept;
			DxResourcePool& GetResourcePool() noexcept;
			ShapeRendererContext& GetShapeRendererContext() noexcept;
			const Language::CppHlsl::Interpreter& GetCppHlslSteamData() const noexcept;
			const Language::CppHlsl::Interpreter& GetCppHlslConstantBuffers() const noexcept;
			StateManager& GetStateManager() noexcept;

		public: // Common buffers
			GraphicObjectID GetCommonCBTransformID() const noexcept;
			GraphicObjectID GetCommonSBTransformID() const noexcept;
			GraphicObjectID GetCommonSBMaterialID() const noexcept;

		public:
			void Initialize2DProjectionMatrix() noexcept;
			void SetViewProjectionMatrix(const Float4x4& viewMatrix, const Float4x4& ProjectionMatrix) noexcept;

		public:
			ID3D11Device* GetDxDevice() noexcept;
			ID3D11DeviceContext* GetDxDeviceContext() noexcept;
			const Int2& GetWindowSize() const noexcept;
			Float2 GetWindowSizeFloat2() const noexcept;
			Window& AccessWindow() noexcept;
			const Window& GetWindow() const noexcept;

		private:
			Window& _window;
			Int2 _lastWindowSize;

		private:
			Color _clearColor;

#pragma region DirectX
		private:
			ComPtr<IDXGISwapChain> _swapChain;
			ComPtr<ID3D11Device> _device;
			ComPtr<ID3D11DeviceContext> _deviceContext;

		private:
			ComPtr<ID3D11RenderTargetView> _backBufferRtv;
			ComPtr<ID3D11Texture2D> _depthStencilBuffer;
			ComPtr<ID3D11DepthStencilView> _depthStencilView;
			ComPtr<ID3D11DepthStencilState> _depthStencilStateLessEqual;

		private:
			ID3D11RasterizerState* _currentRasterizerFor3D;
			ComPtr<ID3D11RasterizerState> _rasterizerStateSolidCullBack;
			ComPtr<ID3D11RasterizerState> _rasterizerStateWireFrameNoCulling;
			ComPtr<ID3D11RasterizerState> _rasterizerStateWireFrameCullBack;
			ComPtr<ID3D11RasterizerState> _rasterizerStateScissorRectangles;
			D3D11_VIEWPORT _fullScreenViewport;
			Rect _fullScreenClipRect;

		private:
			DxShaderHeaderMemory _shaderHeaderMemory;
			DxShaderPool _shaderPool;
			DxResourcePool _resourcePool;

		private:
			CB_View _cbViewData;
			GraphicObjectID _cbViewID;

		private: // Common buffers
			GraphicObjectID _cbTransformID;
			GraphicObjectID _sbTransformID;
			GraphicObjectID _sbMaterialID;

		private:
			ComPtr<ID3D11SamplerState> _pointSamplerState;
			ComPtr<ID3D11SamplerState> _linearSamplerState;
			ComPtr<ID3D11BlendState> _blendState;
#pragma endregion

		private:
			StateManager _stateManager;

		private:
			Language::CppHlsl::Interpreter _cppHlslStreamData;
			Language::CppHlsl::Interpreter _cppHlslConstantBuffers;
			Language::CppHlsl::Interpreter _cppHlslStructuredBuffers;

		private:
			ShapeRendererContext _shapeRendererContext;
			bool _needEndRenderingCall;
		};
	}
}


#include <MintRenderingBase/Include/GraphicDevice.inl>


#endif // !_MINT_RENDERING_BASE_GRAPHIC_DEVICE_H_
