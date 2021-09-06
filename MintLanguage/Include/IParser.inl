#pragma once


namespace mint
{
    namespace Language
    {
#pragma region TypeMetaData<CustomDataType>
        template <typename CustomDataType>
        inline TypeMetaData<CustomDataType>::TypeMetaData()
            : _isBuiltIn{ false }
            , _size{ 0 }
            , _byteOffset{ 0 }
        {
            __noop;
        }

        template <typename CustomDataType>
        MINT_INLINE const TypeMetaData<CustomDataType>& TypeMetaData<CustomDataType>::getInvalid() noexcept
        {
            static const TypeMetaData<CustomDataType> kInvalid;
            return kInvalid;
        }

        template <typename CustomDataType>
        MINT_INLINE void TypeMetaData<CustomDataType>::setBaseData(const std::string& typeName, const bool isBuiltIn)
        {
            _typeName = typeName;
            _isBuiltIn = isBuiltIn;
        }

        template <typename CustomDataType>
        MINT_INLINE void TypeMetaData<CustomDataType>::setDeclName(const std::string& declName)
        {
            _declName = declName;
        }

        template <typename CustomDataType>
        MINT_INLINE void TypeMetaData<CustomDataType>::setSize(const uint32 size)
        {
            _size = size;
        }

        template <typename CustomDataType>
        MINT_INLINE void TypeMetaData<CustomDataType>::setByteOffset(const uint32 byteOffset)
        {
            _byteOffset = byteOffset;
        }

        template <typename CustomDataType>
        MINT_INLINE void TypeMetaData<CustomDataType>::pushMember(const TypeMetaData<CustomDataType>& member)
        {
            _memberArray.push_back(member);
        }

        template <typename CustomDataType>
        MINT_INLINE const bool TypeMetaData<CustomDataType>::isBuiltIn() const noexcept
        {
            return _isBuiltIn;
        }

        template <typename CustomDataType>
        MINT_INLINE const std::string& TypeMetaData<CustomDataType>::getTypeName() const noexcept
        {
            return _typeName;
        }

        template <typename CustomDataType>
        MINT_INLINE const std::string& TypeMetaData<CustomDataType>::getDeclName() const noexcept
        {
            return _declName;
        }

        template <typename CustomDataType>
        MINT_INLINE const uint32 TypeMetaData<CustomDataType>::getSize() const noexcept
        {
            return _size;
        }

        template <typename CustomDataType>
        MINT_INLINE const uint32 TypeMetaData<CustomDataType>::getByteOffset() const noexcept
        {
            return _byteOffset;
        }

        template <typename CustomDataType>
        MINT_INLINE const uint32 TypeMetaData<CustomDataType>::getMemberCount() const noexcept
        {
            return _memberArray.size();
        }

        template <typename CustomDataType>
        MINT_INLINE const TypeMetaData<CustomDataType>& TypeMetaData<CustomDataType>::getMember(const uint32 memberIndex) const noexcept
        {
            return (memberIndex < getMemberCount()) ? _memberArray[memberIndex] : TypeMetaData<CustomDataType>::getInvalid();
        }
#pragma endregion


        MINT_INLINE constexpr const char* IParser::convertErrorTypeToTypeString(const ErrorType errorType)
        {
            return kErrorTypeStringArray[static_cast<uint32>(errorType)][0];
        }

        MINT_INLINE constexpr const char* IParser::convertErrorTypeToContentString(const ErrorType errorType)
        {
            return kErrorTypeStringArray[static_cast<uint32>(errorType)][1];
        }
    }
}
