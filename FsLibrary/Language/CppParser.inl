#include "CppParser.h"
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


		FS_INLINE const std::string& CppTypeTableItem::getTypeName() const noexcept
		{
			return _typeName;
		}


		inline const CppSyntaxClassifier CppParser::convertSymbolToAccessModifierSyntax(const SymbolTableItem& symbol) noexcept
		{
			if (symbol._symbolString == "public" || symbol._symbolString == "protected" || symbol._symbolString == "private")
			{
				return CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_AccessModifier;
			}

			return CppSyntaxClassifier::CppSyntaxClassifier_INVALID;
		}

		inline const CppSyntaxClassifier CppParser::convertLiteralSymbolToSyntax(const SymbolTableItem& symbol) noexcept
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

		FS_INLINE const SymbolTableItem& CppParser::getClassStructAccessModifierSymbol(const CppClassStructAccessModifier cppClassStructAccessModifier) noexcept
		{
			return kClassStructAccessModifierSymbolArray[static_cast<uint8>(cppClassStructAccessModifier)];
		}

	}
}
