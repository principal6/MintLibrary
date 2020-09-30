#pragma once


#ifndef FS_IDX_BASE_H
#define FS_IDX_BASE_H


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


	class DxObjectId final
	{
		friend IDxObject;
		static constexpr uint32		kDxInvalidObjectRawId{ kUint32Max };

	public:
									DxObjectId() = default;
									DxObjectId(const DxObjectId& rhs) = default;
									DxObjectId(DxObjectId&& rhs) = default;
									~DxObjectId() = default;

	public:
		FS_INLINE const bool		operator==(const DxObjectId& rhs) const noexcept
		{
			return _rawId == rhs._rawId;
		}

		FS_INLINE const bool		operator<(const DxObjectId& rhs) const noexcept
		{
			return _rawId < rhs._rawId;
		}

		FS_INLINE const bool		operator>(const DxObjectId& rhs) const noexcept
		{
			return _rawId > rhs._rawId;
		}

	public:
		FS_INLINE const bool		isValid() const noexcept
		{
			return _rawId != kDxInvalidObjectRawId;
		}

	private:
		FS_INLINE void				assignIdXXX() noexcept
		{
			++_lastRawId;
			_rawId = _lastRawId;
		}

	private:
		uint32						_rawId{ kDxInvalidObjectRawId };

	private:
		static std::atomic<uint32>	_lastRawId;
	
	public:
		static const DxObjectId		kInvalidObjectId;
	};


	class IDxObject abstract
	{
	public:
									IDxObject(GraphicDevice* const graphicDevice) : _graphicDevice{ graphicDevice} {}
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
		const DxObjectId&			getId() const noexcept
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


#endif // !FS_IDX_BASE_H
