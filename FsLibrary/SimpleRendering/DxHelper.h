#pragma once


#ifndef FS_DX_HELPER_H
#define FS_DX_HELPER_H


#include <CommonDefinitions.h>
#include <Reflection/IReflective.h>


namespace fs
{
	DXGI_FORMAT convertToDxgiFormat(const ReflectionTypeData& typeData);
	DXGI_FORMAT convertToDxgiFormat(const type_info& typeInfo);

	std::string convertDeclarationNameToHlslSemanticName(const std::string& declarationName);
	std::string convertReflectiveClassToHlslStruct(const fs::IReflective* const reflective, bool mapSemanticNames);
	std::string convertReflectiveClassToHlslConstantBuffer(const fs::IReflective* const reflective, const uint32 registerIndex);

	template <typename T>
	const bool isNullInstance(const T& instance)
	{
		return &instance != &T::kNullInstance;
	}
}


#endif // !FS_DX_HELPER_H
