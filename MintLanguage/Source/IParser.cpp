#include <stdafx.h>
#include <MintLanguage/Include/IParser.h>

#include <MintLanguage/Include/ILexer.h>
#include <MintContainer/Include/Tree.hpp>


namespace mint
{
    namespace CppHlsl
    {
        IParser::IParser(ILexer& lexer)
            : _lexer{ lexer }
            , _symbolTable{ lexer._symbolTable }
            , _symbolAt{ 0 }
        {
            __noop;
        }

        void IParser::reset()
        {
            _symbolAt = 0;
        }

        const bool IParser::continueParsing() const noexcept
        {
            return _symbolAt < _symbolTable.size();
        }

        void IParser::advanceSymbolPositionXXX(const uint32 advanceCount)
        {
            _symbolAt += max(advanceCount, static_cast<uint32>(1));
        }

        const bool IParser::hasSymbol(const uint32 symbolPosition) const noexcept
        {
            return (symbolPosition < _symbolTable.size());
        }

        const uint32 IParser::getSymbolPosition() const noexcept
        {
            return _symbolAt;
        }

        SymbolTableItem& IParser::getSymbol(const uint32 symbolPosition) const noexcept
        {
            return _symbolTable[symbolPosition];
        }

        const bool IParser::findNextSymbol(const uint32 symbolPosition, const char* const cmp, uint32& outSymbolPosition) const noexcept
        {
            for (uint32 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.size(); ++symbolIter)
            {
                const SymbolTableItem& symbol = _symbolTable[symbolIter];
                if (symbol._symbolString == cmp)
                {
                    outSymbolPosition = symbolIter;
                    return true;
                }
            }
            return false;
        }

        const bool IParser::findNextSymbol(const uint32 symbolPosition, const SymbolClassifier symbolClassifier, uint32& outSymbolPosition) const noexcept
        {
            for (uint32 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.size(); ++symbolIter)
            {
                const SymbolTableItem& symbol = _symbolTable[symbolIter];
                if (symbol._symbolClassifier == symbolClassifier)
                {
                    outSymbolPosition = symbolIter;
                    return true;
                }
            }
            return false;
        }

        const bool IParser::findNextSymbolEither(const uint32 symbolPosition, const char* const cmp0, const char* const cmp1, uint32& outSymbolPosition) const noexcept
        {
            for (uint32 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.size(); ++symbolIter)
            {
                const SymbolTableItem& symbol = _symbolTable[symbolIter];
                if (symbol._symbolString == cmp0 || symbol._symbolString == cmp1)
                {
                    outSymbolPosition = symbolIter;
                    return true;
                }
            }
            return false;
        }

        const bool IParser::findNextDepthMatchingGrouperCloseSymbol(const uint32 openSymbolPosition, uint32* const outCloseSymbolPosition) const noexcept
        {
            const SymbolTableItem& openSymbol = getSymbol(openSymbolPosition);
            if (openSymbol._symbolClassifier != SymbolClassifier::Grouper_Open)
            {
                MINT_ASSERT("김장원", false, "symbolPosition 에 있는 Symbol 은 Grouper_Open 이어야 합니다!!!");
                return false;
            }
            
            int32 depth = 0;
            const char grouperClose = _lexer.getGrouperClose(openSymbol._symbolString[0]);
            for (uint32 symbolIter = openSymbolPosition + 1; symbolIter < _symbolTable.size(); ++symbolIter)
            {
                const SymbolTableItem& symbol = _symbolTable[symbolIter];
                if (symbol._symbolString == openSymbol._symbolString)
                {
                    ++depth;
                }
                else if (symbol._symbolString[0] == grouperClose)
                {
                    if (depth == 0)
                    {
                        if (outCloseSymbolPosition != nullptr)
                        {
                            *outCloseSymbolPosition = symbolIter;
                        }
                        MINT_ASSERT("김장원", symbol._symbolClassifier == SymbolClassifier::Grouper_Close, "Symbol 은 찾았지만 Grouper_Close 가 아닙니다!!!");
                        return true;
                    }

                    --depth;
                }
            }

            return false;
        }

        void IParser::reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType)
        {
            _errorMessageArray.push_back(ErrorMessage(symbolTableItem, errorType));
        }

        void IParser::reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation)
        {
            _errorMessageArray.push_back(ErrorMessage(symbolTableItem, errorType, additionalExplanation));
        }

        const bool IParser::hasReportedErrors() const noexcept
        {
            return !_errorMessageArray.empty();
        }


        IParser::ErrorMessage::ErrorMessage()
            : _sourceAt{ 0 }
        {
            __noop;
        }

        IParser::ErrorMessage::ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType)
            : _sourceAt{ symbolTableItem._sourceAt }
        {
            _message = "ERROR[";
            _message += convertErrorTypeToTypeString(errorType);
            _message += "] ";
            _message += convertErrorTypeToContentString(errorType);
            _message += " \'";
            _message += symbolTableItem._symbolString;
            _message += "\' #[";
            _message += std::to_string(_sourceAt);
            _message += "]";
        }

        IParser::ErrorMessage::ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation)
            : ErrorMessage(symbolTableItem, errorType)
        {
            _message += ": ";
            _message += additionalExplanation;
        }
    }
}
