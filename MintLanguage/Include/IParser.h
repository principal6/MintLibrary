#pragma once


#ifndef MINT_LANGUAGE_I_PARSER_H
#define MINT_LANGUAGE_I_PARSER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/Tree.h>

#include <MintLanguage/Include/LanguageCommon.h>


namespace mint
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
                { "Grouper mismatch"                    ,  "���� �ݴ� ��ȣ�� ���� ��ġ���� �ʽ��ϴ�."                                      },
                { "No matching grouper"                 ,  "���� ��ȣ�� �ְ� �ݴ� ��ȣ�� �����ϴ�."                                        },
                { "Wrong predecessor"                   ,  "�տ� �� ���� ���������� ���� �ʽ��ϴ�."                                        },
                { "Wrong successor"                     ,  "�ڿ� �� ���� ���������� ���� �ʽ��ϴ�."                                        },
                { "Lack of code"                        ,  "�� �־�� �� �ڵ尡 �����ϴ�."                                                 },
                { "Repetition of code"                  ,  "�ڵ尡 �ߺ��˴ϴ�."                                                            },
                { "Symbol not found"                    ,  "�ش� �ɺ��� ã�� �� �����ϴ�."                                                 },
                { "Wrong scope"                         ,  "�̰��� ����� �� �����ϴ�."                                                    },
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

        protected:
            void                                            reset();

        protected:
            const bool                                      continueParsing() const noexcept;

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
            const bool                                      findNextDepthMatchingGrouperCloseSymbol(const uint32 openSymbolPosition, uint32* const outCloseSymbolPosition = nullptr) const noexcept;
        
        protected:
            void                                            reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
            void                                            reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);
            const bool                                      hasReportedErrors() const noexcept;

        protected:
            ILexer&                                         _lexer;
            mint::Vector<SymbolTableItem>&                  _symbolTable;

        private:
            uint32                                          _symbolAt;
            mint::Vector<ErrorMessage>                      _errorMessageArray;
        };
    }
}


#include <MintLanguage/Include/IParser.inl>


#endif // !MINT_LANGUAGE_I_PARSER_H