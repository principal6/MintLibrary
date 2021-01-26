﻿#pragma once


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
								Color(const int32 r, const int32 g, const int32 b, const int32 a) : Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f) {}
								Color(const float r, const float g, const float b) : Color(r, g, b, 1.0f) {}
								Color(const int32 r, const int32 g, const int32 b) : Color(r, g, b, 255) {}
			explicit			Color(const fs::Float4& float4) : Color(float4._x, float4._y, float4._z, float4._w) {}

		private:
			fs::Float4			_raw;

		public:
								operator fs::Float4&() noexcept { return _raw; }
								operator const fs::Float4&() const noexcept { return _raw; }
			Color				operator*(const Color& rhs) const noexcept { return Color(_raw._x * rhs._raw._x, _raw._y * rhs._raw._y, _raw._z * rhs._raw._z, _raw._w * rhs._raw._w); }
			Color				operator*(const float s) const noexcept { return Color(_raw * s); }
			Color				operator/(const float s) const { return Color(_raw / s); }
			Color				operator+(const Color& rhs) const { return Color(_raw + rhs._raw); }
			Color				operator-(const Color& rhs) const { return Color(_raw - rhs._raw); }

		public:
			const float			r() const noexcept { return _raw._x; }
			const float			g() const noexcept { return _raw._y; }
			const float			b() const noexcept { return _raw._z; }
			const float			a() const noexcept { return _raw._w; }

			void				r(const int32 value) noexcept { _raw._x = (value / 255.0f); }
			void				g(const int32 value) noexcept { _raw._y = (value / 255.0f); }
			void				b(const int32 value) noexcept { _raw._z = (value / 255.0f); }
			void				a(const int32 value) noexcept { _raw._w = (value / 255.0f); }

			void				r(const float value) noexcept { _raw._x = value; }
			void				g(const float value) noexcept { _raw._y = value; }
			void				b(const float value) noexcept { _raw._z = value; }
			void				a(const float value) noexcept { _raw._w = value; }

		public:
			Color				scaleRgb(const float s) const noexcept { return Color(_raw._x * s, _raw._y * s, _raw._z * s, _raw._w); }
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
			void									setUseMultipleViewports() noexcept;
			const bool								getUseMultipleViewports() const noexcept;

		public:
			void									setPosition(const fs::Float3& position) noexcept;
			void									setColor(const fs::SimpleRendering::Color& color) noexcept;
			void									setColor(const std::vector<fs::SimpleRendering::Color>& colorArray) noexcept;

		protected:
			const fs::Float4&						getColorInternal(const uint32 index) const noexcept;
		
		protected:
			fs::SimpleRendering::GraphicDevice*		_graphicDevice;
		
		protected:
			fs::Float3								_position;
			std::vector<SimpleRendering::Color>		_colorArray;
			fs::SimpleRendering::Color				_defaultColor;
		
		private:
			bool									_useMultipleViewports;
		};
	}
}


#include <FsLibrary/SimpleRendering/IRenderer.inl>


#endif // !FS_I_RENDERER_H
