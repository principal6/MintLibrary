#pragma once


#ifndef FS_GRAPHIC_DEVICE_H
#define FS_GRAPHIC_DEVICE_H


#include <CommonDefinitions.h>
#include <Container/StaticArray.h>
#include <Container/ScopeString.h>
#include <SimpleRendering/RectangleRenderer.h>
#include <SimpleRendering/DxShaderHeaderMemory.h>
#include <Reflection/IReflective.h>


namespace fs
{
	namespace Window
	{
		class IWindow;
	}
	using Microsoft::WRL::ComPtr;

	class VertexData : public IReflective
	{
		FS_REFLECTIVE_CTOR_INIT(VertexData, _flag{ 0 })

		VertexData(const fs::Float3& position, const fs::Float4& color)
			: IReflective()
			, _position{ position }
			, _flag{ 0 }
			, _color{ color }
		{
			__noop;
		}
		VertexData(const fs::Float3& position, const fs::Float2& texCoord)
			: IReflective()
			, _position{ position }
			, _flag{ 1 }
			, _texCoord{ texCoord }
		{
			__noop;
		}
		VertexData(const fs::Float3& position, const fs::Float4& color, const fs::Float2& texCoord)
			: IReflective()
			, _position{ position }
			, _flag{ 2 }
			, _color{ color }
			, _texCoord{ texCoord }
		{
			__noop;
		}

		FS_DECLARE_MEMBER(fs::Float3, _position);
		FS_DECLARE_MEMBER(uint32	, _flag);
		FS_DECLARE_MEMBER(fs::Float4, _color);
		FS_DECLARE_MEMBER(fs::Float2, _texCoord);
		FS_DECLARE_MEMBER(fs::Float2, ___reserved___);

		FS_REGISTER_BEGIN()
			FS_REGISTER_MEMBER(_position);
			FS_REGISTER_MEMBER(_flag);
			FS_REGISTER_MEMBER(_color);
			FS_REGISTER_MEMBER(_texCoord);
			FS_REGISTER_MEMBER(___reserved___);
		FS_REGISTER_END()
	};

	struct DxInputElement
	{
		D3D11_INPUT_ELEMENT_DESC	_inputElementDescriptor;
		fs::ScopeStringA<40>		_semanticName;
	};
	

	class GraphicDevice final
	{
		friend RectangleRenderer;

	public:
														GraphicDevice();
														~GraphicDevice() = default;

	public:
		void											initialize(const fs::Window::IWindow* const window);

	private:
		void											createDxDevice();

	private:
		void											createFontTextureFromMemory();

	public:
		void											beginRendering();
		void											endRendering();

	private:
		void											prepareTriangleDataBuffer();

	public:
		FS_INLINE fs::RectangleRenderer&				getRectangleRenderer() noexcept { return _rectangleRenderer; }

	private:
		const fs::Window::IWindow*						_window;

	private:
		float											_clearColor[4];

#pragma region DirectX
	private:
		ComPtr<IDXGISwapChain>							_swapChain;
		ComPtr<ID3D11Device>							_device;
		ComPtr<ID3D11DeviceContext>						_deviceContext;

	private:
		ComPtr<ID3D11Texture2D>							_backBuffer;
		ComPtr<ID3D11RenderTargetView>					_backBufferRtv;

	private:
		DxShaderHeaderMemory							_shaderHeaderMemory;
		std::vector<DxInputElement>						_inputElementArray;
		ComPtr<ID3D11InputLayout>						_inputLayout;
		ComPtr<ID3D10Blob>								_vertexShaderBlob;
		ComPtr<ID3D11VertexShader>						_vertexShader;
		ComPtr<ID3D10Blob>								_pixelShaderBlob;
		ComPtr<ID3D11PixelShader>						_pixelShader;
		ComPtr<ID3D11SamplerState>						_samplerState;
		ComPtr<ID3D11BlendState>						_blendState;

	private:
		static constexpr uint32							kFontTextureWidth		= 16 * kBitsPerByte;
		static constexpr uint32							kFontTextureHeight		= 60;
		static constexpr uint32							kFontTexturePixelCount	= kFontTextureWidth * kFontTextureHeight;
		StaticArray<uint8, kFontTexturePixelCount * 4>	_fontTextureRaw;
		ComPtr<ID3D11ShaderResourceView>				_fontTextureSrv;

	private:
		using TriangleIndexType							= uint16;

		uint32											_cachedTriangleVertexCount;
		ComPtr<ID3D11Buffer>							_triangleVertexBuffer;
		std::vector<VertexData>							_triangleVertexArray;
		uint32											_triangleVertexStride;
		uint32											_triangleVertexOffset;

		uint32											_cachedTriangleIndexCount;
		std::vector<TriangleIndexType>					_triangleIndexArray;
		ComPtr<ID3D11Buffer>							_triangleIndexBuffer;
		uint32											_triangleIndexOffset;
#pragma endregion

	private:
		fs::RectangleRenderer							_rectangleRenderer;
	};
}


#endif // !FS_GRAPHIC_DEVICE_H
