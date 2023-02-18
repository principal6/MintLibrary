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

			MINT_INLINE void TypeCustomData::SetSemanticName(const std::string& semanticName)
			{
				_semanticName = semanticName;
			}

			MINT_INLINE void TypeCustomData::SetRegisterIndex(const uint32 registerIndex)
			{
				_registerIndex = registerIndex;
			}

			MINT_INLINE void TypeCustomData::SetInputSlot(const uint32 inputSlot)
			{
				_inputSlot = inputSlot;
			}

			MINT_INLINE void TypeCustomData::SetInstanceDataStepRate(const uint32 instanceDataStepRate)
			{
				_instanceDataStepRate = instanceDataStepRate;
			}

			MINT_INLINE void TypeCustomData::PushSlottedStreamData(const TypeMetaData<TypeCustomData>& slottedStreamData)
			{
				_slottedStreamDatas.PushBack(slottedStreamData);
			}

			MINT_INLINE bool TypeCustomData::IsRegisterIndexValid() const noexcept
			{
				return (_registerIndex != kInvalidRegisterIndex);
			}

			MINT_INLINE const std::string& TypeCustomData::GetSemanticName() const noexcept
			{
				return _semanticName;
			}

			MINT_INLINE uint32 TypeCustomData::GetRegisterIndex() const noexcept
			{
				return _registerIndex;
			}

			MINT_INLINE uint32 TypeCustomData::GetInputSlot() const noexcept
			{
				return _inputSlot;
			}

			MINT_INLINE uint32 TypeCustomData::GetInstanceDataStepRate() const noexcept
			{
				return _instanceDataStepRate;
			}

			MINT_INLINE uint32 TypeCustomData::GetSlottedStreamDataCount() const noexcept
			{
				return _slottedStreamDatas.Size();
			}

			MINT_INLINE const TypeMetaData<TypeCustomData>& TypeCustomData::GetSlottedStreamData(const uint32 inputSlot) const noexcept
			{
				return (inputSlot < GetSlottedStreamDataCount()) ? _slottedStreamDatas[inputSlot] : TypeMetaData<TypeCustomData>::GetInvalid();
			}
#pragma endregion
		}
	}
}
