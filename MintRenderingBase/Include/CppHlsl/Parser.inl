#pragma once


namespace mint
{
    namespace Language
    {
        namespace CppHlsl
        {
#pragma region TypeCustomData
            inline TypeCustomData::TypeCustomData()
                : _registerIndex{ kInvalidRegisterIndex }
                , _inputSlot{ 0 }
                , _instanceDataStepRate{ 0 }
            {
                __noop;
            }

            MINT_INLINE void TypeCustomData::setSemanticName(const std::string& semanticName)
            {
                _semanticName = semanticName;
            }

            MINT_INLINE void TypeCustomData::setRegisterIndex(const uint32 registerIndex)
            {
                _registerIndex = registerIndex;
            }

            MINT_INLINE void TypeCustomData::setInputSlot(const uint32 inputSlot)
            {
                _inputSlot = inputSlot;
            }

            MINT_INLINE void TypeCustomData::setInstanceDataStepRate(const uint32 instanceDataStepRate)
            {
                _instanceDataStepRate = instanceDataStepRate;
            }

            MINT_INLINE void TypeCustomData::pushSlottedStreamData(const TypeMetaData& slottedStreamData)
            {
                _slottedStreamDatas.push_back(slottedStreamData);
            }

            MINT_INLINE const bool TypeCustomData::isRegisterIndexValid() const noexcept
            {
                return (_registerIndex != kInvalidRegisterIndex);
            }

            MINT_INLINE const std::string& TypeCustomData::getSemanticName() const noexcept
            {
                return _semanticName;
            }

            MINT_INLINE const uint32 TypeCustomData::getRegisterIndex() const noexcept
            {
                return _registerIndex;
            }

            MINT_INLINE const uint32 TypeCustomData::getInputSlot() const noexcept
            {
                return _inputSlot;
            }

            MINT_INLINE const uint32 TypeCustomData::getInstanceDataStepRate() const noexcept
            {
                return _instanceDataStepRate;
            }

            MINT_INLINE const uint32 TypeCustomData::getSlottedStreamDataCount() const noexcept
            {
                return _slottedStreamDatas.size();
            }

            MINT_INLINE const TypeMetaData& TypeCustomData::getSlottedStreamData(const uint32 inputSlot) const noexcept
            {
                return (inputSlot < getSlottedStreamDataCount()) ? _slottedStreamDatas[inputSlot] : TypeMetaData::getInvalid();
            }
#pragma endregion


#pragma region TypeMetaData
            inline TypeMetaData::TypeMetaData()
                : _isBuiltIn{ false }
                , _size{ 0 }
                , _byteOffset{ 0 }
            {
                __noop;
            }

            MINT_INLINE const TypeMetaData& TypeMetaData::getInvalid() noexcept
            {
                static const TypeMetaData kInvalid;
                return kInvalid;
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

            MINT_INLINE void TypeMetaData::pushMember(const TypeMetaData& member)
            {
                _memberArray.push_back(member);
            }

            MINT_INLINE const bool TypeMetaData::isBuiltIn() const noexcept
            {
                return _isBuiltIn;
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

            MINT_INLINE const uint32 TypeMetaData::getByteOffset() const noexcept
            {
                return _byteOffset;
            }

            MINT_INLINE const uint32 TypeMetaData::getMemberCount() const noexcept
            {
                return _memberArray.size();
            }

            MINT_INLINE const TypeMetaData& TypeMetaData::getMember(const uint32 memberIndex) const noexcept
            {
                return (memberIndex < getMemberCount()) ? _memberArray[memberIndex] : TypeMetaData::getInvalid();
            }
#pragma endregion
        }
    }
}
