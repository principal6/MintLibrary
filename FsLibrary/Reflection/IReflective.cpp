#include <stdafx.h>
#include <Reflection/IReflective.h>


namespace fs
{
	uint32 ReflectionPool::registerType(const std::type_info& type, const size_t typeSize, const std::type_info& member, const std::string memberName, const size_t memberSize)
	{
		const uint32 typeCount = static_cast<uint32>(getInstance()._typeArray.size());
		for (uint32 typeIndex = 0; typeIndex < typeCount; typeIndex++)
		{
			if (getInstance()._typeArray[typeIndex]._isRegisterDone == false && getInstance()._typeArray[typeIndex]._type == type)
			{
				getInstance()._typeArray[typeIndex]._memberArray.emplace_back(member);
				getInstance()._typeArray[typeIndex]._memberArray.back()._type._typeSize = memberSize;
				getInstance()._typeArray[typeIndex]._memberArray.back()._declarationName = memberName;
				return typeIndex;
			}
		}

		// New type added
		getInstance()._typeArray.emplace_back(type);
		getInstance()._typeArray.back()._type._isReflective = true;
		getInstance()._typeArray.back()._type._typeSize = typeSize;
		getInstance()._typeArray.back()._memberArray.emplace_back(member);
		getInstance()._typeArray.back()._memberArray.back()._type._typeSize = memberSize;
		getInstance()._typeArray.back()._memberArray.back()._declarationName = memberName;
		return static_cast<uint32>(getInstance()._typeArray.size());
	}

	void ReflectionPool::registerTypeDone(const uint32 typeIndex)
	{
		getInstance()._typeArray[typeIndex]._isRegisterDone = true;
	}

	const ReflectionTypeData& ReflectionPool::getTypeData(const uint32 typeIndex)
	{
		return getInstance()._typeArray[typeIndex];
	}

	ReflectionPool& ReflectionPool::getInstance()
	{
		static ReflectionPool reflectionPool;
		return reflectionPool;
	}

}
