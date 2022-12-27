#pragma once


#ifndef _MINT_RENDERING_BASE_I_DX_OBJECT_H_
#define _MINT_RENDERING_BASE_I_DX_OBJECT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;
		class IGraphicObject;


		enum class DxShaderType
		{
			VertexShader,
			GeometryShader,
			PixelShader,

			COUNT,
		};


		enum class GraphicObjectType
		{
			None,
			Pool,
			Resource,
			Shader,
		};


		class GraphicObjectID final
		{
			friend IGraphicObject;
			static constexpr uint32 kInvalidGraphicObjectRawID{ kUint32Max };

		public:
			GraphicObjectID() = default;
			GraphicObjectID(const GraphicObjectID& rhs) = default;
			GraphicObjectID(GraphicObjectID&& rhs) = default;
			~GraphicObjectID() = default;

		public:
			GraphicObjectID& operator=(const GraphicObjectID& rhs) noexcept = default;
			GraphicObjectID& operator=(GraphicObjectID&& rhs) noexcept = default;

		public:
			MINT_INLINE bool operator==(const GraphicObjectID& rhs) const noexcept
			{
				return _rawID == rhs._rawID;
			}

			MINT_INLINE bool operator!=(const GraphicObjectID& rhs) const noexcept
			{
				return _rawID != rhs._rawID;
			}

			MINT_INLINE bool operator<(const GraphicObjectID& rhs) const noexcept
			{
				return _rawID < rhs._rawID;
			}

			MINT_INLINE bool operator>(const GraphicObjectID& rhs) const noexcept
			{
				return _rawID > rhs._rawID;
			}

		public:
			MINT_INLINE bool isValid() const noexcept
			{
				return _rawID != kInvalidGraphicObjectRawID;
			}

			MINT_INLINE GraphicObjectType getObjectType() const noexcept
			{
				return _objectType;
			}

			MINT_INLINE bool isObjectType(const GraphicObjectType objectType) const noexcept
			{
				return _objectType == objectType;
			}

		private:
			MINT_INLINE void assignIDXXX() noexcept
			{
				++_lastRawID;
				_rawID = _lastRawID;
			}

			MINT_INLINE void setObjectTypeXXX(const GraphicObjectType objectType) noexcept
			{
				_objectType = objectType;
			}

		private:
			uint32 _rawID{ kInvalidGraphicObjectRawID };
			GraphicObjectType _objectType{ GraphicObjectType::None };

		private:
			static std::atomic<uint32> _lastRawID;

		public:
			static const GraphicObjectID kInvalidObjectID;
		};


		class IGraphicObject abstract
		{
		public:
			IGraphicObject(GraphicDevice& graphicDevice, const GraphicObjectType objectType)
				: _graphicDevice{ graphicDevice }
				, _objectID{}
			{
				_objectID.setObjectTypeXXX(objectType);
			}
			virtual ~IGraphicObject() = default;

		public:
			struct Evaluator
			{
				const GraphicObjectID& operator()(const IGraphicObject& rhs) const noexcept
				{
					return rhs._objectID;
				}
			};

			MINT_INLINE bool operator==(const GraphicObjectID& objectID) const noexcept
			{
				return _objectID == objectID;
			}

		public:
			MINT_INLINE const GraphicObjectID& getID() const noexcept
			{
				return _objectID;
			}

		protected:
			MINT_INLINE void assignIDXXX() noexcept
			{
				_objectID.assignIDXXX();
			}

		protected:
			GraphicDevice& _graphicDevice;
			GraphicObjectID _objectID;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_I_DX_OBJECT_H_
