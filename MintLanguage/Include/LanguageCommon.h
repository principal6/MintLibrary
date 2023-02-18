#pragma once


#ifndef _MINT_LANGUAGE_COMMON_H_
#define _MINT_LANGUAGE_COMMON_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	namespace Language
	{
		class ILexer;


		enum class SymbolClassifier : uint32
		{
			Delimiter,

			Escaper,

			// ==========

			NumberLiteral, // [0-9]+[.]*[0-9]*

			Keyword, // if, else, class, ...

			GROUPER_BEGINS,
			//
			Grouper_Open, // () {} [] 단 <> 는 제외!!!
			Grouper_Close, // () {} [] 단 <> 는 제외!!!
			//
			GROUPER_ENDS,

			StringQuote, // ' "

			StatementTerminator, // ;

			Punctuator, // ,

			OPERATOR_BEGINS, // 두 개짜리 operator 의 첫 문자도 반드시 operator 다!?
			//
			AssignmentOperator,
			RelationalOperator,
			ArithmeticOperator,
			LogicalOperator,
			BitwiseOperator,
			MemberAccessOperator,
			OperatorCandiate,
			//
			OPERATOR_ENDS,

			Identifier, // [a-zA-Z_][a-zA-Z_0-9]+

			StringLiteral,

			// ==========

			SPECIAL_USE,
			POST_CLEARED,

			RESERVED
		};
		static_assert(SymbolClassifier::Delimiter < SymbolClassifier::NumberLiteral);
		static_assert(SymbolClassifier::NumberLiteral < SymbolClassifier::Keyword);
		static_assert(SymbolClassifier::Keyword < SymbolClassifier::GROUPER_BEGINS);
		static_assert(SymbolClassifier::GROUPER_BEGINS < SymbolClassifier::GROUPER_ENDS);
		static_assert(SymbolClassifier::OPERATOR_BEGINS < SymbolClassifier::OPERATOR_ENDS);
		static_assert(SymbolClassifier::OPERATOR_ENDS < SymbolClassifier::Identifier);


		class SymbolTableItem
		{
			friend ILexer;

		public:
			SymbolTableItem();
			SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString, const uint32 sourceAt);
			SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString);

		public:
			bool operator==(const SymbolTableItem& rhs) const noexcept;
			bool operator!=(const SymbolTableItem& rhs) const noexcept;

		public:
			void ClearData();
			uint32 SymbolIndex() const noexcept;

		public:
			SymbolClassifier _symbolClassifier;
			std::string _symbolString;

		private:
			uint32 _symbolIndex;

		public:
			uint32 _sourceAt;
		};
	}
}


#include <MintLanguage/Include/LanguageCommon.inl>


#endif // !_MINT_LANGUAGE_COMMON_H_
