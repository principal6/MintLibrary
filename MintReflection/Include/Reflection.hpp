#pragma once


#ifndef _MINT_REFLECTION_REFLECTION_H_PP
#define _MINT_REFLECTION_REFLECTION_H_PP


#include <MintReflection/Include/Reflection.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/StringReference.hpp>

#include <MintPlatform/Include/BinaryFile.hpp>
#include <MintReflection/Include/JSONSerializer.hpp>


namespace mint
{
#pragma region TypeBaseData && TypeData
	inline TypeBaseData::TypeBaseData()
		: _size{ 0 }
		, _alignment{ 0 }
		, _offset{ 0 }
		, _arrayItemCount{ 0 }
	{
		__noop;
	}


	template <typename T>
	inline void TypeData<T>::Serialize(BinarySerializer& serializer) const noexcept
	{
		serializer.SerializeInternal(_typeName);
		serializer.SerializeInternal(_declarationName);
		serializer.SerializeInternal(_size);
		serializer.SerializeInternal(_alignment);
		serializer.SerializeInternal(_offset);
		serializer.SerializeInternal(_arrayItemCount);
	}

	template <typename T>
	inline void TypeData<T>::SerializeValue(BinarySerializer& serializer, const void* const memberPointer, const uint32 arrayItemCount) const noexcept
	{
		for (uint32 arrayItemIndex = 0; arrayItemIndex < Max(static_cast<uint32>(1), arrayItemCount); ++arrayItemIndex)
		{
			const T* const castedMemberPointer = reinterpret_cast<const T*>(memberPointer);
			serializer.SerializeInternal(*(castedMemberPointer + arrayItemIndex));
		}
	}
	
	template <typename T>
	inline void TypeData<T>::SerializeValue(JSONSerializer& serializer, const uint32 depth, const void* const memberPointer, const uint32 arrayItemCount) const noexcept
	{
		if (arrayItemCount == 0)
		{
			const T* const castedMemberPointer = reinterpret_cast<const T*>(memberPointer);
			serializer.SerializeInternal(depth, _declarationName, *castedMemberPointer);
		}
		else
		{
			serializer.SerializeHelper_ArrayPrefix(depth, _declarationName);
			
			for (uint32 arrayItemIndex = 0; arrayItemIndex < arrayItemCount; ++arrayItemIndex)
			{
				const T* const castedMemberPointer = reinterpret_cast<const T*>(memberPointer);
				serializer.SerializeHelper_ArrayItem(depth, _declarationName, *(castedMemberPointer + arrayItemIndex), arrayItemIndex == arrayItemCount - 1);
			}

			serializer.SerializeHelper_ArrayPostfix(depth);
		}
	}

	template <typename T>
	inline bool TypeData<T>::Deserialize(BinarySerializer& serializer) noexcept
	{
		StringA deserializedTypeName;
		serializer.DeserializeInternal(deserializedTypeName);
		if (_typeName != deserializedTypeName)
		{
			MINT_LOG_ERROR("type name of the class [%s] does not match that of the source binary [%s]", _typeName.CString(), deserializedTypeName.CString());
			return false;
		}

		serializer.DeserializeInternal(_declarationName);
		serializer.DeserializeInternal(_size);
		serializer.DeserializeInternal(_alignment);
		serializer.DeserializeInternal(_offset);
		serializer.DeserializeInternal(_arrayItemCount);
		return true;
	}

	template <typename T>
	inline void TypeData<T>::DeserializeValue(BinarySerializer& serializer, void* const memberPointer, const uint32 arrayItemCount) noexcept
	{
		for (uint32 arrayItemIndex = 0; arrayItemIndex < Max(static_cast<uint32>(1), arrayItemCount); ++arrayItemIndex)
		{
			T* const castedMemberPointer = reinterpret_cast<T*>(memberPointer);
			serializer.DeserializeInternal(*(castedMemberPointer + arrayItemIndex));
		}
	}
#pragma endregion


#pragma region SerializerScopedDepth
	inline SerializerScopedDepth::SerializerScopedDepth(BinarySerializer* const serializer)
		: _serializer{ serializer }
	{
		++_serializer->_depth;
	}

	inline SerializerScopedDepth::~SerializerScopedDepth()
	{
		--_serializer->_depth;
	}
#pragma endregion


#pragma region BinarySerializer
	namespace SerializerUtil
	{
		MINT_INLINE const char* getDepthPrefix(const uint32 depth) noexcept
		{
			static StringA result;
			result.Clear();
			for (uint32 iter = 0; iter < depth; ++iter)
			{
				result += "-";
			}
			result += " ";
			return result.CString();
		}
	}


	//#define _DEBUG_LOG_SERIALIZER_INTERNALS
#if defined (_DEBUG_LOG_SERIALIZER_INTERNALS)
#define _MINT_LOG_SERIALIZATION_NOT_SPECIALIZED    SerializerScopedDepth serializerScopedDepth(this); if (isTypeData == false) MINT_LOG("%sserialization of the type[%s] is NOT specialized!", SerializerUtil::getDepthPrefix(_depth), typeid(from).name());
#define _MINT_LOG_SERIALIZATION_SPECIALIZED        SerializerScopedDepth serializerScopedDepth(this); if (isTypeData == false) MINT_LOG("%sserialization of the type[%s] IS SPECIALIZED.", SerializerUtil::getDepthPrefix(_depth), typeid(from).name());

#define _MINT_LOG_DESERIALIZATION_NOT_SPECIALIZED  SerializerScopedDepth serializerScopedDepth(this); if (isTypeData == false) MINT_LOG("%sdeserialization of the type[%s] is NOT specialized!", SerializerUtil::getDepthPrefix(_depth), typeid(to).name());
#define _MINT_LOG_DESERIALIZATION_SPECIALIZED      SerializerScopedDepth serializerScopedDepth(this); if (isTypeData == false) MINT_LOG("%sdeserialization of the type[%s] IS SPECIALIZED.", SerializerUtil::getDepthPrefix(_depth), typeid(to).name());
#else
#define _MINT_LOG_SERIALIZATION_NOT_SPECIALIZED      
#define _MINT_LOG_SERIALIZATION_SPECIALIZED          

#define _MINT_LOG_DESERIALIZATION_NOT_SPECIALIZED    
#define _MINT_LOG_DESERIALIZATION_SPECIALIZED        
#endif


	template <typename T>
	inline bool BinarySerializer::Serialize(const T& from, const char* const fileName) noexcept
	{
		_writer.Clear();

#if defined (_DEBUG_LOG_SERIALIZER_INTERNALS)
		if constexpr (IsReflectionClass<T>::value == true)
		{
			MINT_LOG("serializing a reflection class to [%s]", fileName);
		}
		else
		{
			MINT_LOG("serializing a non-reflection type to [%s]", fileName);
		}
#endif

		SerializeInternal(from);

		return _writer.Save(fileName);
	}

	template <typename T>
	inline void BinarySerializer::SerializeInternal(const T& from) noexcept
	{
		_MINT_LOG_SERIALIZATION_NOT_SPECIALIZED;

		if constexpr (IsReflectionClass<T>::value == true)
		{
			const ReflectionData& reflectionData = from.getReflectionData();
			TypeBaseData* const typeData = reflectionData._typeData;
			typeData->Serialize(*this);

			const uint32 memberCount = reflectionData._memberTypeDatas.Size();
			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				const TypeBaseData& memberTypeData = *reflectionData._memberTypeDatas[memberIndex];
				memberTypeData.Serialize(*this);
			}

			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				const TypeBaseData& memberTypeData = *reflectionData._memberTypeDatas[memberIndex];
				memberTypeData.SerializeValue(*this, reinterpret_cast<const char*>(&from) + memberTypeData._offset, memberTypeData._arrayItemCount);
			}
		}
		else
		{
			_writer.Write(from);
		}
	}

	template <typename T>
	inline void BinarySerializer::SerializeInternal(const String<T>& from) noexcept
	{
		_MINT_LOG_SERIALIZATION_SPECIALIZED;

		SerializeInternal(from.Length());

		SerializeInternal(from.CString());
	}

	template <typename T>
	inline void BinarySerializer::SerializeInternal(const Vector<T>& from) noexcept
	{
		_MINT_LOG_SERIALIZATION_SPECIALIZED;

		const uint32 count = from.Size();
		SerializeInternal(count);

		for (uint32 index = 0; index < count; ++index)
		{
			SerializeInternal(from[index]);
		}
	}

	template <typename T>
	inline bool BinarySerializer::Deserialize(const char* const fileName, T& to) noexcept
	{
		if (_reader.Open(fileName) == false)
		{
			return false;
		}

#if defined (_DEBUG_LOG_SERIALIZER_INTERNALS)
		if constexpr (IsReflectionClass<T>::value == true)
		{
			MINT_LOG("deserializing a reflection class from [%s]", fileName);
		}
		else
		{
			MINT_LOG("deserializing a non-reflection type from [%s]", fileName);
		}
#endif

		return DeserializeInternal(to);
	}

	template <typename T>
	inline bool BinarySerializer::DeserializeInternal(T& to) noexcept
	{
		_MINT_LOG_DESERIALIZATION_NOT_SPECIALIZED;

		if constexpr (IsReflectionClass<T>::value == true)
		{
			const ReflectionData& reflectionData = to.getReflectionData();
			TypeBaseData* const typeData = reflectionData._typeData;
			if (typeData->Deserialize(*this) == false)
			{
				return false;
			}

			const uint32 memberCount = reflectionData._memberTypeDatas.Size();
			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				TypeBaseData* const memberTypeData = reflectionData._memberTypeDatas[memberIndex];
				memberTypeData->Deserialize(*this);
			}

			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				TypeBaseData* const memberTypeData = reflectionData._memberTypeDatas[memberIndex];
				memberTypeData->DeserializeValue(*this, reinterpret_cast<char*>(&to) + memberTypeData->_offset, memberTypeData->_arrayItemCount);
			}
		}
		else
		{
			to = *_reader.Read<T>();
		}

		return true;
	}

	template <typename T>
	inline bool BinarySerializer::DeserializeInternal(String<T>& to) noexcept
	{
		_MINT_LOG_DESERIALIZATION_SPECIALIZED;

		const uint32 length = *_reader.Read<uint32>();
		to.Reserve(length);
		to = _reader.Read<const T>(length + 1);
		return true;
	}

	template <typename T>
	inline bool BinarySerializer::DeserializeInternal(Vector<T>& to) noexcept
	{
		_MINT_LOG_DESERIALIZATION_SPECIALIZED;

		const uint32 count = *_reader.Read<decltype(count)>();
		to.Resize(count);

		for (uint32 index = 0; index < count; ++index)
		{
			DeserializeInternal(to[index]);
		}
		return true;
	}
#pragma endregion
}


#endif // !_MINT_REFLECTION_REFLECTION_H_PP
