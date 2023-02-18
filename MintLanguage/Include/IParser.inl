#include "IParser.h"
#pragma once


namespace mint
{
	namespace Language
	{
#pragma region SyntaxTreeItem
		template <typename ClassifierType>
		inline SyntaxTreeItem<ClassifierType>::SyntaxTreeItem()
			: _classifier{}
		{
			__noop;
		}

		template <typename ClassifierType>
		inline SyntaxTreeItem<ClassifierType>::~SyntaxTreeItem()
		{
			__noop;
		}
#pragma endregion


#pragma region TypeMetaData
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
			_memberArray.PushBack(member);
		}

		template <typename CustomDataType>
		MINT_INLINE bool TypeMetaData<CustomDataType>::isBuiltIn() const noexcept
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
		MINT_INLINE uint32 TypeMetaData<CustomDataType>::getSize() const noexcept
		{
			return _size;
		}

		template <typename CustomDataType>
		MINT_INLINE uint32 TypeMetaData<CustomDataType>::getByteOffset() const noexcept
		{
			return _byteOffset;
		}

		template <typename CustomDataType>
		MINT_INLINE uint32 TypeMetaData<CustomDataType>::getMemberCount() const noexcept
		{
			return _memberArray.Size();
		}

		template <typename CustomDataType>
		MINT_INLINE const TypeMetaData<CustomDataType>& TypeMetaData<CustomDataType>::getMember(const uint32 memberIndex) const noexcept
		{
			return (memberIndex < getMemberCount()) ? _memberArray[memberIndex] : TypeMetaData<CustomDataType>::getInvalid();
		}
#pragma endregion


#pragma region IParser
		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		MINT_INLINE constexpr const char* IParser<TypeCustomDataType, SyntaxClassifierType>::convertErrorTypeToTypeString(const ErrorType errorType)
		{
			return kErrorTypeStringArray[static_cast<uint32>(errorType)][0];
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		MINT_INLINE constexpr const char* IParser<TypeCustomDataType, SyntaxClassifierType>::convertErrorTypeToContentString(const ErrorType errorType)
		{
			return kErrorTypeStringArray[static_cast<uint32>(errorType)][1];
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline IParser<TypeCustomDataType, SyntaxClassifierType>::ErrorMessage::ErrorMessage()
			: _sourceAt{ 0 }
		{
			__noop;
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline IParser<TypeCustomDataType, SyntaxClassifierType>::ErrorMessage::ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType)
			: _sourceAt{ symbolTableItem._sourceAt }
		{
			_message = "ERROR[";
			_message += convertErrorTypeToTypeString(errorType);
			_message += "] ";
			_message += convertErrorTypeToContentString(errorType);
			_message += " \'";
			_message += symbolTableItem._symbolString;
			_message += "\' #[";
			_message += std::to_string(_sourceAt);
			_message += "]";
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline IParser<TypeCustomDataType, SyntaxClassifierType>::ErrorMessage::ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation)
			: ErrorMessage(symbolTableItem, errorType)
		{
			_message += ": ";
			_message += additionalExplanation;
		}


		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline IParser<TypeCustomDataType, SyntaxClassifierType>::IParser(ILexer& lexer)
			: _lexer{ lexer }
			, _symbolTable{ lexer._symbolTable }
			, _symbolAt{ 0 }
		{
			__noop;
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline void IParser<TypeCustomDataType, SyntaxClassifierType>::registerTypeInternal(const std::string& typeFullName, const uint32 typeSize, const bool isBuiltIn) noexcept
		{
			TypeMetaData<TypeCustomDataType> typeMetaData;
			typeMetaData.setBaseData(typeFullName, isBuiltIn);
			typeMetaData.setSize(typeSize);
			_typeMetaDatas.PushBack(typeMetaData);
			_typeMetaDataMap.Insert(typeFullName, _typeMetaDatas.Size() - 1);
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline void IParser<TypeCustomDataType, SyntaxClassifierType>::reset()
		{
			_symbolAt = 0;
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline bool IParser<TypeCustomDataType, SyntaxClassifierType>::continueParsing() const noexcept
		{
			return _symbolAt < _symbolTable.Size();
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline void IParser<TypeCustomDataType, SyntaxClassifierType>::advanceSymbolPositionXXX(const uint32 advanceCount)
		{
			_symbolAt += Max(advanceCount, static_cast<uint32>(1));
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline bool IParser<TypeCustomDataType, SyntaxClassifierType>::hasSymbol(const uint32 symbolPosition) const noexcept
		{
			return (symbolPosition < _symbolTable.Size());
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline uint32 IParser<TypeCustomDataType, SyntaxClassifierType>::getSymbolPosition() const noexcept
		{
			return _symbolAt;
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline SymbolTableItem& IParser<TypeCustomDataType, SyntaxClassifierType>::getSymbol(const uint32 symbolPosition) const noexcept
		{
			return _symbolTable[symbolPosition];
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline bool IParser<TypeCustomDataType, SyntaxClassifierType>::findNextSymbol(const uint32 symbolPosition, const char* const cmp, uint32& outSymbolPosition) const noexcept
		{
			for (uint32 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.Size(); ++symbolIter)
			{
				const SymbolTableItem& symbol = _symbolTable[symbolIter];
				if (symbol._symbolString == cmp)
				{
					outSymbolPosition = symbolIter;
					return true;
				}
			}
			return false;
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline bool IParser<TypeCustomDataType, SyntaxClassifierType>::findNextSymbol(const uint32 symbolPosition, const SymbolClassifier symbolClassifier, uint32& outSymbolPosition) const noexcept
		{
			for (uint32 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.Size(); ++symbolIter)
			{
				const SymbolTableItem& symbol = _symbolTable[symbolIter];
				if (symbol._symbolClassifier == symbolClassifier)
				{
					outSymbolPosition = symbolIter;
					return true;
				}
			}
			return false;
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline bool IParser<TypeCustomDataType, SyntaxClassifierType>::findNextSymbolEither(const uint32 symbolPosition, const char* const cmp0, const char* const cmp1, uint32& outSymbolPosition) const noexcept
		{
			for (uint32 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.Size(); ++symbolIter)
			{
				const SymbolTableItem& symbol = _symbolTable[symbolIter];
				if (symbol._symbolString == cmp0 || symbol._symbolString == cmp1)
				{
					outSymbolPosition = symbolIter;
					return true;
				}
			}
			return false;
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline bool IParser<TypeCustomDataType, SyntaxClassifierType>::findNextDepthMatchingGrouperCloseSymbol(const uint32 openSymbolPosition, uint32* const outCloseSymbolPosition) const noexcept
		{
			const SymbolTableItem& openSymbol = getSymbol(openSymbolPosition);
			if (openSymbol._symbolClassifier != SymbolClassifier::Grouper_Open)
			{
				MINT_ASSERT(false, "symbolPosition 에 있는 Symbol 은 Grouper_Open 이어야 합니다!!!");
				return false;
			}

			int32 depth = 0;
			const char grouperClose = _lexer.getGrouperClose(openSymbol._symbolString[0]);
			for (uint32 symbolIter = openSymbolPosition + 1; symbolIter < _symbolTable.Size(); ++symbolIter)
			{
				const SymbolTableItem& symbol = _symbolTable[symbolIter];
				if (symbol._symbolString == openSymbol._symbolString)
				{
					++depth;
				}
				else if (symbol._symbolString[0] == grouperClose)
				{
					if (depth == 0)
					{
						if (outCloseSymbolPosition != nullptr)
						{
							*outCloseSymbolPosition = symbolIter;
						}
						MINT_ASSERT(symbol._symbolClassifier == SymbolClassifier::Grouper_Close, "Symbol 은 찾았지만 Grouper_Close 가 아닙니다!!!");
						return true;
					}

					--depth;
				}
			}

			return false;
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline void IParser<TypeCustomDataType, SyntaxClassifierType>::reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType)
		{
			_errorMessageArray.push_back(ErrorMessage(symbolTableItem, errorType));
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline void IParser<TypeCustomDataType, SyntaxClassifierType>::reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation)
		{
			_errorMessageArray.push_back(ErrorMessage(symbolTableItem, errorType, additionalExplanation));
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline bool IParser<TypeCustomDataType, SyntaxClassifierType>::hasReportedErrors() const noexcept
		{
			return !_errorMessageArray.IsEmpty();
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline uint32 IParser<TypeCustomDataType, SyntaxClassifierType>::getTypeMetaDataCount() const noexcept
		{
			return _typeMetaDatas.Size();
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline const TypeMetaData<TypeCustomDataType>& IParser<TypeCustomDataType, SyntaxClassifierType>::getTypeMetaData(const std::string& typeName) const noexcept
		{
			KeyValuePair found = _typeMetaDataMap.Find(typeName);
			MINT_ASSERT(found.IsValid() == true, "Type[%s] 가 존재하지 않습니다!", typeName.c_str());

			const uint32 typeIndex = *found._value;
			return _typeMetaDatas[typeIndex];
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline const TypeMetaData<TypeCustomDataType>& IParser<TypeCustomDataType, SyntaxClassifierType>::getTypeMetaData(const int32 typeIndex) const noexcept
		{
			return _typeMetaDatas[typeIndex];
		}

		template<typename TypeCustomDataType, typename SyntaxClassifierType>
		inline bool IParser<TypeCustomDataType, SyntaxClassifierType>::existsTypeMetaData(const std::string& typeName) const noexcept
		{
			return _typeMetaDataMap.Find(typeName).IsValid();
		}

		template<typename TypeCustomDataType, typename SyntaxClassifierType>
		inline void IParser<TypeCustomDataType, SyntaxClassifierType>::pushTypeMetaData(const std::string& typeName, const TypeMetaData<TypeCustomDataType>& typeMetaData) noexcept
		{
			_typeMetaDatas.PushBack(typeMetaData);
			_typeMetaDataMap.Insert(typeName, _typeMetaDatas.Size() - 1);
		}

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		inline TypeMetaData<TypeCustomDataType>& IParser<TypeCustomDataType, SyntaxClassifierType>::accessTypeMetaData(const std::string& typeName) noexcept
		{
			KeyValuePair found = _typeMetaDataMap.Find(typeName);
			MINT_ASSERT(found.IsValid() == true, "Type[%s] 가 존재하지 않습니다!", typeName.c_str());

			const uint32 typeIndex = *found._value;
			return _typeMetaDatas[typeIndex];
		}
#pragma endregion
	}
}
