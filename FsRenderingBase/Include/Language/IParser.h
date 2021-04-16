#pragma once


#ifndef FS_I_PARSER_H
#define FS_I_PARSER_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsContainer/Include/Vector.h>
#include <FsContainer/Include/Tree.h>

#include <FsRenderingBase/Include/Language/LanguageCommon.h>


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
                { "Grouper mismatch"                    ,  "열고 닫는 기호가 서로 일치하지 않습니다."                                      },
                { "No matching grouper"                 ,  "여는 기호만 있고 닫는 기호가 없습니다."                                        },
                { "Wrong predecessor"                   ,  "앞에 온 것이 문법적으로 맞지 않습니다."                                        },
                { "Wrong successor"                     ,  "뒤에 온 것이 문법적으로 맞지 않습니다."                                        },
                { "Lack of code"                        ,  "더 있어야 할 코드가 없습니다."                                                 },
                { "Repetition of code"                  ,  "코드가 중복됩니다."                                                            },
                { "Symbol not found"                    ,  "해당 심볼을 찾을 수 없습니다."                                                 },
                { "Wrong scope"                         ,  "이곳에 사용할 수 없습니다."                                                    },
            };

        protected:
            static_assert(static_cast<uint32>(ErrorType::COUNT) == ARRAYSIZE(kErrorTypeStringArray));
            static constexpr const char*                    convertErrorTypeToTypeString(const ErrorType errorType);
            static constexpr const char*                    convertErrorTypeToContentString(const ErrorType errorType);

            class ErrorMessage
            {
            public:
                                                            ErrorMessage();
                                                            ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
                                                            ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);

            private:
                const uint32                                _sourceAt;
                std::string                                 _message;
            };

        public:
                                                            IParser(ILexer& lexer);
            virtual                                         ~IParser() = default;

        public:
            virtual const bool                              execute() abstract;

        public:
            const fs::Tree<SyntaxTreeItem>&                 getSyntaxTree() const noexcept;
            const std::string                               getSyntaxTreeString() noexcept;

        private:
            void                                            getSyntaxTreeStringInternal(const uint32 headSpace, const TreeNodeAccessor<SyntaxTreeItem>& node, const uint32 depth, std::string& outResult) noexcept;

        protected:
            void                                            reset();

        protected:
            const bool                                      needToContinueParsing() const noexcept;

        protected:
            void                                            advanceSymbolPositionXXX(const uint32 advanceCount);

        protected:
            const bool                                      hasSymbol(const uint32 symbolPosition) const noexcept;
            const uint32                                    getSymbolPosition() const noexcept;
            SymbolTableItem&                                getSymbol(const uint32 symbolPosition) const noexcept;

        protected:
            const bool                                      findNextSymbol(const uint32 symbolPosition, const char* const cmp, uint32& outSymbolPosition) const noexcept;
            const bool                                      findNextSymbol(const uint32 symbolPosition, const SymbolClassifier symbolClassifier, uint32& outSymbolPosition) const noexcept;
            const bool                                      findNextSymbolEither(const uint32 symbolPosition, const char* const cmp0, const char* const cmp1, uint32& outSymbolPosition) const noexcept;
            const bool                                      findNextDepthMatchingCloseSymbol(const uint32 openSymbolPosition, const char* const closeSymbolString, uint32& outSymbolPosition) const noexcept;
        
        protected:
            void                                            reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
            void                                            reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);
            const bool                                      hasReportedErrors() const noexcept;

        protected:
            fs::TreeNodeAccessor<SyntaxTreeItem>            getSyntaxTreeRootNode() noexcept;
            
        protected:
            ILexer&                                         _lexer;
            fs::Vector<SymbolTableItem>&                    _symbolTable;

        private:
            fs::Tree<SyntaxTreeItem>                        _syntaxTree;
        
        private:
            uint32                                          _symbolAt;
            fs::Vector<ErrorMessage>                        _errorMessageArray;
            static const SymbolTableItem                    kRootSymbol;
        };
    }
}


#include <FsRenderingBase/Include/Language/IParser.inl>


#endif // !FS_I_PARSER_H
