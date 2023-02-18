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

			MINT_INLINE void TypeCustomData::pushSlottedStreamData(const TypeMetaData<TypeCustomData>& slottedStreamData)
			{
				_slottedStreamDatas.PushBack(slottedStreamData);
			}

			MINT_INLINE bool TypeCustomData::isRegisterIndexValid() const noexcept
			{
				return (_registerIndex != kInvalidRegisterIndex);
			}

			MINT_INLINE const std::string& TypeCustomData::getSemanticName() const noexcept
			{
				return _semanticName;
			}

			MINT_INLINE uint32 TypeCustomData::getRegisterIndex() const noexcept
			{
				return _registerIndex;
			}

			MINT_INLINE uint32 TypeCustomData::getInputSlot() const noexcept
			{
				return _inputSlot;
			}

			MINT_INLINE uint32 TypeCustomData::getInstanceDataStepRate() const noexcept
			{
				return _instanceDataStepRate;
			}

			MINT_INLINE uint32 TypeCustomData::getSlottedStreamDataCount() const noexcept
			{
				return _slottedStreamDatas.Size();
			}

			MINT_INLINE const TypeMetaData<TypeCustomData>& TypeCustomData::getSlottedStreamData(const uint32 inputSlot) const noexcept
			{
				return (inputSlot < getSlottedStreamDataCount()) ? _slottedStreamDatas[inputSlot] : TypeMetaData<TypeCustomData>::getInvalid();
			}
#pragma endregion
		}
	}
}
