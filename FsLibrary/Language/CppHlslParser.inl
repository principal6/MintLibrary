#include "CppHlslParser.h"
#pragma once


namespace fs
{
	namespace Language
	{
		FS_INLINE static const CppClassStructAccessModifier convertStringToCppClassStructAccessModifier(const std::string& input)
		{
			if (input == "public")
			{
				return CppClassStructAccessModifier::Public;
			}
			else if (input == "protected")
			{
				return CppClassStructAccessModifier::Protected;
			}
			return CppClassStructAccessModifier::Private;
		}

		FS_INLINE static const std::string& convertCppClassStructAccessModifierToString(const CppClassStructAccessModifier input)
		{
			static const std::string kPublic		= "public";
			static const std::string kProtected		= "protected";
			static const std::string kPrivate		= "private";
			if (input == CppClassStructAccessModifier::Public)
			{
				return kPublic;
			}
			else if (input == CppClassStructAccessModifier::Protected)
			{
				return kProtected;
			}
			return kPrivate;
		}


		inline CppTypeTableItem::CppTypeTableItem(const SymbolTableItem& typeSymbol, const CppAdditionalInfo_TypeFlags& typeFlags)
			: _typeSymbol{ typeSymbol }
			, _typeFlags{ typeFlags }
			, _typeSize{ 0 }
		{
			__noop;
		}

		inline CppTypeTableItem::CppTypeTableItem(const SymbolTableItem& typeSymbol, const CppUserDefinedTypeInfo& userDefinedTypeInfo)
			: _typeSymbol{ typeSymbol }
			, _userDefinedTypeInfo{ userDefinedTypeInfo }
			, _typeSize{ 0 }
		{
			__noop;
		}

		inline CppTypeTableItem::CppTypeTableItem(const SyntaxTreeItem& typeSyntax)
			: CppTypeTableItem(typeSyntax._symbolTableItem, static_cast<CppAdditionalInfo_TypeFlags>(typeSyntax.getAdditionalInfo()))
		{
			__noop;
		}

		FS_INLINE const std::string& CppTypeTableItem::getTypeName() const noexcept
		{
			return _typeSymbol._symbolString;
		}

		FS_INLINE void CppTypeTableItem::setTypeSize(const uint32 typeSize) noexcept
		{
			_typeSize = typeSize;
		}

		FS_INLINE const uint32 CppTypeTableItem::getTypeSize() const noexcept
		{
			return _typeSize;
		}


		FS_INLINE const CppAdditionalInfo_TypeFlags CppTypeModifierSet::getTypeFlags() const noexcept
		{
			const CppAdditionalInfo_TypeFlags longFlags =
				(0 == _longState)
				? CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_NONE
				: (1 == _longState)
				? CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Long
				: CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_LongLong;

			const CppAdditionalInfo_TypeFlags signFlags
				= static_cast<CppAdditionalInfo_TypeFlags>(static_cast<int>(_signState / 2) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Unsigned);

			return static_cast<CppAdditionalInfo_TypeFlags>(
				static_cast<int>(_isConst) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Const |
				static_cast<int>(_isConstexpr) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Constexpr |
				static_cast<int>(_isMutable) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Mutable |
				static_cast<int>(_isStatic) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Static |
				static_cast<int>(_isThreadLocal) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_ThreadLocal |
				static_cast<int>(_isShort) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Short |
				longFlags | signFlags
				);
		}


		inline const CppSyntaxClassifier CppHlslParser::convertSymbolToAccessModifierSyntax(const SymbolTableItem& symbol) noexcept
		{
			if (symbol._symbolString == "public" || symbol._symbolString == "protected" || symbol._symbolString == "private")
			{
				return CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_AccessModifier;
			}

			return CppSyntaxClassifier::CppSyntaxClassifier_INVALID;
		}

		inline const CppSyntaxClassifier CppHlslParser::convertLiteralSymbolToSyntax(const SymbolTableItem& symbol) noexcept
		{
			if (symbol._symbolClassifier == SymbolClassifier::NumberLiteral)
			{
				return CppSyntaxClassifier::CppSyntaxClassifier_Literal_Number;
			}
			else if (symbol._symbolClassifier == SymbolClassifier::Keyword)
			{
				if (symbol._symbolString == "true" || symbol._symbolString == "false")
				{
					return CppSyntaxClassifier::CppSyntaxClassifier_Literal_TrueFalse;
				}
				else if (symbol._symbolString == "nullptr")
				{
					return CppSyntaxClassifier::CppSyntaxClassifier_Literal_Nullptr;
				}
			}
			else if (symbol._symbolClassifier == SymbolClassifier::StringLiteral)
			{
				return CppSyntaxClassifier::CppSyntaxClassifier_Literal_String;
			}

			return CppSyntaxClassifier::CppSyntaxClassifier_INVALID;
		}

		FS_INLINE const SymbolTableItem& CppHlslParser::getClassStructAccessModifierSymbol(const CppClassStructAccessModifier cppClassStructAccessModifier) noexcept
		{
			return kClassStructAccessModifierSymbolArray[static_cast<uint8>(cppClassStructAccessModifier)];
		}
	}
}
