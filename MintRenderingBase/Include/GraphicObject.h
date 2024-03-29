﻿#pragma once


#ifndef _MINT_RENDERING_BASE_GRAPHIC_OBJECT_H_
#define _MINT_RENDERING_BASE_GRAPHIC_OBJECT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;
		class GraphicObject;


		enum class GraphicShaderType
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
			InputLayout,
			Shader,
		};


		class GraphicObjectID final
		{
			friend GraphicObject;
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
			MINT_INLINE bool IsValid() const noexcept
			{
				return _rawID != kInvalidGraphicObjectRawID;
			}

			MINT_INLINE GraphicObjectType GetObjectType() const noexcept
			{
				return _objectType;
			}

			MINT_INLINE bool IsObjectType(const GraphicObjectType objectType) const noexcept
			{
				return _objectType == objectType;
			}

			MINT_INLINE void Invalidate() noexcept
			{
				_rawID = kInvalidGraphicObjectRawID;
			}

		private:
			MINT_INLINE void AssignIDXXX() noexcept
			{
				++_lastRawID;
				_rawID = _lastRawID;
			}

			MINT_INLINE void SetObjectTypeXXX(const GraphicObjectType objectType) noexcept
			{
				_objectType = objectType;
			}

		private:
			uint32 _rawID{ kInvalidGraphicObjectRawID };
			GraphicObjectType _objectType{ GraphicObjectType::None };

		private:
			static std::atomic<uint32> _lastRawID;

		public:
			static const GraphicObjectID kInvalidGraphicObjectID;
		};


		class GraphicObject
		{
		public:
			GraphicObject(GraphicDevice& graphicDevice, const GraphicObjectType objectType)
				: _graphicDevice{ graphicDevice }
				, _objectID{}
			{
				_objectID.SetObjectTypeXXX(objectType);
			}
			GraphicObject(const GraphicObject& rhs) = delete;
			GraphicObject(GraphicObject&& rhs) noexcept
				: _graphicDevice{ rhs._graphicDevice }
				, _objectID{ std::move(rhs._objectID) }
			{
				__noop;
			}
			virtual ~GraphicObject() = default;

		public:
			GraphicObject& operator=(const GraphicObject& rhs) = delete;
			GraphicObject& operator=(GraphicObject&& rhs) noexcept
			{
				if (this != &rhs)
				{
					_objectID = std::move(rhs._objectID);
				}
				return *this;
			}

		public:
			struct Evaluator
			{
				const GraphicObjectID& operator()(const GraphicObject& rhs) const noexcept
				{
					return rhs._objectID;
				}
			};

			struct AscendingComparator
			{
				bool operator()(const GraphicObject& lhs, const GraphicObject& rhs) const noexcept
				{
					return lhs._objectID < rhs._objectID;
				}
			};

			MINT_INLINE bool operator==(const GraphicObjectID& objectID) const noexcept
			{
				return _objectID == objectID;
			}

		public:
			MINT_INLINE const GraphicObjectID& GetID() const noexcept
			{
				return _objectID;
			}

		protected:
			MINT_INLINE void AssignIDXXX() noexcept
			{
				_objectID.AssignIDXXX();
			}

		protected:
			GraphicDevice& _graphicDevice;
			GraphicObjectID _objectID;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_GRAPHIC_OBJECT_H_
