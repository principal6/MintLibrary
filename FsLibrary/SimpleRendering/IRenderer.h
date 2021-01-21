#pragma once


#ifndef FS_I_RENDERER_H
#define FS_I_RENDERER_H


#include <FsLibrary/SimpleRendering/SimpleRenderingCommon.h>

#include <FsMath/Include/Float2.h>
#include <FsMath/Include/Float3.h>
#include <FsMath/Include/Float4.h>

#include <FsLibrary/SimpleRendering/IDxObject.h>


namespace fs
{
	namespace SimpleRendering
	{
		class GraphicDevice;


		struct Color
		{
		public:
								Color() : Color(255, 255, 255) {}
								Color(const float r, const float g, const float b, const float a) : _raw{ r, g, b, a } {}
								Color(const int r, const int g, const int b, const int a) : Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f) {}
								Color(const float r, const float g, const float b) : Color(r, g, b, 1.0f) {}
								Color(const int r, const int g, const int b) : Color(r, g, b, 255) {}
			explicit			Color(const fs::Float4& float4) : Color(float4._x, float4._y, float4._z, float4._w) {}

		private:
			fs::Float4			_raw;

		public:
								operator fs::Float4&() noexcept { return _raw; }
								operator const fs::Float4&() const noexcept { return _raw; }
			Color				operator*(const float s) const noexcept { return Color(_raw * s); }
			Color				operator/(const float s) const { return Color(_raw / s); }
			Color				operator+(const Color& rhs) const { return Color(_raw + rhs._raw); }
			Color				operator-(const Color& rhs) const { return Color(_raw - rhs._raw); }
		};


		class IRenderer abstract
		{
		public:
													IRenderer() = default;
													IRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice);
			virtual									~IRenderer() = default;

		public:
			virtual void							initializeShaders() noexcept abstract;
			virtual void							flushData() noexcept abstract;
			virtual void							render() noexcept abstract;

		public:
			void									setPosition(const fs::Float3& position) noexcept;
			void									setColor(const fs::SimpleRendering::Color& color) noexcept;
			void									setColor(const std::vector<fs::SimpleRendering::Color>& colorArray) noexcept;

		protected:
			const fs::Float4&						getColorInternal(const uint32 index) const noexcept;
		
		protected:
			fs::SimpleRendering::GraphicDevice*		_graphicDevice;
			fs::Float3								_position;
			std::vector<SimpleRendering::Color>		_colorArray;
			fs::SimpleRendering::Color				_defaultColor;
		};
	}
}


#include <FsLibrary/SimpleRendering/IRenderer.inl>


#endif // !FS_I_RENDERER_H
