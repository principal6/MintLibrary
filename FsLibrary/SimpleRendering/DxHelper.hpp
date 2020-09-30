#pragma once

#include <stdafx.h>
#include <SimpleRendering\DxHelper.h>

#include <Math/Float2.hpp>
#include <Math/Float3.hpp>
#include <Math/Float4.hpp>


namespace fs
{
	FS_INLINE DXGI_FORMAT convertToDxgiFormat(const ReflectionTypeData& typeData)
	{
		if (typeData == typeid(fs::Float2))
		{
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (typeData == typeid(fs::Float3))
		{
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (typeData == typeid(fs::Float4))
		{
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		else if (typeData == typeid(uint32))
		{
			return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
		}
		return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	}

	FS_INLINE DXGI_FORMAT convertToDxgiFormat(const type_info& typeInfo)
	{
		return convertToDxgiFormat(ReflectionTypeData(typeInfo));
	}
}
