#pragma once


#ifndef _MINT_LANGUAGE_I_LEXER_H_
#define _MINT_LANGUAGE_I_LEXER_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>

#include <MintLanguage/Include/LanguageCommon.h>


namespace mint
{
	namespace Language
	{
		class ILexer;

		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		class IParser;


		enum class GrouperClassifier : uint8
		{
			Open,	// ( [ {
			Close,	// ) ] }

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

		enum class LineSkipperClassifier : uint8
		{
			SingleMarker,
			OpenMarker,
			CloseMarker,
			OpenCloseMarker,

			COUNT,
		};

		enum class LineSkipperSemantic : uint8
		{
			Comment,
			Preprocessor,

			COUNT,
		};

		static_assert(static_cast<uint32>(SymbolClassifier::OPERATOR_ENDS) - (static_cast<uint32>(SymbolClassifier::OPERATOR_BEGINS) + 1) == static_cast<uint32>(OperatorClassifier::COUNT));
		static_assert(static_cast<uint32>(SymbolClassifier::GROUPER_ENDS) - (static_cast<uint32>(SymbolClassifier::GROUPER_BEGINS) + 1) == static_cast<uint32>(GrouperClassifier::COUNT));
		static constexpr SymbolClassifier GetSymbolClassifierFromOperatorClassifier(const OperatorClassifier operatorClassifier);
		static constexpr SymbolClassifier GetSymbolClassifierFromGrouperClassifier(const GrouperClassifier grouperClassifier);

		struct GrouperTableItem
		{
		public:
			GrouperTableItem();
			GrouperTableItem(const char input, const GrouperClassifier grouperClassifier);

		public:
			MINT_INLINE GrouperClassifier GetClassifier() const noexcept { return _grouperClassifier; }

		private:
			char _input;
			GrouperClassifier _grouperClassifier;
		};

		class OperatorTableItem
		{
		public:
			OperatorTableItem();
			OperatorTableItem(const char* const string, const OperatorClassifier operatorClassifier);

		public:
			MINT_INLINE uint32 GetLength() const noexcept { return _length; }
			MINT_INLINE OperatorClassifier GetClassifier() const noexcept { return _operatorClassifier; }

		private:
			std::string _string;
			uint32 _length;
			OperatorClassifier _operatorClassifier;
		};

		class LineSkipperTableItem
		{
		public:
			LineSkipperTableItem();
			LineSkipperTableItem(const char* const string, const LineSkipperSemantic lineSkipperSemantic, const LineSkipperClassifier lineSkipperClassifier, const uint16 groupID);

		public:
			MINT_INLINE bool IsSameString(const LineSkipperTableItem& rhs) const noexcept { return _string == rhs._string; }
			MINT_INLINE bool IsSameGroup(const LineSkipperTableItem& rhs) const noexcept { return _groupID == rhs._groupID; }
			MINT_INLINE bool CheckSemantic(const LineSkipperSemantic lineSkipperSemantic) const noexcept { return _lineSkipperSemantic == lineSkipperSemantic; }
			MINT_INLINE bool CheckClassifier(const LineSkipperClassifier lineSkipperClassifier) const noexcept { return _lineSkipperClassifier == lineSkipperClassifier; }
			MINT_INLINE LineSkipperClassifier GetClassifier() const noexcept { return _lineSkipperClassifier; }

		private:
			std::string _string;
			uint16 _groupID;
			LineSkipperSemantic _lineSkipperSemantic;
			LineSkipperClassifier _lineSkipperClassifier;
		};


		// Lexical Analyzer
		// Tokens of lexeme
		// Also known as Scanner
		class ILexer
		{
			template <typename TypeCustomDataType, typename SyntaxClassifierType>
			friend class IParser;

		public:
			ILexer();
			virtual ~ILexer() = default;

		public:
			void SetSource(const std::string& source);

		protected:
			void SetEscaper(const char escaper);
			void SetStatementTerminator(const char statementTerminator);
			void RegisterDelimiter(const char delimiter);
			void RegisterLineSkipper(const char* const lineSkipperOpen, const char* const lineSkipperClose, const LineSkipperSemantic lineSkipperSemantic);
			void RegisterLineSkipper(const char* const lineSkipper, const LineSkipperSemantic lineSkipperSemantic);
			void RegisterKeyword(const char* const keyword);
			void RegisterGrouper(const char grouperOpen, const char grouperClose);
			void RegisterStringQuote(const char stringQuote);
			void RegisterPunctuator(const char* const punctuator);
			void RegisterOperator(const char* const operator_, const OperatorClassifier operatorClassifier);

		private:
			uint16 GetLineSkipperNextGroupID() noexcept;

		protected:
			virtual bool Execute() noexcept abstract;
			bool ExecuteDefault() noexcept;

		protected:
			bool ContinuesExecution(const uint32 sourceAt) const noexcept;
			char GetCh0(const uint32 sourceAt) const noexcept;
			char GetCh1(const uint32 sourceAt) const noexcept;
			char GetCh2(const uint32 sourceAt) const noexcept;
			void ExecuteInternalScanning(uint32& prevSourceAt, uint32& sourceAt);

		private:
			void AdvanceExecution(const SymbolClassifier symbolClassifier, const uint32 advance, uint32& prevSourceAt, uint32& sourceAt);

		protected:
			void EndExecution();

		private:
			void SetStringLiterals();
			void SetSymbolIndices();

		protected:
			bool IsDelimiter(const char input) const noexcept;
			bool IsLineSkipper(const char ch0, const char ch1, LineSkipperTableItem& out) const noexcept;
			bool IsStatementTerminator(const char input) const noexcept;
			bool IsGrouper(const char input, GrouperTableItem& out) const noexcept;
			bool IsStringQuote(const char input) const noexcept;
			bool IsPunctuator(const char ch0, const char ch1, const char ch2, uint32& outAdvance) const noexcept;
			bool IsOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept;
			bool IsNumber(const std::string& input) const noexcept;
			bool IsKeyword(const std::string& input) const noexcept;
			bool IsEscaper(const char input) const noexcept;
			MINT_INLINE void SetParsePlainEscaper(const bool value) noexcept { _parsePlainEscaper = value; }
			MINT_INLINE bool ParsePlainEscaper() const noexcept { return _parsePlainEscaper; }
			MINT_INLINE void SetDefaultSymbolClassifier(const SymbolClassifier defaultSymbolClassifier) noexcept { _defaultSymbolClassifier = defaultSymbolClassifier; }

		public:
			const Vector<SymbolTableItem>& GetSymbolTable() const noexcept;
			uint32 GetSymbolCount() const noexcept;
			const SymbolTableItem& GetSymbol(const uint32 symbolIndex) const noexcept;
			char GetGrouperClose(const char grouperOpen) const noexcept;

		protected:
			std::string _source;
			uint32 _totalTimeMs;

		private:
			char _escaper;
			char _statementTerminator;
			bool _parsePlainEscaper;
			SymbolClassifier _defaultSymbolClassifier;

		protected:
			HashMap<char, int8> _delimiterUmap;

		protected:
			Vector<LineSkipperTableItem> _lineSkipperTable;
			HashMap<uint64, uint32> _lineSkipperUmap;
			uint16 _lineSkipperNextGroupID;

		protected:
			Vector<std::string> _keywordTable;
			HashMap<uint64, uint32> _keywordUmap;

		protected:
			Vector<GrouperTableItem> _grouperTable;
			HashMap<char, uint32> _grouperUmap;
			HashMap<char, char> _grouperOpenToCloseMap;

		protected:
			HashMap<char, int8> _stringQuoteUmap;

		protected:
			Vector<std::string> _punctuatorTable;
			HashMap<uint64, uint32> _punctuatorUmap;

		protected:
			Vector<OperatorTableItem> _operatorTable;
			HashMap<uint64, uint32> _operatorUmap;

		protected:
			Vector<SymbolTableItem> _symbolTable;
		};
	}
}


#include <MintLanguage/Include/ILexer.inl>


#endif // !_MINT_LANGUAGE_I_LEXER_H_
