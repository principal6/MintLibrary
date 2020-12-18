#include "CppHlslParser.h"
#pragma once


namespace fs
{
	namespace Language
	{
		FS_INLINE const CppHlslSubInfo_AccessModifier convertStringToCppClassStructAccessModifier(const std::string& input)
		{
			if (input == "public")
			{
				return CppHlslSubInfo_AccessModifier::Public;
			}
			else if (input == "protected")
			{
				return CppHlslSubInfo_AccessModifier::Protected;
			}
			return CppHlslSubInfo_AccessModifier::Private;
		}


		inline CppHlslTypeInfo::CppHlslTypeInfo()
			: _isBuiltIn{ false }
			, _size{ 0 }
		{
			__noop;
		}

		inline CppHlslTypeInfo::CppHlslTypeInfo(const bool isBuiltIn, const std::string& fullName, const uint32 size)
			: _isBuiltIn{ isBuiltIn }
			, _fullName{ fullName }
			, _size{ size }
		{
			__noop;
		}

		FS_INLINE const uint32 CppHlslTypeInfo::getMemberCount() const noexcept
		{
			return static_cast<uint32>(_memberArray.size());
		}

		FS_INLINE const CppHlslTypeInfo& CppHlslTypeInfo::getMember(const uint32 memberIndex) const noexcept
		{
			return (memberIndex < getMemberCount()) ? _memberArray[memberIndex] : kInvalidTypeInfo;
		}


		inline CppHlslTypeTableItem::CppHlslTypeTableItem(const SymbolTableItem& typeSymbol, const CppHlslMainInfo_TypeModifierFlags& typeModifierFlags)
			: _typeSymbol{ typeSymbol }
			, _typeModifierFlags{ typeModifierFlags }
			, _typeSize{ 0 }
		{
			__noop;
		}

		inline CppHlslTypeTableItem::CppHlslTypeTableItem(const SymbolTableItem& typeSymbol, const CppHlslUserDefinedTypeInfo& userDefinedTypeInfo)
			: _typeSymbol{ typeSymbol }
			, _userDefinedTypeInfo{ userDefinedTypeInfo }
			, _typeSize{ 0 }
		{
			__noop;
		}

		inline CppHlslTypeTableItem::CppHlslTypeTableItem(const SyntaxTreeItem& typeSyntax)
			: CppHlslTypeTableItem(typeSyntax._symbolTableItem, static_cast<CppHlslMainInfo_TypeModifierFlags>(typeSyntax.getMainInfo()))
		{
			__noop;
		}

		FS_INLINE const std::string& CppHlslTypeTableItem::getTypeName() const noexcept
		{
			return _typeSymbol._symbolString;
		}

		FS_INLINE void CppHlslTypeTableItem::setTypeSize(const uint32 typeSize) noexcept
		{
			_typeSize = typeSize;
		}

		FS_INLINE const uint32 CppHlslTypeTableItem::getTypeSize() const noexcept
		{
			return _typeSize;
		}


		FS_INLINE const CppHlslMainInfo_TypeModifierFlags CppHlslTypeModifierSet::getTypeModifierFlags() const noexcept
		{
			const CppHlslMainInfo_TypeModifierFlags longFlags =
				(0 == _longState)
				? CppHlslMainInfo_TypeModifierFlags::NONE
				: (1 == _longState)
				  ? CppHlslMainInfo_TypeModifierFlags::Long
				  : CppHlslMainInfo_TypeModifierFlags::LongLong;

			const CppHlslMainInfo_TypeModifierFlags signFlags
				= static_cast<CppHlslMainInfo_TypeModifierFlags>(static_cast<int32>(_signState / 2) * static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Unsigned));

			return static_cast<CppHlslMainInfo_TypeModifierFlags>(
				static_cast<int>(_isConst)			* static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Const		) |
				static_cast<int>(_isConstexpr)		* static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Constexpr	) |
				static_cast<int>(_isMutable)		* static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Mutable		) |
				static_cast<int>(_isStatic)			* static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Static		) |
				static_cast<int>(_isThreadLocal)	* static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::ThreadLocal	) |
				static_cast<int>(_isShort)			* static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Short		) |
				static_cast<int32>(longFlags) | static_cast<int32>(signFlags)
				);
		}


		inline const CppHlslSyntaxClassifier CppHlslParser::convertSymbolToAccessModifierSyntax(const SymbolTableItem& symbol) noexcept
		{
			if (symbol._symbolString == "public" || symbol._symbolString == "protected" || symbol._symbolString == "private")
			{
				return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_AccessModifier;
			}

			return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_INVALID;
		}

		inline const CppHlslSyntaxClassifier CppHlslParser::convertLiteralSymbolToSyntax(const SymbolTableItem& symbol) noexcept
		{
			if (symbol._symbolClassifier == SymbolClassifier::NumberLiteral)
			{
				return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Literal_Number;
			}
			else if (symbol._symbolClassifier == SymbolClassifier::Keyword)
			{
				if (symbol._symbolString == "true" || symbol._symbolString == "false")
				{
					return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Literal_TrueFalse;
				}
				else if (symbol._symbolString == "nullptr")
				{
					return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Literal_Nullptr;
				}
			}
			else if (symbol._symbolClassifier == SymbolClassifier::StringLiteral)
			{
				return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Literal_String;
			}

			return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_INVALID;
		}
	}
}
