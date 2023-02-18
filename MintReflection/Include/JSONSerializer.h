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
		bool Serialize(const T& from, const StringReferenceA& fileName);

	private:
		template<typename CharType, typename T>
		void SerializeInternal(const uint32 depth, const StringReference<CharType>& declarationName, const T& from);
		
		template<typename CharType>
		void SerializeInternal(const uint32 depth, const StringReference<CharType>& declarationName, const String<CharType>& from);

		template<typename CharType>
		void SerializeInternal(const uint32 depth, const StringReference<CharType>& declarationName, const StringReference<CharType>& from);
		
		template<typename CharType, typename T>
		void SerializeInternal(const uint32 depth, const StringReference<CharType>& declarationName, const Vector<T>& from);
		
	private:
		void SerializeHelper_Indent(const uint32 depth);

		template<typename CharType>
		void SerializeHelper_Declaration(const StringReference<CharType>& declarationName);

		template<typename CharType>
		void SerializeHelper_ArrayPrefix(const uint32 depth, const StringReference<CharType>& declarationName);

		template<typename CharType, typename T>
		void SerializeHelper_ArrayItem(const uint32 depth, const StringReference<CharType>& declarationName, const T& item, const bool isLastItem);

		void SerializeHelper_ArrayPostfix(const uint32 depth);

	private:
		TextFileReader _reader;
		TextFileWriter _writer;
		bool _indents = true;
	};
}


#endif // !_MINT_REFLECTION_JSON_SERIALIZER_H_
