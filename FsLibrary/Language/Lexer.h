#pragma once


#ifndef FS_LEXER_H
#define FS_LEXER_H


#include <CommonDefinitions.h>

#include <Container/Vector.h>
#include <Container/DynamicString.h>

#include <Language/LanguageCommon.h>


namespace fs
{
	namespace Language
	{
		class IParser;


		enum class GrouperClassifier : uint8
		{
			Open,					// ( [ {
			Close,					// ) ] }

			COUNT
		};

		enum class OperatorClassifier : uint8
		{
			AssignmentOperator,		// = += -= *= /= %= &= |= ^=
			RelationalOperator,		// < <= > >= == !=
			ArithmeticOperator,		// + - * / % ++() ()++ --() ()--
			LogicalOperator,		// && || !
			BitwiseOperator,		// & | ^ ~
			MemberAccessOperator,	// .
			OperatorCandiate,		// ? :

			COUNT
		};

		enum class CommentMarkerClassifier : uint8
		{
			SingleMarker,
			OpenMarker,
			CloseMarker,
			OpenCloseMarker,
			
			COUNT,
		};

		static_assert(static_cast<uint32>(SymbolClassifier::OPERATOR_ENDS) - (static_cast<uint32>(SymbolClassifier::OPERATOR_BEGINS) + 1) == static_cast<uint32>(OperatorClassifier::COUNT));
		static_assert(static_cast<uint32>(SymbolClassifier::GROUPER_ENDS ) - (static_cast<uint32>(SymbolClassifier::GROUPER_BEGINS ) + 1) == static_cast<uint32>(GrouperClassifier::COUNT));
		static constexpr const SymbolClassifier getSymbolClassifierFromOperatorClassifier(const OperatorClassifier operatorClassifier);
		static constexpr const SymbolClassifier getSymbolClassifierFromGrouperClassifier(const GrouperClassifier grouperClassifier);

		struct GrouperTableItem
		{
									GrouperTableItem()
										: _input{ '\0' }
										, _grouperClassifier{ GrouperClassifier::COUNT }
									{
										__noop;
									}
									GrouperTableItem(const char input, const GrouperClassifier grouperClassifier)
										: _input{ input }
										, _grouperClassifier { grouperClassifier }
									{
										__noop;
									}

			char					_input;
			GrouperClassifier		_grouperClassifier;
		};

		struct OperatorTableItem
		{
									OperatorTableItem()
										: _length{ 0 }
										, _operatorClassifier{ OperatorClassifier::COUNT }
									{
										__noop;
									}
									OperatorTableItem(const char* const string, const OperatorClassifier operatorClassifier) 
										: _string{ string }
										, _operatorClassifier{ operatorClassifier }
									{
										_length = _string.length();
									}

			std::string				_string;
			uint64					_length;
			OperatorClassifier		_operatorClassifier;
		};

		struct CommentMarkerTableItem
		{
									CommentMarkerTableItem()
										: _length{ 0 }
										, _commentMarkerGroupId{ kUint16Max }
										, _commentMarkerClassifier{ CommentMarkerClassifier::COUNT }
									{
										__noop;
									}

									CommentMarkerTableItem(const char* const string, const CommentMarkerClassifier commentMarkerClassifier, const uint16 commentMarkerGroupId)
										: _string{ string }
										, _commentMarkerGroupId{ commentMarkerGroupId }
										, _commentMarkerClassifier{ commentMarkerClassifier }
									{
										_length = _string.length();
									}

			static const uint16		getNextGroupId()
			{
				const uint16 result = _commentMarkerNextGroupId;
				++_commentMarkerNextGroupId;
				return result;
			}

			std::string				_string;
			uint64					_length;
			uint16					_commentMarkerGroupId;
			CommentMarkerClassifier	_commentMarkerClassifier;

		private:
			static uint16			_commentMarkerNextGroupId;
		};


		// Lexical Analyzer
		// Tokens of lexeme
		// Also known as Scanner
		// Uses STL (vector, string, unordered_map)
		class Lexer
		{
			friend IParser;

		public:
													Lexer(const std::string& source);
													~Lexer() = default;

		public:
			void									setEscaper(const char escaper);
			void									setStatementTerminator(const char statementTerminator);
			void									registerCommentMarker(const char* const commentMarker);
			void									registerCommentMarker(const char* const commentMarkerOpen, const char* const commentMarkerClose);
			void									registerDelimiter(const char delimiter);
			void									registerKeyword(const char* const keyword);
			void									registerGrouper(const char grouper, const GrouperClassifier grouperClassifier);
			void									registerStringQuote(const char stringQuote);
			void									registerPunctuator(const char* const punctuator);
			void									registerOperator(const char* const operator_, const OperatorClassifier operatorClassifier);

		public:
			const bool								execute();
			void									updateSymbolIndex();

		private:
			const bool								isCommentMarker(const char ch0, const char ch1, CommentMarkerTableItem& out) const noexcept;
			const bool								isDelimiter(const char input) const noexcept;
			const bool								isStatementTerminator(const char input) const noexcept;
			const bool								isGrouper(const char input, GrouperTableItem& out) const noexcept;
			const bool								isStringQuote(const char input) const noexcept;
			const bool								isPunctuator(const char ch0, const char ch1, const char ch2, uint64& outAdvance) const noexcept;
			const bool								isOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept;
			const bool								isNumber(const std::string& input) const noexcept;
			const bool								isKeyword(const std::string& input) const noexcept;

		public:
			const std::vector<SymbolTableItem>&		getSymbolTable() const noexcept;
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
			std::vector<CommentMarkerTableItem>		_commentMarkerTable;
			std::unordered_map<uint64, uint64>		_commentMarkerUmap;

		private:
			std::vector<std::string>				_keywordTable;
			std::unordered_map<uint64, uint64>		_keywordUmap;

		private:
			std::vector<GrouperTableItem>			_grouperTable;
			std::unordered_map<char, uint64>		_grouperUmap;

		private:
			std::unordered_map<char, int8>			_stringQuoteUmap;

		private:
			std::vector<std::string>				_punctuatorTable;
			std::unordered_map<uint64, uint64>		_punctuatorUmap;

		private:
			std::vector<OperatorTableItem>			_operatorTable;
			std::unordered_map<uint64, uint64>		_operatorUmap;

		private:
			std::vector<SymbolTableItem>			_symbolTable;
		};
	}
}


#include <Language/Lexer.inl>


#endif // !FS_LEXER_H
