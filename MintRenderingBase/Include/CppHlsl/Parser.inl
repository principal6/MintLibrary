#include "Parser.h"
#pragma once


namespace mint
{
    namespace CppHlsl
    {
        inline TypeMetaData::TypeMetaData()
            : _isBuiltIn{ false }
            , _registerIndex{ kInvalidRegisterIndex }
            , _size{ 0 }
            , _byteOffset{ 0 }
        {
            __noop;
        }

        MINT_INLINE void TypeMetaData::setBaseData(const std::string& typeName, const bool isBuiltIn)
        {
            _typeName = typeName;
            _isBuiltIn = isBuiltIn;
        }

        MINT_INLINE void TypeMetaData::setDeclName(const std::string& declName)
        {
            _declName = declName;
        }

        MINT_INLINE void TypeMetaData::setSize(const uint32 size)
        {
            _size = size;
        }

        MINT_INLINE void TypeMetaData::setByteOffset(const uint32 byteOffset)
        {
            _byteOffset = byteOffset;
        }

        MINT_INLINE void TypeMetaData::setSemanticName(const std::string& semanticName)
        {
            _semanticName = semanticName;
        }

        MINT_INLINE void TypeMetaData::setRegisterIndex(const uint32 registerIndex)
        {
            _registerIndex = registerIndex;
        }

        MINT_INLINE void TypeMetaData::pushMember(const TypeMetaData& member)
        {
            _memberArray.push_back(member);
        }

        MINT_INLINE const bool TypeMetaData::isBuiltIn() const noexcept
        {
            return _isBuiltIn;
        }

        MINT_INLINE const bool TypeMetaData::isRegisterIndexValid() const noexcept
        {
            return (_registerIndex != kInvalidRegisterIndex);
        }

        MINT_INLINE const std::string& TypeMetaData::getTypeName() const noexcept
        {
            return _typeName;
        }

        MINT_INLINE const std::string& TypeMetaData::getDeclName() const noexcept
        {
            return _declName;
        }

        MINT_INLINE const uint32 TypeMetaData::getSize() const noexcept
        {
            return _size;
        }

        inline const uint32 TypeMetaData::getByteOffset() const noexcept
        {
            return _byteOffset;
        }

        MINT_INLINE const std::string& TypeMetaData::getSemanticName() const noexcept
        {
            return _semanticName;
        }

        MINT_INLINE const uint32 TypeMetaData::getRegisterIndex() const noexcept
        {
            return _registerIndex;
        }

        MINT_INLINE const uint32 TypeMetaData::getMemberCount() const noexcept
        {
            return static_cast<uint32>(_memberArray.size());
        }

        MINT_INLINE const TypeMetaData& TypeMetaData::getMember(const uint32 memberIndex) const noexcept
        {
            return (memberIndex < getMemberCount()) ? _memberArray[memberIndex] : kInvalidTypeMetaData;
        }
    }
}
