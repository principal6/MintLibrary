#pragma once


#ifndef _MINT_RENDERING_BASE_GRAPHICS_DEVICE_H_
#define _MINT_RENDERING_BASE_GRAPHICS_DEVICE_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/Array.h>
#include <MintContainer/Include/StackString.h>

#include <MintRenderingBase/Include/DxShaderHeaderMemory.h>
#include <MintRenderingBase/Include/Shader.h>
#include <MintRenderingBase/Include/ShaderPipeline.h>
#include <MintRenderingBase/Include/GraphicsResource.h>
#include <MintRenderingBase/Include/Material.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>

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
		class FontRenderer;
		class ShapeRenderer;
		class SpriteRenderer;
		using Microsoft::WRL::ComPtr;
		struct SB_Transform;
	}
}

namespace mint
{
	namespace Rendering
	{
		D3D11_RECT RectToD3dRect(const Rect& rect) noexcept;


		class SafeResourceMapper
		{
		public:
			SafeResourceMapper(GraphicsDevice& graphicsDevice, ID3D11Resource* const resource, const uint32 subresource);
			~SafeResourceMapper();

		public:
			bool IsValid() const noexcept;
			void Set(const void* const data, const uint32 size) noexcept;

		private:
			GraphicsDevice& _graphicsDevice;
			ID3D11Resource* const _resource;
			const uint32 _subresource;
			D3D11_MAPPED_SUBRESOURCE _mappedSubresource;
		};


		class GraphicsDevice final
		{
			friend SafeResourceMapper;

		public:
			class StateManager;

		public:
			static GraphicsDevice& GetInvalidInstance();

		public:
			GraphicsDevice(Window& window, bool usesMSAA);
			~GraphicsDevice();

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
			void InitializeDxShaderHeaderMemory();
			void InitializeSamplerStates();
			void InitializeBlendStates();
			void InitializeFullScreenData(const Int2& windowSize);
			void SetDefaultRenderTargetsAndDepthStencil();

		public:
			void BeginRendering();
			void BeginWorldSpaceRendering();
			void EndWorldSpaceRendering();
			void BeginScreenSpaceRendering();
			void EndScreenSpaceRendering();
			void EndRendering();

			void Draw(const uint32 vertexCount, const uint32 vertexOffset) noexcept;
			void DrawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset) noexcept;

		public:
			void UseScissorRectangles() noexcept;
			void UseFullScreenViewport() noexcept;
			void UseWireFrameNoCullingRasterizer() noexcept;
			void UseWireFrameCullBackRasterizer() noexcept;
			void UseSolidCullBackRasterizer() noexcept;
			void SetSolidCullBackRasterizer() noexcept;
			void SetSolidCullFrontRasterizer() noexcept;
			void SetSolidCullNoneRasterizer() noexcept;
			const Rect& GetFullScreenClipRect() const noexcept;

		public:
			ShaderPool& GetShaderPool() noexcept;
			ShaderPipelinePool& GetShaderPipelinePool() noexcept;
			GraphicsResourcePool& GetResourcePool() noexcept;
			MaterialPool& GetMaterialPool() noexcept;
			FontRenderer& GetFontRenderer() noexcept;
			ShapeRenderer& GetShapeRenderer() noexcept;
			SpriteRenderer& GetSpriteRenderer() noexcept;
			const Language::CppHlsl::Interpreter& GetCppHlslSteamData() const noexcept;
			const Language::CppHlsl::Interpreter& GetCppHlslConstantBuffers() const noexcept;

		public:
			class StateManager
			{
			public:
				StateManager(GraphicsDevice& graphicsDevice);
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
				void SetShaderResources(GraphicsShaderType graphicsShaderType, const GraphicsResource* resource, uint32 bindingSlot) noexcept;
				void SetConstantBuffers(GraphicsShaderType graphicsShaderType, const GraphicsResource* constantBuffer, uint32 bindingSlot);

			private:
				GraphicsDevice& _graphicsDevice;

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
				Vector<GraphicsObjectID> _vsShaderResourceIDs;
				Vector<GraphicsObjectID> _gsShaderResourceIDs;
				Vector<GraphicsObjectID> _psShaderResourceIDs;

			private: // Constant Buffers
				Vector<GraphicsObjectID> _vsConstantBufferIDs;
				Vector<GraphicsObjectID> _gsConstantBufferIDs;
				Vector<GraphicsObjectID> _psConstantBufferIDs;
			};
			StateManager& GetStateManager() noexcept;

		public: // Common buffers
			GraphicsObjectID GetCommonCBTransformID() const noexcept;
			GraphicsObjectID GetCommonSBTransformID() const noexcept;
			GraphicsObjectID GetCommonSBMaterialID() const noexcept;

		public:
			void SetViewProjectionMatrix(const Float4x4& viewMatrix, const Float4x4& projectionMatrix) noexcept;
			const Float4x4& GetProjectionMatrix() const noexcept;
			const Float4x4& GetViewProjectionMatrix() const noexcept;
			Float4x4 GetScreenSpace2DProjectionMatrix() const noexcept;

		public:
			ID3D11Device* GetDxDevice() noexcept;
			ID3D11DeviceContext* GetDxDeviceContext() noexcept;
			const Int2& GetWindowSize() const noexcept;
			Window& AccessWindow() noexcept;
			const Window& GetWindow() const noexcept;

		private:
			void Render() noexcept;

		private:
			Window& _window;
			bool _usesMSAA;
			Int2 _cachedWindowSize;

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
			ComPtr<ID3D11RasterizerState> _rasterizerStateSolidCullFront;
			ComPtr<ID3D11RasterizerState> _rasterizerStateSolidCullNone;
			ComPtr<ID3D11RasterizerState> _rasterizerStateWireFrameNoCulling;
			ComPtr<ID3D11RasterizerState> _rasterizerStateWireFrameCullBack;
			ComPtr<ID3D11RasterizerState> _rasterizerStateScissorRectangles;
			D3D11_VIEWPORT _fullScreenViewport;
			Rect _fullScreenClipRect;

		private:
			DxShaderHeaderMemory _shaderHeaderMemory;
			ShaderPool _shaderPool;
			ShaderPipelinePool _shaderPipelinePool;
			GraphicsResourcePool _resourcePool;
			MaterialPool _materialPool;

		private:
			CB_View _cbViewData;
			float4x4 _viewMatrixCache;
			float4x4 _projectionMatrixCache;
			GraphicsObjectID _cbViewID;

		private: // Common buffers
			GraphicsObjectID _cbTransformID;
			GraphicsObjectID _sbTransformID;
			GraphicsObjectID _sbMaterialID;

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
			OwnPtr<LowLevelRenderer<VS_INPUT_SHAPE>> _lowLevelRendererForShapeAndFont;
			OwnPtr<LowLevelRenderer<VS_INPUT_SHAPE>> _lowLevelRendererForSprite;
			Vector<SB_Transform> _sbTransformDataForShapeAndFont;
			Vector<SB_Transform> _sbTransformDataForSprite;
			OwnPtr<FontRenderer> _fontRenderer;
			OwnPtr<ShapeRenderer> _shapeRenderer;
			OwnPtr<SpriteRenderer> _spriteRenderer;
			bool _isInRenderingScope;
		};
	}
}


#include <MintRenderingBase/Include/GraphicsDevice.inl>


#endif // !_MINT_RENDERING_BASE_GRAPHICS_DEVICE_H_
