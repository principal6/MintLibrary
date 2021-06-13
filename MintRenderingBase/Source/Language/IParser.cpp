#include <stdafx.h>
#include <MintRenderingBase/Include/Language/IParser.h>

#include <MintRenderingBase/Include/Language/ILexer.h>
#include <MintContainer/Include/Tree.hpp>


namespace mint
{
    namespace Language
    {
        const SymbolTableItem IParser::kRootSymbol = SymbolTableItem(SymbolClassifier::SPECIAL_USE, "ROOT");
        IParser::IParser(ILexer& lexer)
            : _lexer{ lexer }
            , _symbolTable{ lexer._symbolTable }
            , _symbolAt{ 0 }
        {
            __noop;
        }

        const mint::Tree<SyntaxTreeItem>& IParser::getSyntaxTree() const noexcept
        {
            return _syntaxTree;
        }

        const std::string IParser::getSyntaxTreeString() noexcept
        {
            mint::TreeNodeAccessor<SyntaxTreeItem> rootNode = _syntaxTree.getRootNode();

            std::string result;
            getSyntaxTreeStringInternal(32, rootNode, 0, result);
            return result;
        }

        void IParser::getSyntaxTreeStringInternal(const uint32 headSpace, const TreeNodeAccessor<SyntaxTreeItem>& node, const uint32 depth, std::string& outResult) noexcept
        {
            if (node.isValid() == true)
            {
                std::string line;

                const SyntaxTreeItem& syntaxTreeItem = node.getNodeData();
                const SyntaxClassifierEnumType syntaxClassifier = syntaxTreeItem.getSyntaxClassifier();
                const SyntaxMainInfoType mainInfo = syntaxTreeItem.getMainInfo();
                const SyntaxSubInfoType subInfo = syntaxTreeItem.getSubInfo();
                
                const SymbolTableItem& symbolTableItem = syntaxTreeItem._symbolTableItem;

                for (uint32 i = 0; i < depth; ++i)
                {
                    line.append(">");
                }

                line.append(symbolTableItem._symbolString);

                const size_t headLength = line.length();
                if (headLength < headSpace)
                {
                    const size_t remnant = headSpace - headLength;
                    for (size_t i = 0; i < remnant; ++i)
                    {
                        line.append(" ");
                    }
                }

                /*
                line.append("SymbolClassifier[");
                line.append(std::to_string(static_cast<uint32>(symbolTableItem._symbolClassifier)));
                line.append("] ");
                */

                if (syntaxClassifier != kUint32Max)
                {
                    line.append(" SyntaxClassifier[");
                    line.append(std::to_string(syntaxClassifier));
                    line.append("] ");
                }
                
                if (mainInfo != 0)
                {
                    line.append(" MainInfo[");
                    line.append(std::to_string(mainInfo));
                    line.append("]");
                }

                if (subInfo != 0)
                {
                    line.append(" SubInfo[");
                    line.append(std::to_string(subInfo));
                    line.append("]");
                }

                if (symbolTableItem._sourceAt != kUint64Max)
                {
                    line.append(" At[#");
                    line.append(std::to_string(symbolTableItem._sourceAt));
                    line.append("] ");
                }

                line.append("\n");
                outResult.append(line);

                const uint32 childNodeCount = node.getChildNodeCount();
                for (uint32 childNodeIndex = 0; childNodeIndex < childNodeCount; ++childNodeIndex)
                {
                    const TreeNodeAccessor<SyntaxTreeItem>& childNode = node.getChildNode(childNodeIndex);
                    getSyntaxTreeStringInternal(headSpace, childNode, depth + 1, outResult);
                }
            }
        }

        void IParser::reset()
        {
            _symbolAt = 0;
            _syntaxTree.createRootNode(kRootSymbol);
        }

        const bool IParser::needToContinueParsing() const noexcept
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

        const bool IParser::findNextDepthMatchingCloseSymbol(const uint32 openSymbolPosition, const char* const closeSymbolString, uint32& outSymbolPosition) const noexcept
        {
            const SymbolTableItem& openSymbol = getSymbol(openSymbolPosition);
            if (openSymbol._symbolClassifier != SymbolClassifier::Grouper_Open)
            {
                MINT_ASSERT("김장원", false, "symbolPosition 에 있는 Symbol 은 Grouper_Open 이어야 합니다!!!");
                return false;
            }

            int32 depth = 0;
            for (uint32 symbolIter = openSymbolPosition + 1; symbolIter < _symbolTable.size(); ++symbolIter)
            {
                const SymbolTableItem& symbol = _symbolTable[symbolIter];
                if (symbol._symbolString == openSymbol._symbolString)
                {
                    ++depth;
                }
                else if (symbol._symbolString == closeSymbolString)
                {
                    if (depth == 0)
                    {
                        outSymbolPosition = symbolIter;
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

        mint::TreeNodeAccessor<SyntaxTreeItem> IParser::getSyntaxTreeRootNode() noexcept
        {
            return _syntaxTree.getRootNode();
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
