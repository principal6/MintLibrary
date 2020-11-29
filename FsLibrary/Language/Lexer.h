#pragma once


#ifndef FS_LEXER_H
#define FS_LEXER_H


#include <CommonDefinitions.h>

#include <Container/Vector.h>
#include <Container/DynamicString.h>


namespace fs
{
	namespace Language
	{
		enum GrouperClassifier
		{
			GrouperClassifier_Open,						// ( [ {
			GrouperClassifier_Close,					// ) ] }

			GrouperClassifier_COUNT
		};

		enum OperatorClassifier
		{
			OperatorClassifier_AssignmentOperator,		// = += -= *= /= %= &= |= ^=
			OperatorClassifier_RelationalOperator,		// < <= > >= == !=
			OperatorClassifier_ArithmeticOperator,		// + - * / % ++() ()++ --() ()--
			OperatorClassifier_LogicalOperator,			// && || !
			OperatorClassifier_BitwiseOperator,			// & | ^ ~
			OperatorClassifier_MemberAccessOperator,	// .
			OperatorClassifier_OperatorCandiate,		// ? :

			OperatorClassifier_COUNT
		};

		enum SymbolClassifier
		{
			SymbolClassifier_Delimiter,

			SymbolClassifier_NumberLiteral,				// [0-9]+[.]*[0-9]*

			SymbolClassifier_Keyword,					// if, else, class, ...

			SymbolClassifier_GROUPER_BEGINS,
			//
			SymbolClassifier_Grouper_Open,				// () {} []   단 <> 는 제외!!!
			SymbolClassifier_Grouper_Close,				// () {} []   단 <> 는 제외!!!
			//
			SymbolClassifier_GROUPER_ENDS,

			SymbolClassifier_StringQuote,				// ' "

			SymbolClassifier_StatementTerminator,		// ;

			SymbolClassifier_Punctuator,				// ,

			SymbolClassifier_OPERATOR_BEGINS,			// 두 개짜리 operator 의 첫 문자도 반드시 operator 다!!!
			//
			SymbolClassifier_AssignmentOperator,
			SymbolClassifier_RelationalOperator,
			SymbolClassifier_ArithmeticOperator,
			SymbolClassifier_LogicalOperator,
			SymbolClassifier_BitwiseOperator,
			SymbolClassifier_MemberAccessOperator,
			SymbolClassifier_OperatorCandiate,
			//
			SymbolClassifier_OPERATOR_ENDS,

			SymbolClassifier_Identifier,				// [a-zA-Z_][a-zA-Z_0-9]+
		};
		static_assert(SymbolClassifier::SymbolClassifier_Delimiter			< SymbolClassifier::SymbolClassifier_NumberLiteral		);
		static_assert(SymbolClassifier::SymbolClassifier_NumberLiteral		< SymbolClassifier::SymbolClassifier_Keyword			);
		static_assert(SymbolClassifier::SymbolClassifier_Keyword			< SymbolClassifier::SymbolClassifier_GROUPER_BEGINS		);
		static_assert(SymbolClassifier::SymbolClassifier_GROUPER_BEGINS		< SymbolClassifier::SymbolClassifier_GROUPER_ENDS		);
		static_assert(SymbolClassifier::SymbolClassifier_OPERATOR_BEGINS	< SymbolClassifier::SymbolClassifier_OPERATOR_ENDS		);
		static_assert(SymbolClassifier::SymbolClassifier_OPERATOR_ENDS		< SymbolClassifier::SymbolClassifier_Identifier			);
		static_assert(SymbolClassifier::SymbolClassifier_OPERATOR_ENDS - (SymbolClassifier::SymbolClassifier_OPERATOR_BEGINS + 1) == OperatorClassifier::OperatorClassifier_COUNT);
		static_assert(SymbolClassifier::SymbolClassifier_GROUPER_ENDS - (SymbolClassifier::SymbolClassifier_GROUPER_BEGINS + 1) == GrouperClassifier::GrouperClassifier_COUNT);
		FS_INLINE static constexpr const SymbolClassifier getSymbolClassifierFromOperatorClassifier(const OperatorClassifier operatorClassifier)
		{
			return static_cast<SymbolClassifier>(SymbolClassifier::SymbolClassifier_OPERATOR_BEGINS + 1 + operatorClassifier);
		}
		FS_INLINE static constexpr const SymbolClassifier getSymbolClassifierFromGrouperClassifier(const GrouperClassifier grouperClassifier)
		{
			return static_cast<SymbolClassifier>(SymbolClassifier::SymbolClassifier_GROUPER_BEGINS + 1 + grouperClassifier);
		}

		struct GrouperTableItem
		{
								GrouperTableItem()
									: _input{ '\0' }
									, _grouperClassifier{ GrouperClassifier::GrouperClassifier_COUNT }
								{
									__noop;
								}
								GrouperTableItem(const char input, const GrouperClassifier grouperClassifier)
									: _input{ input }
									, _grouperClassifier { grouperClassifier }
								{
									__noop;
								}

			char				_input;
			GrouperClassifier	_grouperClassifier;
		};

		struct OperatorTableItem
		{
								OperatorTableItem()
									: _length{ 0 }
									, _operatorClassifier{ OperatorClassifier::OperatorClassifier_COUNT }
								{
									__noop;
								}
								OperatorTableItem(const char* const string, const OperatorClassifier operatorClassifier) 
									: _string{ string }
									, _operatorClassifier{ operatorClassifier }
								{
									_length = _string.length();
								}

			std::string			_string;
			uint64				_length;
			OperatorClassifier	_operatorClassifier;
		};

		struct SymbolTableItem
		{
									SymbolTableItem()
										: _stringIndex{ 0 }
										, _sourceAt{ 0 }
										, _symbolClassifier{ SymbolClassifier::SymbolClassifier_Identifier }
									{
										__noop;
									}

#if defined FS_DEBUG
									SymbolTableItem(const uint64 stringIndex, const std::string& stringForDebug, const uint64 sourceAt, const SymbolClassifier symbolClassifier)
										: _stringIndex{ stringIndex }
										, _stringForDebug{ stringForDebug }
										, _sourceAt{ sourceAt }
										, _symbolClassifier{ symbolClassifier }
									{
										__noop;
									}
#else
									SymbolTableItem(const uint64 stringIndex, const uint64 sourceAt, const SymbolClassifier symbolClassifier)
										: _stringIndex{ stringIndex }
										, _sourceAt{ sourceAt }
										, _symbolClassifier{ symbolClassifier }
									{
										__noop;
									}
#endif

			uint64					_stringIndex;
#if defined FS_DEBUG
			std::string				_stringForDebug;
#endif
			uint64					_sourceAt;
			SymbolClassifier		_symbolClassifier;
		};


		// Lexical Analyzer
		// Tokens of lexeme
		// Also known as Scanner
		class Lexer
		{
		public:
													Lexer(const std::string& source);
													~Lexer() = default;

		public:
			void									setEscaper(const char escaper);
			void									setStatementTerminator(const char statementTerminator);
			void									registerDelimiter(const char delimiter);
			void									registerKeyword(const char* const keyword);
			void									registerGrouper(const char grouper, const GrouperClassifier grouperClassifier);
			void									registerStringQuote(const char stringQuote);
			void									registerOperator(const char* const operator_, const OperatorClassifier operatorClassifier);

		public:
			void									execute();

		private:
			const bool								isDelimiter(const char input) const noexcept;
			const bool								isStatementTerminator(const char input) const noexcept;
			const bool								isGrouper(const char input, GrouperTableItem& out) const noexcept;
			const bool								isStringQuote(const char input) const noexcept;
			const bool								isOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept;
			const bool								isNumber(const std::string& input) const noexcept;
			const bool								isKeyword(const std::string& input) const noexcept;

		private:
			const uint64							getStringIndex(const char ch0, const char ch1) noexcept;
			const uint64							getStringIndex(const std::string& input) noexcept;

		public:
			const uint32							getSymbolCount() const noexcept;
			const SymbolTableItem&					getSymbol(const uint32 symbolIndex) const noexcept;

		private:
			std::string								_source;
			uint64									_totalTimeMs;

		private:
			char									_escaper;
			char									_statementTerminator;

		private:
			std::unordered_map<char, int8>			_delimiterUmap;

		private:
			std::vector<std::string>				_keywordTable;
			std::unordered_map<uint64, uint64>		_keywordUmap;

		private:
			std::vector<GrouperTableItem>			_grouperTable;
			std::unordered_map<char, uint64>		_grouperUmap;

		private:
			std::unordered_map<char, int8>			_stringQuoteUmap;

		private:
			std::vector<OperatorTableItem>			_operatorTable;
			std::unordered_map<uint64, uint64>		_operatorUmap;

		private:
			std::vector<std::string>				_stringTable;
			std::unordered_map<uint64, uint64>		_stringTableUmap;

		private:
			std::vector<SymbolTableItem>			_symbolTable;
		};
	}
}


#endif // !FS_LEXER_H
