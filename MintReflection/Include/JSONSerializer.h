#pragma once


#ifndef _MINT_REFLECTION_JSON_SERIALIZER_H_
#define _MINT_REFLECTION_JSON_SERIALIZER_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintPlatform/Include/TextFile.h>
#include <MintContainer/Include/StringReference.h>


namespace mint
{
	class JSONSerializer
	{
		template <typename T>
		friend class TypeData;

	public:
		JSONSerializer();
		~JSONSerializer();

	public:
		template<typename T>
		bool serialize(const T& from, const StringReferenceA& fileName);

	private:
		template<typename CharType, typename T>
		void serialize_internal(const uint32 depth, const StringReference<CharType>& declarationName, const T& from);
		
		template<typename CharType>
		void serialize_internal(const uint32 depth, const StringReference<CharType>& declarationName, const String<CharType>& from);

		template<typename CharType>
		void serialize_internal(const uint32 depth, const StringReference<CharType>& declarationName, const StringReference<CharType>& from);
		
		template<typename CharType, typename T>
		void serialize_internal(const uint32 depth, const StringReference<CharType>& declarationName, const Vector<T>& from);
		
	private:
		void serialize_helper_indent(const uint32 depth);

		template<typename CharType>
		void serialize_helper_declaration(const StringReference<CharType>& declarationName);

		template<typename CharType>
		void serialize_helper_arrayPrefix(const uint32 depth, const StringReference<CharType>& declarationName);

		template<typename CharType, typename T>
		void serialize_helper_arrayItem(const uint32 depth, const StringReference<CharType>& declarationName, const T& item, const bool isLastItem);

		void serialize_helper_arrayPostfix(const uint32 depth);

	private:
		TextFileReader _reader;
		TextFileWriter _writer;
		bool _indents = true;
	};
}


#endif // !_MINT_REFLECTION_JSON_SERIALIZER_H_
