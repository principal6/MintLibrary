#include <stdafx.h>
#include <Reflection/IReflective.h>


namespace fs
{
	uint32 ReflectionPool::registerType(const std::type_info& type, const size_t byteOffset, const size_t typeSize, const std::type_info& member, const fs::DynamicStringA& memberName, const size_t memberSize)
	{
		const uint32 typeCount = static_cast<uint32>(getInstance()._typeArray.size());
		for (uint32 typeIndex = 0; typeIndex < typeCount; typeIndex++)
		{
			if (getInstance()._typeArray[typeIndex] == type)
			{
				if (getInstance()._typeArray[typeIndex]._isRegisterDone == false)
				{
					getInstance()._typeArray[typeIndex]._memberArray.emplace_back(member);
					getInstance()._typeArray[typeIndex]._memberArray.back()._byteOffset = static_cast<uint32>(byteOffset);
					getInstance()._typeArray[typeIndex]._memberArray.back()._typeSize = static_cast<uint32>(memberSize);
					getInstance()._typeArray[typeIndex]._memberArray.back()._declarationName = memberName;
					getInstance()._typeArray[typeIndex]._compactTypeSize += static_cast<uint32>(memberSize);
					return typeIndex;
				}
				else
				{
					return typeIndex;
				}
			}
		}

		// New type added
		getInstance()._typeArray.emplace_back(type);
		getInstance()._typeArray.back()._isReflective = true;
		getInstance()._typeArray.back()._typeSize = static_cast<uint32>(typeSize);
		getInstance()._typeArray.back()._memberArray.emplace_back(member);
		getInstance()._typeArray.back()._memberArray.back()._byteOffset = static_cast<uint32>(byteOffset);
		getInstance()._typeArray.back()._memberArray.back()._typeSize = static_cast<uint32>(memberSize);
		getInstance()._typeArray.back()._memberArray.back()._declarationName = memberName;
		getInstance()._typeArray.back()._compactTypeSize += static_cast<uint32>(memberSize);
		return typeCount;
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
