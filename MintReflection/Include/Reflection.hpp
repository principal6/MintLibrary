#pragma once


#ifndef MINT_REFLECTION_REFLECTION_HPP
#define MINT_REFLECTION_REFLECTION_HPP


#include <MintReflection/Include/Reflection.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/String.hpp>

#include <MintPlatform/Include/BinaryFile.hpp>


namespace mint
{
#pragma region TypeBaseData && TypeData
    inline TypeBaseData::TypeBaseData()
        : _size{ 0 }
        , _alignment{ 0 }
        , _offset{ 0 }
    {
        __noop;
    }

    inline TypeBaseData::~TypeBaseData()
    {
        __noop;
    }


    template <typename T>
    inline TypeData<T>::TypeData()
    {
        __noop;
    }

    template <typename T>
    inline TypeData<T>::~TypeData()
    {
        __noop;
    }

    template <typename T>
    inline void TypeData<T>::serialize(Serializer& serializer) noexcept
    {
        serializer._serializeInternal(_typeName, true);
        serializer._serializeInternal(_declarationName, true);
        serializer._serializeInternal(_size, true);
        serializer._serializeInternal(_alignment, true);
        serializer._serializeInternal(_offset, true);
    }

    template <typename T>
    inline void TypeData<T>::serializeValue(Serializer& serializer, const void* const memberPointer) noexcept
    {
        const T* const castedMemberPointer = reinterpret_cast<const T*>(memberPointer);
        serializer._serializeInternal(*castedMemberPointer, false);
    }

    template <typename T>
    inline const bool TypeData<T>::deserialize(Serializer& serializer) noexcept
    {
        StringA deserializedTypeName;
        serializer._deserializeInternal(deserializedTypeName, true);
        if (_typeName != deserializedTypeName)
        {
            MINT_LOG_ERROR("±èÀå¿ø", "type name of the class [%s] does not match that of the source binary [%s]", _typeName.c_str(), deserializedTypeName.c_str());
            return false;
        }

        serializer._deserializeInternal(_declarationName, true);
        serializer._deserializeInternal(_size, true);
        serializer._deserializeInternal(_alignment, true);
        serializer._deserializeInternal(_offset, true);

        return true;
    }

    template <typename T>
    inline void TypeData<T>::deserializeValue(Serializer& serializer, void* const memberPointer) noexcept
    {
        T* const castedMemberPointer = reinterpret_cast<T*>(memberPointer);
        serializer._deserializeInternal(*castedMemberPointer, false);
    }
#pragma endregion


#pragma region SerializerScopedDepth
    inline SerializerScopedDepth::SerializerScopedDepth(Serializer* const serializer)
        : _serializer{ serializer }
    {
        ++_serializer->_depth;
    }

    inline SerializerScopedDepth::~SerializerScopedDepth()
    {
        --_serializer->_depth;
    }
#pragma endregion


#pragma region Serializer
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
    #define _MINT_LOG_SERIALIZATION_NOT_SPECIALIZED    SerializerScopedDepth serializerScopedDepth(this); if (isTypeData == false) MINT_LOG("±èÀå¿ø", "%sserialization of the type[%s] is NOT specialized!", SerializerUtil::getDepthPrefix(_depth), typeid(from).name());
    #define _MINT_LOG_SERIALIZATION_SPECIALIZED        SerializerScopedDepth serializerScopedDepth(this); if (isTypeData == false) MINT_LOG("±èÀå¿ø", "%sserialization of the type[%s] IS SPECIALIZED.", SerializerUtil::getDepthPrefix(_depth), typeid(from).name());

    #define _MINT_LOG_DESERIALIZATION_NOT_SPECIALIZED  SerializerScopedDepth serializerScopedDepth(this); if (isTypeData == false) MINT_LOG("±èÀå¿ø", "%sdeserialization of the type[%s] is NOT specialized!", SerializerUtil::getDepthPrefix(_depth), typeid(to).name());
    #define _MINT_LOG_DESERIALIZATION_SPECIALIZED      SerializerScopedDepth serializerScopedDepth(this); if (isTypeData == false) MINT_LOG("±èÀå¿ø", "%sdeserialization of the type[%s] IS SPECIALIZED.", SerializerUtil::getDepthPrefix(_depth), typeid(to).name());
#else
    #define _MINT_LOG_SERIALIZATION_NOT_SPECIALIZED      
    #define _MINT_LOG_SERIALIZATION_SPECIALIZED          

    #define _MINT_LOG_DESERIALIZATION_NOT_SPECIALIZED    
    #define _MINT_LOG_DESERIALIZATION_SPECIALIZED        
#endif


    template <typename T>
    inline const bool Serializer::serialize(const T& from, const char* const fileName) noexcept
    {
        _writer.clear();

    #if defined (_DEBUG_LOG_SERIALIZER_INTERNALS)
        if constexpr (IsReflectionClass<T>::value == true)
        {
            MINT_LOG("±èÀå¿ø", "serializing a reflection class to [%s]", fileName);
        }
        else
        {
            MINT_LOG("±èÀå¿ø", "serializing a non-reflection type to [%s]", fileName);
        }
    #endif

        _serializeInternal(from, false);

        return _writer.save(fileName);
    }

    template <typename T>
    inline void Serializer::_serializeInternal(const T& from, const bool isTypeData) noexcept
    {
        _MINT_LOG_SERIALIZATION_NOT_SPECIALIZED;
        
        if constexpr (IsReflectionClass<T>::value == true)
        {
            TypeBaseData* const typeData = from.getReflectionData()._typeData;
            typeData->serialize(*this);

            const uint32 memberCount = from.getReflectionData()._memberTypeDatas.size();
            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                TypeBaseData* const memberTypeData = from.getReflectionData()._memberTypeDatas[memberIndex];
                memberTypeData->serialize(*this);
            }
 
            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                TypeBaseData* const memberTypeData = from.getReflectionData()._memberTypeDatas[memberIndex];
                memberTypeData->serializeValue(*this, reinterpret_cast<const char*>(&from) + memberTypeData->_offset);
            }
        }
        else
        {
            _writer.write(from);
        }
    }

    template <typename T>
    inline void Serializer::_serializeInternal(const String<T>& from, const bool isTypeData) noexcept
    {
        _MINT_LOG_SERIALIZATION_SPECIALIZED;

        _writer.write(from.length());
        
        _writer.write(from.c_str());
    }
    
    template <typename T>
    inline void Serializer::_serializeInternal(const Vector<T>& from, const bool isTypeData) noexcept
    {
        _MINT_LOG_SERIALIZATION_SPECIALIZED;

        const uint32 count = from.size();
        _writer.write(count);
        
        for (uint32 index = 0; index < count; ++index)
        {
            _serializeInternal(from[index], false);
        }
    }

    template <typename T>
    inline const bool Serializer::deserialize(const char* const fileName, T& to) noexcept
    {
        if (_reader.open(fileName) == false)
        {
            return false;
        }

    #if defined (_DEBUG_LOG_SERIALIZER_INTERNALS)
        if constexpr (IsReflectionClass<T>::value == true)
        {
            MINT_LOG("±èÀå¿ø", "deserializing a reflection class from [%s]", fileName);
        }
        else
        {
            MINT_LOG("±èÀå¿ø", "deserializing a non-reflection type from [%s]", fileName);
        }
    #endif

        return _deserializeInternal(to, false);
    }

    template <typename T>
    inline const bool Serializer::_deserializeInternal(T& to, const bool isTypeData) noexcept
    {
        _MINT_LOG_DESERIALIZATION_NOT_SPECIALIZED;

        if constexpr (IsReflectionClass<T>::value == true)
        {
            TypeBaseData* const typeData = to.getReflectionData()._typeData;
            if (typeData->deserialize(*this) == false)
            {
                return false;
            }

            const uint32 memberCount = to.getReflectionData()._memberTypeDatas.size();
            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                TypeBaseData* const memberTypeData = to.getReflectionData()._memberTypeDatas[memberIndex];
                memberTypeData->deserialize(*this);
            }

            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                TypeBaseData* const memberTypeData = to.getReflectionData()._memberTypeDatas[memberIndex];
                memberTypeData->deserializeValue(*this, reinterpret_cast<char*>(&to) + memberTypeData->_offset);
            }
        }
        else
        {
            to = *_reader.read<T>();
        }

        return true;
    }

    template <typename T>
    inline const bool Serializer::_deserializeInternal(String<T>& to, const bool isTypeData) noexcept
    {
        _MINT_LOG_DESERIALIZATION_SPECIALIZED;
        
        const uint32 length = *_reader.read<uint32>();
        to.reserve(length);
        to = _reader.read<const T>(length + 1);

        return true;
    }

    template <typename T>
    inline const bool Serializer::_deserializeInternal(Vector<T>& to, const bool isTypeData) noexcept
    {
        _MINT_LOG_DESERIALIZATION_SPECIALIZED;
        
        const uint32 count = *_reader.read<decltype(count)>();
        to.resize(count);

        for (uint32 index = 0; index < count; ++index)
        {
            _deserializeInternal(to[index], false);
        }

        return true;
    }
#pragma endregion
}


#endif // !MINT_REFLECTION_REFLECTION_HPP
