#pragma once


#ifndef FS_I_PARSER_H
#define FS_I_PARSER_H


#include <CommonDefinitions.h>

#include <FsLibrary/Container/Tree.h>

#include <FsLibrary/Language/LanguageCommon.h>


namespace fs
{
	namespace Language
	{
		class ILexer;


		class IParser abstract
		{
		protected:
			enum class ErrorType
			{
				GrouperMismatch,
				NoMatchingGrouper,
				WrongPredecessor,
				WrongSuccessor,
				LackOfCode,
				RepetitionOfCode,
				SymbolNotFound,
				WrongScope,

				COUNT
			};

		private:
			static constexpr const char* const kErrorTypeStringArray[][2]
			{
				{ "Grouper mismatch"                   ,  "열고 닫는 기호가 서로 일치하지 않습니다."                                       },
				{ "No matching grouper"                ,  "여는 기호만 있고 닫는 기호가 없습니다."                                         },
				{ "Wrong predecessor"                  ,  "앞에 온 것이 문법적으로 맞지 않습니다."                                         },
				{ "Wrong successor"                    ,  "뒤에 온 것이 문법적으로 맞지 않습니다."                                         },
				{ "Lack of code"                       ,  "더 있어야 할 코드가 없습니다."                                                  },
				{ "Repetition of code"                 ,  "코드가 중복됩니다."													           },
				{ "Symbol not found"				   ,  "해당 심볼을 찾을 수 없습니다."												   },
				{ "Wrong scope"				           ,  "이곳에 사용할 수 없습니다."												       },
			};

		protected:
			static_assert(static_cast<uint32>(ErrorType::COUNT) == ARRAYSIZE(kErrorTypeStringArray));
			static constexpr const char*					convertErrorTypeToTypeString(const ErrorType errorType);
			static constexpr const char*					convertErrorTypeToContentString(const ErrorType errorType);

			class ErrorMessage
			{
			public:
															ErrorMessage();
															ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
															ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);

			private:
				const uint64								_sourceAt;
				std::string									_message;
			};

		public:
															IParser(ILexer& lexer);
			virtual											~IParser() = default;

		public:
			virtual const bool								execute() abstract;

		public:
			const fs::Tree<SyntaxTreeItem>&					getSyntaxTree() const noexcept;
			const std::string								getSyntaxTreeString() noexcept;

		private:
			void											getSyntaxTreeStringInternal(const uint64 headSpace, const TreeNodeAccessor<SyntaxTreeItem>& node, const uint64 depth, std::string& outResult) noexcept;

		protected:
			void											reset();

		protected:
			const bool										needToContinueParsing() const noexcept;

		protected:
			void											advanceSymbolPositionXXX(const uint64 advanceCount);

		protected:
			const bool										hasSymbol(const uint64 symbolPosition) const noexcept;
			const uint64									getSymbolPosition() const noexcept;
			SymbolTableItem&								getSymbol(const uint64 symbolPosition) const noexcept;

		protected:
			const bool										findNextSymbol(const uint64 symbolPosition, const char* const cmp, uint64& outSymbolPosition) const noexcept;
			const bool										findNextSymbol(const uint64 symbolPosition, const SymbolClassifier symbolClassifier, uint64& outSymbolPosition) const noexcept;
			const bool										findNextSymbolEither(const uint64 symbolPosition, const char* const cmp0, const char* const cmp1, uint64& outSymbolPosition) const noexcept;
			const bool										findNextDepthMatchingCloseSymbol(const uint64 openSymbolPosition, const char* const closeSymbolString, uint64& outSymbolPosition) const noexcept;
		
		protected:
			void											reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
			void											reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);
			const bool										hasReportedErrors() const noexcept;

		protected:
			fs::TreeNodeAccessor<SyntaxTreeItem>			getSyntaxTreeRootNode() noexcept;
			
		protected:
			ILexer&											_lexer;
			std::vector<SymbolTableItem>&					_symbolTable;

		private:
			fs::Tree<SyntaxTreeItem>						_syntaxTree;
		
		private:
			uint64											_symbolAt;
			std::vector<ErrorMessage>						_errorMessageArray;
			static const SymbolTableItem					kRootSymbol;
		};
	}
}


#include <FsLibrary/Language/IParser.inl>


#endif // !FS_I_PARSER_H
