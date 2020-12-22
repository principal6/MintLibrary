#pragma once


#ifndef FS_I_DX_BASE_H
#define FS_I_DX_BASE_H


#include <CommonDefinitions.h>


namespace fs
{
	class GraphicDevice;
	class IDxObject;


	enum class DxShaderType
	{
		VertexShader,
		PixelShader,
	};


	enum class DxObjectType
	{
		None,
		Pool,
		Buffer,
		Shader,
	};


	class DxObjectId final
	{
		friend IDxObject;
		static constexpr uint32			kDxInvalidObjectRawId{ kUint32Max };

	public:
										DxObjectId() = default;
										DxObjectId(const DxObjectId& rhs) = default;
										DxObjectId(DxObjectId&& rhs) = default;
										~DxObjectId() = default;

	public:
		DxObjectId&						operator=(const DxObjectId& rhs) noexcept = default;
		DxObjectId&						operator=(DxObjectId&& rhs) noexcept = default;
	
	public:
		FS_INLINE const bool			operator==(const DxObjectId& rhs) const noexcept
		{
			return _rawId == rhs._rawId;
		}

		FS_INLINE const bool			operator<(const DxObjectId& rhs) const noexcept
		{
			return _rawId < rhs._rawId;
		}

		FS_INLINE const bool			operator>(const DxObjectId& rhs) const noexcept
		{
			return _rawId > rhs._rawId;
		}

	public:
		FS_INLINE const bool			isValid() const noexcept
		{
			return _rawId != kDxInvalidObjectRawId;
		}

		FS_INLINE const DxObjectType	getObjectType() const noexcept
		{
			return _objectType;
		}

		FS_INLINE const bool			isObjectType(const DxObjectType objectType) const noexcept
		{
			return _objectType == objectType;
		}

	private:
		FS_INLINE void					assignIdXXX() noexcept
		{
			++_lastRawId;
			_rawId = _lastRawId;
		}

		FS_INLINE void					setObjectTypeXXX(const DxObjectType objectType) noexcept
		{
			_objectType = objectType;
		}

	private:
		uint32							_rawId{ kDxInvalidObjectRawId };
		DxObjectType					_objectType{ DxObjectType::None };

	private:
		static std::atomic<uint32>		_lastRawId;
	
	public:
		static const DxObjectId			kInvalidObjectId;
	};


	class IDxObject abstract
	{
	public:
									IDxObject(GraphicDevice* const graphicDevice, const DxObjectType objectType) : _graphicDevice{ graphicDevice} { _objectId.setObjectTypeXXX(objectType); }
		virtual						~IDxObject() = default;

	public:
		FS_INLINE const bool		operator==(const DxObjectId& objectId) const noexcept
		{
			return objectId == _objectId;
		}

		FS_INLINE const bool		operator<(const DxObjectId& objectId) const noexcept
		{
			return objectId < _objectId;
		}

		FS_INLINE const bool		operator>(const DxObjectId& objectId) const noexcept
		{
			return objectId > _objectId;
		}

	public:
		FS_INLINE const DxObjectId&	getId() const noexcept
		{
			return _objectId;
		}

	protected:
		FS_INLINE void				assignIdXXX() noexcept
		{
			_objectId.assignIdXXX();
		}

	protected:
		GraphicDevice*				_graphicDevice{ nullptr };
		DxObjectId					_objectId{};
	};
}


#endif // !FS_I_DX_BASE_H
