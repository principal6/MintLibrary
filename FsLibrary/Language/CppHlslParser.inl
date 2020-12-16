#include "CppHlslParser.h"
#pragma once


namespace fs
{
	namespace Language
	{
		FS_INLINE const CppSubInfo_AccessModifier convertStringToCppClassStructAccessModifier(const std::string& input)
		{
			if (input == "public")
			{
				return CppSubInfo_AccessModifier::Public;
			}
			else if (input == "protected")
			{
				return CppSubInfo_AccessModifier::Protected;
			}
			return CppSubInfo_AccessModifier::Private;
		}


		inline CppTypeTableItem::CppTypeTableItem(const SymbolTableItem& typeSymbol, const CppMainInfo_TypeFlags& typeFlags)
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
			: CppTypeTableItem(typeSyntax._symbolTableItem, static_cast<CppMainInfo_TypeFlags>(typeSyntax.getMainInfo()))
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


		FS_INLINE const CppMainInfo_TypeFlags CppTypeModifierSet::getTypeFlags() const noexcept
		{
			const CppMainInfo_TypeFlags longFlags =
				(0 == _longState)
				? CppMainInfo_TypeFlags::NONE
				: (1 == _longState)
				  ? CppMainInfo_TypeFlags::Long
				  : CppMainInfo_TypeFlags::LongLong;

			const CppMainInfo_TypeFlags signFlags
				= static_cast<CppMainInfo_TypeFlags>(static_cast<int32>(_signState / 2) * static_cast<int32>(CppMainInfo_TypeFlags::Unsigned));

			return static_cast<CppMainInfo_TypeFlags>(
				static_cast<int>(_isConst)			* static_cast<int32>(CppMainInfo_TypeFlags::Const		) |
				static_cast<int>(_isConstexpr)		* static_cast<int32>(CppMainInfo_TypeFlags::Constexpr	) |
				static_cast<int>(_isMutable)		* static_cast<int32>(CppMainInfo_TypeFlags::Mutable		) |
				static_cast<int>(_isStatic)			* static_cast<int32>(CppMainInfo_TypeFlags::Static		) |
				static_cast<int>(_isThreadLocal)	* static_cast<int32>(CppMainInfo_TypeFlags::ThreadLocal	) |
				static_cast<int>(_isShort)			* static_cast<int32>(CppMainInfo_TypeFlags::Short		) |
				static_cast<int32>(longFlags) | static_cast<int32>(signFlags)
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
	}
}
