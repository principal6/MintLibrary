#pragma once


#ifndef FS_I_RENDERER_H
#define FS_I_RENDERER_H


#include <Math/Float2.h>
#include <Math/Float3.h>
#include <Math/Float4.h>

#include <SimpleRendering/IDxObject.h>


namespace fs
{
	namespace SimpleRendering
	{
		class GraphicDevice;


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
			void									setColor(const fs::Float4& color) noexcept;
			void									setColor(const std::vector<fs::Float4>& colorArray) noexcept;

		protected:
			const fs::Float4&						getColorInternal(const uint32 index) const noexcept;
		
		protected:
			fs::SimpleRendering::GraphicDevice*		_graphicDevice;
			fs::Float3								_position;
			std::vector<fs::Float4>					_colorArray;
			fs::Float4								_defaultColor;
		};
	}
}


#include <SimpleRendering/IRenderer.inl>


#endif // !FS_I_RENDERER_H
