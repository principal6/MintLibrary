#pragma once


#ifndef FS_TRIANGLE_BUFFER_H
#define FS_TRIANGLE_BUFFER_H


#include <FsLibrary/CommonDefinitions.h>

#include <FsLibrary/SimpleRendering/CppHlslStructs.h>


namespace fs
{
	namespace SimpleRendering
	{
		class GraphicDevice;


		using Microsoft::WRL::ComPtr;
		using TriangleIndexType = uint16;


		template <typename T>
		class TriangleBuffer
		{
		public:
															TriangleBuffer(fs::SimpleRendering::GraphicDevice* const graphicDevice);
															~TriangleBuffer() = default;

		public:
			std::vector<T>&									vertexArray() noexcept;
			std::vector<TriangleIndexType>&					indexArray() noexcept;

		public:
			void											flush();
			const bool										isReady() const noexcept;
			void											render();

		private:
			void											prepareBuffer();

		private:
			fs::SimpleRendering::GraphicDevice* const		_graphicDevice;
			const uint32									_vertexStride;
		
		private:
			uint32											_cachedVertexCount;
			std::vector<T>									_vertexArray;
			uint32											_vertexOffset;
			ComPtr<ID3D11Buffer>							_vertexBuffer;

		private:
			uint32											_cachedIndexCount;
			std::vector<TriangleIndexType>					_indexArray;
			uint32											_indexOffset;
			ComPtr<ID3D11Buffer>							_indexBuffer;
		};
	}
}


#endif // !FS_TRIANGLE_BUFFER_H
