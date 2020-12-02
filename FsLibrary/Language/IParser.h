#pragma once


#ifndef FS_I_PARSER_H
#define FS_I_PARSER_H


#include <CommonDefinitions.h>

#include <Language/LanguageCommon.h>
#include <Container/Tree.h>


namespace fs
{
	namespace Language
	{
		class Lexer;


		class IParser abstract
		{
		protected:
			enum class ErrorType
			{
				GrouperMismatch,
				NoMatchingGrouper,
				WrongSuccessor,

				COUNT
			};

		private:
			static constexpr const char* const kErrorTypeString[]
			{
				"GrouperMismatch 열고 닫는 기호가 서로 일치하지 않습니다.",
				"NoMatchingGrouper 여는 기호만 있고 닫는 기호가 없습니다.",
				"WrongSuccessor 뒤에 온 것이 문법적으로 맞지 않습니다.",
			};

		protected:
			static constexpr const char* convertErrorTypeToString(const ErrorType errorType)
			{
				return kErrorTypeString[static_cast<uint32>(errorType)];
			}
			static_assert(static_cast<uint32>(ErrorType::COUNT) == ARRAYSIZE(kErrorTypeString));

			class ErrorMessage
			{
			public:
										ErrorMessage();
										ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
										ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);

			private:
				const uint64			_sourceAt;
				std::string				_message;
			};

		public:
													IParser(Lexer& lexer);
			virtual									~IParser() = default;

		public:
			virtual const bool						execute() abstract;

		protected:
			void									reset();

		protected:
			const bool								needToContinueParsing() const noexcept;

		protected:
			void									advancePosition(const uint64 advanceCount);
			void									advancePositionTo(const uint64 nextSymbolPosition);

		protected:
			const bool								hasNextSymbols(const uint64 nextSymbolCount) const noexcept;
			const bool								hasNextSymbols(const uint64 symbolPosition, const uint64 nextSymbolCount) const noexcept;
			const uint64							getSymbolPosition() const noexcept;
			SymbolTableItem&						getSymbol(const uint64 symbolPosition) const noexcept;
			SymbolTableItem&						getCurrentSymbol() const noexcept;

		protected:
			const bool								findNextSymbol(const uint64 symbolPosition, const char* const cmp, uint64& outSymbolPosition) const noexcept;
			const bool								findNextSymbolEither(const uint64 symbolPosition, const char* const cmp0, const char* const cmp1, uint64& outSymbolPosition) const noexcept;
		
		protected:
			void									reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
			void									reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);
			
		protected:
			Lexer&									_lexer;
			std::vector<SymbolTableItem>&			_symbolTable;
			uint64									_symbolCount;

		private:
			fs::Tree<SyntaxTreeItem>				_syntaxTree;
		
		protected:
			fs::TreeNodeAccessor<SyntaxTreeItem>	_syntaxTreeCurrentParentNode;

		private:
			uint64									_symbolAt;
			std::vector<ErrorMessage>				_errorMessageArray;
		};
	}
}


#endif // !FS_I_PARSER_H
