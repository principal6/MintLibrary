#pragma once


#ifndef FS_LANGUAGE_COMMON_H
#define FS_LANGUAGE_COMMON_H


#include <CommonDefinitions.h>


namespace fs
{
	namespace Language
	{
		class ILexer;


		enum class SymbolClassifier : uint32
		{
			Delimiter,

			// ==========

			NumberLiteral,				// [0-9]+[.]*[0-9]*

			Keyword,					// if, else, class, ...

			GROUPER_BEGINS,
			//
			Grouper_Open,				// () {} []   단 <> 는 제외!!!
			Grouper_Close,				// () {} []   단 <> 는 제외!!!
			//
			GROUPER_ENDS,

			StringQuote,				// ' "

			StatementTerminator,		// ;

			Punctuator,					// ,

			OPERATOR_BEGINS,			// 두 개짜리 operator 의 첫 문자도 반드시 operator 다!?
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

			Identifier,					// [a-zA-Z_][a-zA-Z_0-9]+
			
			StringLiteral,

			// ==========

			SPECIAL_USE,
			POST_CLEARED,

			RESERVED
		};
		static_assert(SymbolClassifier::Delimiter			<	SymbolClassifier::NumberLiteral		);
		static_assert(SymbolClassifier::NumberLiteral		<	SymbolClassifier::Keyword			);
		static_assert(SymbolClassifier::Keyword				<	SymbolClassifier::GROUPER_BEGINS	);
		static_assert(SymbolClassifier::GROUPER_BEGINS		<	SymbolClassifier::GROUPER_ENDS		);
		static_assert(SymbolClassifier::OPERATOR_BEGINS		<	SymbolClassifier::OPERATOR_ENDS		);
		static_assert(SymbolClassifier::OPERATOR_ENDS		<	SymbolClassifier::Identifier		);
		

		class SymbolTableItem
		{
			friend ILexer;

		public:
									SymbolTableItem();
									SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString, const uint64 sourceAt);
									SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString);

		public:
			const bool				operator==(const SymbolTableItem& rhs) const noexcept;
			const bool				operator!=(const SymbolTableItem& rhs) const noexcept;

		public:
			void					clearData();
			const uint64			index() const noexcept;

		public:
			SymbolClassifier		_symbolClassifier;
			std::string				_symbolString;

		private:
			uint64					_symbolIndex;
		
		public:
			uint64					_sourceAt;
		};


		using SyntaxClassifierEnumType	= uint32;
		using SyntaxMainInfoType		= uint16;
		using SyntaxSubInfoType			= uint8;
		class SyntaxTreeItem
		{
		public:
												SyntaxTreeItem();
												SyntaxTreeItem(const SymbolTableItem& symbolTableItem);
												SyntaxTreeItem(const SymbolTableItem& symbolTableItem, const SyntaxClassifierEnumType syntaxClassifier);

		public:
			const SyntaxClassifierEnumType		getSyntaxClassifier() const noexcept;

		public:
			void								setMainInfo(const SyntaxMainInfoType syntaxMainInfo);
			void								setSubInfo(const SyntaxSubInfoType syntaxSubInfo);
			
		public:
			const SyntaxMainInfoType			getMainInfo() const noexcept;
			const SyntaxSubInfoType				getSubInfo() const noexcept;

		private:
			SyntaxClassifierEnumType			_syntaxClassifier;
			SyntaxMainInfoType					_syntaxMainInfo;
			SyntaxSubInfoType					_syntaxSubInfo;

		public:
			SymbolTableItem						_symbolTableItem;
		};
	}
}


#include <Language/LanguageCommon.inl>


#endif // !FS_LANGUAGE_COMMON_H
