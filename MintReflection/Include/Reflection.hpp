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
	inline void TypeData<T>::serialize(BinarySerializer& serializer) const noexcept
	{
		serializer.serialize_internal(_typeName);
		serializer.serialize_internal(_declarationName);
		serializer.serialize_internal(_size);
		serializer.serialize_internal(_alignment);
		serializer.serialize_internal(_offset);
		serializer.serialize_internal(_arrayItemCount);
	}

	template <typename T>
	inline void TypeData<T>::serializeValue(BinarySerializer& serializer, const void* const memberPointer, const uint32 arrayItemCount) const noexcept
	{
		for (uint32 arrayItemIndex = 0; arrayItemIndex < mint::max(static_cast<uint32>(1), arrayItemCount); ++arrayItemIndex)
		{
			const T* const castedMemberPointer = reinterpret_cast<const T*>(memberPointer);
			serializer.serialize_internal(*(castedMemberPointer + arrayItemIndex));
		}
	}
	
	template <typename T>
	inline void TypeData<T>::serializeValue(JSONSerializer& serializer, const uint32 depth, const void* const memberPointer, const uint32 arrayItemCount) const noexcept
	{
		if (arrayItemCount == 0)
		{
			const T* const castedMemberPointer = reinterpret_cast<const T*>(memberPointer);
			serializer.serialize_internal(depth, _declarationName, *castedMemberPointer);
		}
		else
		{
			serializer.serialize_helper_arrayPrefix(depth, _declarationName);
			
			for (uint32 arrayItemIndex = 0; arrayItemIndex < arrayItemCount; ++arrayItemIndex)
			{
				const T* const castedMemberPointer = reinterpret_cast<const T*>(memberPointer);
				serializer.serialize_helper_arrayItem(depth, _declarationName, *(castedMemberPointer + arrayItemIndex), arrayItemIndex == arrayItemCount - 1);
			}

			serializer.serialize_helper_arrayPostfix(depth);
		}
	}

	template <typename T>
	inline bool TypeData<T>::deserialize(BinarySerializer& serializer) noexcept
	{
		StringA deserializedTypeName;
		serializer.deserialize_internal(deserializedTypeName);
		if (_typeName != deserializedTypeName)
		{
			MINT_LOG_ERROR("type name of the class [%s] does not match that of the source binary [%s]", _typeName.c_str(), deserializedTypeName.c_str());
			return false;
		}

		serializer.deserialize_internal(_declarationName);
		serializer.deserialize_internal(_size);
		serializer.deserialize_internal(_alignment);
		serializer.deserialize_internal(_offset);
		serializer.deserialize_internal(_arrayItemCount);
		return true;
	}

	template <typename T>
	inline void TypeData<T>::deserializeValue(BinarySerializer& serializer, void* const memberPointer, const uint32 arrayItemCount) noexcept
	{
		for (uint32 arrayItemIndex = 0; arrayItemIndex < mint::max(static_cast<uint32>(1), arrayItemCount); ++arrayItemIndex)
		{
			T* const castedMemberPointer = reinterpret_cast<T*>(memberPointer);
			serializer.deserialize_internal(*(castedMemberPointer + arrayItemIndex));
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
			result.clear();
			for (uint32 iter = 0; iter < depth; ++iter)
			{
				result += "-";
			}
			result += " ";
			return result.c_str();
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
	inline bool BinarySerializer::serialize(const T& from, const char* const fileName) noexcept
	{
		_writer.clear();

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

		serialize_internal(from);

		return _writer.save(fileName);
	}

	template <typename T>
	inline void BinarySerializer::serialize_internal(const T& from) noexcept
	{
		_MINT_LOG_SERIALIZATION_NOT_SPECIALIZED;

		if constexpr (IsReflectionClass<T>::value == true)
		{
			const ReflectionData& reflectionData = from.getReflectionData();
			TypeBaseData* const typeData = reflectionData._typeData;
			typeData->serialize(*this);

			const uint32 memberCount = reflectionData._memberTypeDatas.size();
			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				const TypeBaseData& memberTypeData = *reflectionData._memberTypeDatas[memberIndex];
				memberTypeData.serialize(*this);
			}

			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				const TypeBaseData& memberTypeData = *reflectionData._memberTypeDatas[memberIndex];
				memberTypeData.serializeValue(*this, reinterpret_cast<const char*>(&from) + memberTypeData._offset, memberTypeData._arrayItemCount);
			}
		}
		else
		{
			_writer.write(from);
		}
	}

	template <typename T>
	inline void BinarySerializer::serialize_internal(const String<T>& from) noexcept
	{
		_MINT_LOG_SERIALIZATION_SPECIALIZED;

		serialize_internal(from.length());

		serialize_internal(from.c_str());
	}

	template <typename T>
	inline void BinarySerializer::serialize_internal(const Vector<T>& from) noexcept
	{
		_MINT_LOG_SERIALIZATION_SPECIALIZED;

		const uint32 count = from.size();
		serialize_internal(count);

		for (uint32 index = 0; index < count; ++index)
		{
			serialize_internal(from[index]);
		}
	}

	template <typename T>
	inline bool BinarySerializer::deserialize(const char* const fileName, T& to) noexcept
	{
		if (_reader.open(fileName) == false)
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

		return deserialize_internal(to);
	}

	template <typename T>
	inline bool BinarySerializer::deserialize_internal(T& to) noexcept
	{
		_MINT_LOG_DESERIALIZATION_NOT_SPECIALIZED;

		if constexpr (IsReflectionClass<T>::value == true)
		{
			const ReflectionData& reflectionData = to.getReflectionData();
			TypeBaseData* const typeData = reflectionData._typeData;
			if (typeData->deserialize(*this) == false)
			{
				return false;
			}

			const uint32 memberCount = reflectionData._memberTypeDatas.size();
			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				TypeBaseData* const memberTypeData = reflectionData._memberTypeDatas[memberIndex];
				memberTypeData->deserialize(*this);
			}

			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				TypeBaseData* const memberTypeData = reflectionData._memberTypeDatas[memberIndex];
				memberTypeData->deserializeValue(*this, reinterpret_cast<char*>(&to) + memberTypeData->_offset, memberTypeData->_arrayItemCount);
			}
		}
		else
		{
			to = *_reader.read<T>();
		}

		return true;
	}

	template <typename T>
	inline bool BinarySerializer::deserialize_internal(String<T>& to) noexcept
	{
		_MINT_LOG_DESERIALIZATION_SPECIALIZED;

		const uint32 length = *_reader.read<uint32>();
		to.reserve(length);
		to = _reader.read<const T>(length + 1);
		return true;
	}

	template <typename T>
	inline bool BinarySerializer::deserialize_internal(Vector<T>& to) noexcept
	{
		_MINT_LOG_DESERIALIZATION_SPECIALIZED;

		const uint32 count = *_reader.read<decltype(count)>();
		to.resize(count);

		for (uint32 index = 0; index < count; ++index)
		{
			deserialize_internal(to[index]);
		}
		return true;
	}
#pragma endregion
}


#endif // !_MINT_REFLECTION_REFLECTION_H_PP
