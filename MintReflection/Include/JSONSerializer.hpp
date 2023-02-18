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
	inline bool JSONSerializer::serialize(const T& from, const StringReferenceA& fileName)
	{
		if constexpr (IsReflectionClass<T>::value == false)
		{
			MINT_LOG_ERROR("'from' must be a reflection class!");
			return false;
		}

		_writer.clear();

		_indents = true;

		serialize_internal(0, StringReferenceA(""), from);

		return _writer.save(fileName.CString());
	}

	template<typename CharType, typename T>
	inline void JSONSerializer::serialize_internal(const uint32 depth, const StringReference<CharType>& declarationName, const T& from)
	{
		if constexpr (IsReflectionClass<T>::value == true)
		{
			serialize_helper_indent(depth);
			if (depth > 0)
			{
				serialize_helper_declaration(declarationName);
			}

			_writer.write("{\n");

			const ReflectionData& reflectionData = from.getReflectionData();
			const uint32 memberCount = reflectionData._memberTypeDatas.Size();
			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				const TypeBaseData& memberTypeData = *reflectionData._memberTypeDatas[memberIndex];
				memberTypeData.serializeValue(*this, depth + 1, reinterpret_cast<const char*>(&from) + memberTypeData._offset, memberTypeData._arrayItemCount);

				if (memberIndex < memberCount - 1)
				{
					_writer.write(",\n");
				}
				else
				{
					_writer.write("\n");
				}
			}

			serialize_helper_indent(depth);

			_writer.write("}");
		}
		else if constexpr (std::is_integral_v<T> == true || std::is_floating_point_v<T> == true)
		{
			serialize_helper_indent(depth);
			serialize_helper_declaration(declarationName);

			StackString<CharType, 256> buffer;
			StringUtil::ToString(from, buffer);
			_writer.write(buffer.CString());
		}
		else
		{
			MINT_NEVER;
		}
	}

	template<typename CharType>
	inline void JSONSerializer::serialize_internal(const uint32 depth, const StringReference<CharType>& declarationName, const String<CharType>& from)
	{
		serialize_helper_indent(depth);
		serialize_helper_declaration(declarationName);

		_writer.write("\"");
		_writer.write(from.CString());
		_writer.write("\"");
	}

	template<typename CharType>
	inline void JSONSerializer::serialize_internal(const uint32 depth, const StringReference<CharType>& declarationName, const StringReference<CharType>& from)
	{
		serialize_helper_indent(depth);
		serialize_helper_declaration(declarationName);

		_writer.write("\"");
		_writer.write(from.CString());
		_writer.write("\"");
	}

	template<typename CharType, typename T>
	inline void JSONSerializer::serialize_internal(const uint32 depth, const StringReference<CharType>& declarationName, const Vector<T>& from)
	{
		serialize_helper_arrayPrefix(depth, declarationName);

		const uint32 count = from.Size();
		for (uint32 index = 0; index < count; ++index)
		{
			serialize_helper_arrayItem(depth, declarationName, from[index], index == count - 1);
		}

		serialize_helper_arrayPostfix(depth);
	}

	inline void JSONSerializer::serialize_helper_indent(const uint32 depth)
	{
		if (depth == 0)
		{
			return;
		}

		if (_indents)
		{
			for (uint32 depthIter = 0; depthIter < depth; depthIter++)
			{
				_writer.write("\t");
			}
		}
	}

	template<typename CharType>
	inline void JSONSerializer::serialize_helper_declaration(const StringReference<CharType>& declarationName)
	{
		_writer.write("\"");
		_writer.write(declarationName.CString());
		_writer.write("\": ");
	}

	template<typename CharType>
	inline void JSONSerializer::serialize_helper_arrayPrefix(const uint32 depth, const StringReference<CharType>& declarationName)
	{
		serialize_helper_indent(depth);
		serialize_helper_declaration(declarationName);

		_writer.write("[\n");
	}

	template<typename CharType, typename T>
	inline void JSONSerializer::serialize_helper_arrayItem(const uint32 depth, const StringReference<CharType>& declarationName, const T& item, const bool isLastItem)
	{
		serialize_helper_indent(depth + 1);

		_writer.write("{ ");

		_indents = false;
		serialize_internal(depth, declarationName, item);
		_indents = true;

		_writer.write((isLastItem ? " }\n" : " },\n"));
	}

	inline void JSONSerializer::serialize_helper_arrayPostfix(const uint32 depth)
	{
		serialize_helper_indent(depth);

		_writer.write("]");
	}
}
