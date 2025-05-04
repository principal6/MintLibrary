#pragma once


#ifndef _MINT_RENDERING_BASE_GRAPHICS_OBJECT_H_
#define _MINT_RENDERING_BASE_GRAPHICS_OBJECT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>

namespace mint
{
	template<typename T>
	class OwnPtr;
	template<typename T>
	class RefCounted;

	namespace Rendering
	{
		class GraphicsDevice;
		class GraphicsObject;
	}
}

namespace mint
{
	namespace Rendering
	{
		enum class GraphicsShaderType
		{
			VertexShader,
			GeometryShader,
			PixelShader,

			COUNT,
		};


		enum class GraphicsObjectType
		{
			None,
			Resource,
			InputLayout,
			Shader,
			ShaderPipeline,
		};


		class GraphicsObjectID final
		{
			friend GraphicsObject;
			static constexpr uint32 kInvalidGraphicsObjectRawID{ kUint32Max };

		public:
			GraphicsObjectID() = default;
			GraphicsObjectID(const GraphicsObjectID& rhs) = default;
			GraphicsObjectID(GraphicsObjectID&& rhs) = default;
			~GraphicsObjectID() = default;

		public:
			GraphicsObjectID& operator=(const GraphicsObjectID& rhs) noexcept = default;
			GraphicsObjectID& operator=(GraphicsObjectID&& rhs) noexcept = default;

		public:
			MINT_INLINE bool operator==(const GraphicsObjectID& rhs) const noexcept
			{
				return _rawID == rhs._rawID;
			}

			MINT_INLINE bool operator!=(const GraphicsObjectID& rhs) const noexcept
			{
				return _rawID != rhs._rawID;
			}

			MINT_INLINE bool operator<(const GraphicsObjectID& rhs) const noexcept
			{
				return _rawID < rhs._rawID;
			}

			MINT_INLINE bool operator>(const GraphicsObjectID& rhs) const noexcept
			{
				return _rawID > rhs._rawID;
			}

		public:
			MINT_INLINE bool IsValid() const noexcept
			{
				return _rawID != kInvalidGraphicsObjectRawID;
			}

			MINT_INLINE GraphicsObjectType GetObjectType() const noexcept
			{
				return _objectType;
			}

			MINT_INLINE bool IsObjectType(const GraphicsObjectType objectType) const noexcept
			{
				return _objectType == objectType;
			}

			MINT_INLINE void Invalidate() noexcept
			{
				_rawID = kInvalidGraphicsObjectRawID;
			}

		private:
			MINT_INLINE void AssignIDXXX() noexcept
			{
				++_lastRawID;
				_rawID = _lastRawID;
			}

			MINT_INLINE void SetObjectTypeXXX(const GraphicsObjectType objectType) noexcept
			{
				_objectType = objectType;
			}

		private:
			uint32 _rawID{ kInvalidGraphicsObjectRawID };
			GraphicsObjectType _objectType{ GraphicsObjectType::None };

		private:
			static std::atomic<uint32> _lastRawID;

		public:
			static const GraphicsObjectID kInvalidGraphicsObjectID;
		};


		class GraphicsObject abstract
		{
		public:
			GraphicsObject(GraphicsDevice& graphicsDevice, const GraphicsObjectType objectType)
				: _graphicsDevice{ graphicsDevice }
				, _objectID{}
			{
				_objectID.SetObjectTypeXXX(objectType);
			}
			GraphicsObject(const GraphicsObject& rhs) = delete;
			GraphicsObject(GraphicsObject&& rhs) noexcept
				: _graphicsDevice{ rhs._graphicsDevice }
				, _objectID{ std::move(rhs._objectID) }
			{
				__noop;
			}
			virtual ~GraphicsObject() = default;

		public:
			GraphicsObject& operator=(const GraphicsObject& rhs) = delete;
			GraphicsObject& operator=(GraphicsObject&& rhs) noexcept
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
				const GraphicsObjectID& operator()(const GraphicsObject& rhs) const noexcept;
				const GraphicsObjectID& operator()(const OwnPtr<GraphicsObject>& rhs) const noexcept;
				template<typename T>
				const GraphicsObjectID& operator()(const RefCounted<T>& rhs) const noexcept
				{
					return operator()(static_cast<const GraphicsObject&>(*rhs));
				}
			};

			struct AscendingComparator
			{
				bool operator()(const GraphicsObject& lhs, const GraphicsObject& rhs) const noexcept;
				bool operator()(const OwnPtr<GraphicsObject>& lhs, const OwnPtr<GraphicsObject>& rhs) const noexcept;
				template<typename T>
				bool operator()(const RefCounted<T>& lhs, const RefCounted<T>& rhs) const noexcept
				{
					return operator()(static_cast<const GraphicsObject&>(*lhs), static_cast<const GraphicsObject&>(*rhs));
				}
			};

			MINT_INLINE bool operator==(const GraphicsObjectID& objectID) const noexcept
			{
				return _objectID == objectID;
			}

		public:
			MINT_INLINE const GraphicsObjectID& GetID() const noexcept
			{
				return _objectID;
			}

		protected:
			MINT_INLINE void AssignIDXXX() noexcept
			{
				_objectID.AssignIDXXX();
			}

		protected:
			GraphicsDevice& _graphicsDevice;
			GraphicsObjectID _objectID;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_GRAPHICS_OBJECT_H_
