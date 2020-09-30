#include <stdafx.h>
#include <SimpleRendering\DxHelper.h>
#include <Container/StaticArray.h>


namespace fs
{
	std::string convertDeclarationNameToHlslSemanticName(const std::string& declarationName)
	{
		std::string semanticName = declarationName.substr(1);
		const uint32 semanticNameLength = static_cast<uint32>(semanticName.length());
		for (uint32 semanticNameIter = 0; semanticNameIter < semanticNameLength; semanticNameIter++)
		{
			semanticName[semanticNameIter] = ::toupper(semanticName[semanticNameIter]);
		}
		return semanticName;
	}
	static constexpr fs::StaticArray<fs::StaticArray<const char*, 2>, 5> kHlslTypeMatchingTable
	{
		{ "Float2"           , "float2"      },
		{ "Float3"           , "float3"      },
		{ "Float4"           , "float4"      },
		{ "Float4x4"         , "float4x4"    },
		{ "unsigned int"     , "uint"        },
	};
	static std::unordered_map<std::string, std::string> sHlslTypeMap;
	static constexpr fs::StaticArray<fs::StaticArray<const char*, 2>, 1> kHlslSemanticMatchingTable
	{
		{ "POSITION"         , "SV_POSITION" },
	};
	static std::unordered_map<std::string, std::string> sHlslSemanticMap;
	static void prepareStaticMaps()
	{
		if (sHlslTypeMap.empty() == true)
		{
			for (uint32 typeMapElementIndex = 0; typeMapElementIndex < kHlslTypeMatchingTable.size(); typeMapElementIndex++)
			{
				sHlslTypeMap.insert(std::make_pair(kHlslTypeMatchingTable[typeMapElementIndex][0], kHlslTypeMatchingTable[typeMapElementIndex][1]));
			}
		}
		if (sHlslSemanticMap.empty() == true)
		{
			for (uint32 semanticMapElementIndex = 0; semanticMapElementIndex < kHlslSemanticMatchingTable.size(); semanticMapElementIndex++)
			{
				sHlslSemanticMap.insert(std::make_pair(kHlslSemanticMatchingTable[semanticMapElementIndex][0], kHlslSemanticMatchingTable[semanticMapElementIndex][1]));
			}
		}
	}

	std::string convertReflectiveClassToHlslStruct(const fs::IReflective* const reflective, bool mapSemanticNames)
	{
		prepareStaticMaps();

		std::string result;
		result.append("struct ");
		result.append(reflective->getType().typeName());
		result.append("\n{\n");
		std::string semanticName;
		const uint32 memberCount = reflective->getMemberCount();
		for (uint32 memberIndex = 0; memberIndex < memberCount; memberIndex++)
		{
			const fs::ReflectionTypeData& memberType = reflective->getMemberType(memberIndex);
			auto found = sHlslTypeMap.find(memberType.typeName());
			result.push_back('\t');
			if (found != sHlslTypeMap.end())
			{
				result.append(found->second);
			}
			else
			{
				result.append(memberType.typeName());
			}
			result.append(" ");
			result.append(memberType.declarationName());
			result.append(" : ");

			semanticName = convertDeclarationNameToHlslSemanticName(memberType.declarationName());
			if (true == mapSemanticNames)
			{
				auto found = sHlslSemanticMap.find(semanticName);
				if (found != sHlslSemanticMap.end())
				{
					result.append(found->second);
				}
				else
				{
					result.append(semanticName);
				}
			}
			else
			{
				result.append(semanticName);
			}
			result.append(";\n");
		}
		result.append("};\n\n");
		return result;
	}

	std::string convertReflectiveClassToHlslConstantBuffer(const fs::IReflective* const reflective, const uint32 registerIndex)
	{
		prepareStaticMaps();

		std::string result;
		result.append("cbuffer ");
		result.append(reflective->getType().typeName());
		result.append(" : register(");
		result.append("b" + std::to_string(registerIndex));
		result.append(")\n{\n");
		const uint32 memberCount = reflective->getMemberCount();
		for (uint32 memberIndex = 0; memberIndex < memberCount; memberIndex++)
		{
			const fs::ReflectionTypeData& memberType = reflective->getMemberType(memberIndex);
			auto found = sHlslTypeMap.find(memberType.typeName());
			result.push_back('\t');
			if (found != sHlslTypeMap.end())
			{
				result.append(found->second);
			}
			else
			{
				result.append(memberType.typeName());
			}
			result.append(" ");
			result.append(memberType.declarationName());
			result.append(";\n");
		}
		result.append("};\n\n");
		return result;
	}
}
