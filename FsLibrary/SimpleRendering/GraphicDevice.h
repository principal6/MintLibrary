﻿#pragma once


#ifndef FS_GRAPHIC_DEVICE_H
#define FS_GRAPHIC_DEVICE_H


#include <CommonDefinitions.h>
#include <Container/StaticArray.h>
#include <Container/ScopeString.h>
#include <Container/UniqueString.h>
#include <SimpleRendering/RectangleRenderer.h>
#include <SimpleRendering/DxShaderHeaderMemory.h>
#include <SimpleRendering/DxShader.h>
#include <SimpleRendering/DxBuffer.h>
#include <Reflection/IReflective.h>
#include <Math/Float4x4.h>


#define FS_HLSL_CLASS class alignas(16)


namespace fs
{
	namespace Window
	{
		class IWindow;
	}
	using Microsoft::WRL::ComPtr;

	FS_HLSL_CLASS VS_INPUT : public IReflective
	{
		FS_REFLECTIVE_CTOR_INIT(VS_INPUT, _flag{ 0 })

		VS_INPUT(const fs::Float3& position, const fs::Float4& color)
			: IReflective()
			, _position{ position }
			, _color{ color }
			, _flag{ 0 }
		{
			__noop;
		}
		VS_INPUT(const fs::Float3& position, const fs::Float2& texCoord)
			: IReflective()
			, _position{ position }
			, _texCoord{ texCoord }
			, _flag{ 1 }
		{
			__noop;
		}
		VS_INPUT(const fs::Float3& position, const fs::Float4& color, const fs::Float2& texCoord)
			: IReflective()
			, _position{ position }
			, _color{ color }
			, _texCoord{ texCoord }
			, _flag{ 2 }
		{
			__noop;
		}

		FS_DECLARE_MEMBER(fs::Float4, _position);
		FS_DECLARE_MEMBER(fs::Float4, _color);
		FS_DECLARE_MEMBER(fs::Float2, _texCoord);
		FS_DECLARE_MEMBER(uint32	, _flag);

		FS_REGISTER_BEGIN()
			FS_REGISTER_MEMBER(_position);
			FS_REGISTER_MEMBER(_color);
			FS_REGISTER_MEMBER(_texCoord);
			FS_REGISTER_MEMBER(_flag);
		FS_REGISTER_END()
	};

	FS_HLSL_CLASS VS_OUTPUT : public IReflective
	{
		FS_REFLECTIVE_CTOR_INIT(VS_OUTPUT, _flag{ 0 })

		FS_DECLARE_MEMBER(fs::Float4, _position);
		FS_DECLARE_MEMBER(fs::Float4, _color);
		FS_DECLARE_MEMBER(fs::Float2, _texCoord);
		FS_DECLARE_MEMBER(uint32	, _flag);

		FS_REGISTER_BEGIN()
			FS_REGISTER_MEMBER(_position);
			FS_REGISTER_MEMBER(_color);
			FS_REGISTER_MEMBER(_texCoord);
			FS_REGISTER_MEMBER(_flag);
		FS_REGISTER_END()
	};

	FS_HLSL_CLASS CB_Transforms : public IReflective
	{
		FS_REFLECTIVE_CTOR(CB_Transforms)

		FS_DECLARE_MEMBER(fs::Float4x4, _cbProjectionMatrix);

		FS_REGISTER_BEGIN()
			FS_REGISTER_MEMBER(_cbProjectionMatrix);
		FS_REGISTER_END()
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

	public:
		FS_INLINE ID3D11Device*							getDxDevice() noexcept { return _device.Get(); }
		FS_INLINE ID3D11DeviceContext*					getDxDeviceContext() noexcept { return _deviceContext.Get(); }

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
		DxShaderPool									_shaderPool;
		DxBufferPool									_bufferPool;

		ComPtr<ID3D11SamplerState>						_samplerState;
		ComPtr<ID3D11BlendState>						_blendState;

	private:
		static constexpr uint32							kFontTextureWidth		= 16 * kBitsPerByte;
		static constexpr uint32							kFontTextureHeight		= 60;
		static constexpr uint32							kFontTexturePixelCount	= kFontTextureWidth * kFontTextureHeight;
		std::vector<uint8>								_fontTextureRaw;
		ComPtr<ID3D11ShaderResourceView>				_fontTextureSrv;

	private:
		using TriangleIndexType							= uint16;

		uint32											_cachedTriangleVertexCount;
		ComPtr<ID3D11Buffer>							_triangleVertexBuffer;
		std::vector<VS_INPUT>							_triangleVertexArray;
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
