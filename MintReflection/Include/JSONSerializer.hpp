#pragma once


#include <MintReflection/Include/JSONSerializer.h>
#include <MintContainer/Include/StringReference.hpp>
#include <MintContainer/Include/StackString.hpp>


#pragma optimize("", off)


namespace mint
{
	inline JSONSerializer::JSONSerializer()
	{
		__noop;
	}

	inline JSONSerializer::~JSONSerializer()
	{
		__noop;
	}

	template<typename T>
	inline bool JSONSerializer::Serialize(const T& from, const StringReferenceA& fileName)
	{
		if constexpr (IsReflectionClass<T>::value == false)
		{
			MINT_LOG_ERROR("'from' must be a reflection class!");
			return false;
		}

		_writer.Clear();

		_indents = true;

		SerializeInternal(0, StringReferenceA(""), from);

		return _writer.Save(fileName.CString());
	}

	template<typename CharType, typename T>
	inline void JSONSerializer::SerializeInternal(const uint32 depth, const StringReference<CharType>& declarationName, const T& from)
	{
		if constexpr (IsReflectionClass<T>::value == true)
		{
			SerializeHelper_Indent(depth);
			if (depth > 0)
			{
				SerializeHelper_Declaration(declarationName);
			}

			_writer.Write("{\n");

			const ReflectionData& reflectionData = from.GetReflectionData();
			const uint32 memberCount = reflectionData._memberTypeDatas.Size();
			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				const TypeBaseData& memberTypeData = *reflectionData._memberTypeDatas[memberIndex];
				memberTypeData.SerializeValue(*this, depth + 1, reinterpret_cast<const char*>(&from) + memberTypeData._offset, memberTypeData._arrayItemCount);

				if (memberIndex < memberCount - 1)
				{
					_writer.Write(",\n");
				}
				else
				{
					_writer.Write("\n");
				}
			}

			SerializeHelper_Indent(depth);

			_writer.Write("}");
		}
		else if constexpr (std::is_integral_v<T> == true || std::is_floating_point_v<T> == true)
		{
			SerializeHelper_Indent(depth);
			SerializeHelper_Declaration(declarationName);

			StackString<CharType, 256> buffer;
			StringUtil::ToString(from, buffer);
			_writer.Write(buffer.CString());
		}
		else
		{
			MINT_NEVER;
		}
	}

	template<typename CharType>
	inline void JSONSerializer::SerializeInternal(const uint32 depth, const StringReference<CharType>& declarationName, const String<CharType>& from)
	{
		SerializeHelper_Indent(depth);
		SerializeHelper_Declaration(declarationName);

		_writer.Write("\"");
		_writer.Write(from.CString());
		_writer.Write("\"");
	}

	template<typename CharType>
	inline void JSONSerializer::SerializeInternal(const uint32 depth, const StringReference<CharType>& declarationName, const StringReference<CharType>& from)
	{
		SerializeHelper_Indent(depth);
		SerializeHelper_Declaration(declarationName);

		_writer.Write("\"");
		_writer.Write(from.CString());
		_writer.Write("\"");
	}

	template<typename CharType, typename T>
	inline void JSONSerializer::SerializeInternal(const uint32 depth, const StringReference<CharType>& declarationName, const Vector<T>& from)
	{
		SerializeHelper_ArrayPrefix(depth, declarationName);

		const uint32 count = from.Size();
		for (uint32 index = 0; index < count; ++index)
		{
			SerializeHelper_ArrayItem(depth, declarationName, from[index], index == count - 1);
		}

		SerializeHelper_ArrayPostfix(depth);
	}

	inline void JSONSerializer::SerializeHelper_Indent(const uint32 depth)
	{
		if (depth == 0)
		{
			return;
		}

		if (_indents)
		{
			for (uint32 depthIter = 0; depthIter < depth; depthIter++)
			{
				_writer.Write("\t");
			}
		}
	}

	template<typename CharType>
	inline void JSONSerializer::SerializeHelper_Declaration(const StringReference<CharType>& declarationName)
	{
		_writer.Write("\"");
		_writer.Write(declarationName.CString());
		_writer.Write("\": ");
	}

	template<typename CharType>
	inline void JSONSerializer::SerializeHelper_ArrayPrefix(const uint32 depth, const StringReference<CharType>& declarationName)
	{
		SerializeHelper_Indent(depth);
		SerializeHelper_Declaration(declarationName);

		_writer.Write("[\n");
	}

	template<typename CharType, typename T>
	inline void JSONSerializer::SerializeHelper_ArrayItem(const uint32 depth, const StringReference<CharType>& declarationName, const T& item, const bool isLastItem)
	{
		SerializeHelper_Indent(depth + 1);

		_writer.Write("{ ");

		_indents = false;
		SerializeInternal(depth, declarationName, item);
		_indents = true;

		_writer.Write((isLastItem ? " }\n" : " },\n"));
	}

	inline void JSONSerializer::SerializeHelper_ArrayPostfix(const uint32 depth)
	{
		SerializeHelper_Indent(depth);

		_writer.Write("]");
	}
}
