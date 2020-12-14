#include <stdafx.h>
#include <Language/IParser.h>

#include <Language/Lexer.h>
#include <Container/Tree.hpp>


namespace fs
{
	namespace Language
	{
		const SymbolTableItem IParser::kRootSymbol = SymbolTableItem(SymbolClassifier::SPECIAL_USE, "ROOT");
		IParser::IParser(Lexer& lexer)
			: _lexer{ lexer }
			, _symbolTable{ lexer._symbolTable }
			, _symbolAt{ 0 }
		{
			__noop;
		}

		const fs::Tree<SyntaxTreeItem>& IParser::getSyntaxTree() const noexcept
		{
			return _syntaxTree;
		}

		const std::string IParser::getSyntaxTreeString() noexcept
		{
			fs::TreeNodeAccessor<SyntaxTreeItem> rootNode = _syntaxTree.getRootNode();

			std::string result;
			getSyntaxTreeStringInternal(rootNode, 0, result);
			return result;
		}

		void IParser::getSyntaxTreeStringInternal(const TreeNodeAccessor<SyntaxTreeItem>& node, const uint64 depth, std::string& outResult) noexcept
		{
			if (node.isValid() == true)
			{
				const SyntaxTreeItem& syntaxTreeItem = node.getNodeData();
				const SyntaxClassifierEnumType syntaxClassifier = syntaxTreeItem.getSyntaxClassifier();
				const SyntaxAdditionalInfoType additionalInfo = syntaxTreeItem.getAdditionalInfo();
				
				const SymbolTableItem& symbolTableItem = syntaxTreeItem._symbolTableItem;

				for (uint64 i = 0; i < depth; ++i)
				{
					outResult.append(">");
				}

				outResult.append(symbolTableItem._symbolString);

				outResult.append("    At[");
				outResult.append(std::to_string(symbolTableItem._sourceAt));
				outResult.append("] ");

				outResult.append(" SymbolClassifier[");
				outResult.append(std::to_string(static_cast<uint32>(symbolTableItem._symbolClassifier)));
				outResult.append("] ");

				outResult.append(" SyntaxClassifier[");
				outResult.append(std::to_string(syntaxClassifier));
				outResult.append("] ");

				outResult.append(" SyntaxAdditionalInfo[");
				outResult.append(std::to_string(additionalInfo));
				outResult.append("]");

				outResult.append("\n");

				const uint32 childNodeCount = node.getChildNodeCount();
				for (uint32 childNodeIndex = 0; childNodeIndex < childNodeCount; ++childNodeIndex)
				{
					const TreeNodeAccessor<SyntaxTreeItem>& childNode = node.getChildNode(childNodeIndex);
					getSyntaxTreeStringInternal(childNode, depth + 1, outResult);
				}
			}
		}

		void IParser::reset()
		{
			_symbolAt = 0;
			_syntaxTreeCurrentParentNode = _syntaxTree.createRootNode(kRootSymbol);
		}

		const bool IParser::needToContinueParsing() const noexcept
		{
			return _symbolAt < _symbolTable.size();
		}

		void IParser::advanceSymbolPosition(const uint64 advanceCount)
		{
			_symbolAt += max(advanceCount, static_cast<uint64>(1));
		}

		const bool IParser::hasSymbol(const uint64 symbolPosition) const noexcept
		{
			return (symbolPosition < _symbolTable.size());
		}

		const uint64 IParser::getSymbolPosition() const noexcept
		{
			return _symbolAt;
		}

		SymbolTableItem& IParser::getSymbol(const uint64 symbolPosition) const noexcept
		{
			return _symbolTable[symbolPosition];
		}

		const bool IParser::findNextSymbol(const uint64 symbolPosition, const char* const cmp, uint64& outSymbolPosition) const noexcept
		{
			for (uint64 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.size(); ++symbolIter)
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

		const bool IParser::findNextSymbol(const uint64 symbolPosition, const SymbolClassifier symbolClassifier, uint64& outSymbolPosition) const noexcept
		{
			for (uint64 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.size(); ++symbolIter)
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

		const bool IParser::findNextSymbolEither(const uint64 symbolPosition, const char* const cmp0, const char* const cmp1, uint64& outSymbolPosition) const noexcept
		{
			for (uint64 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.size(); ++symbolIter)
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

		const bool IParser::findNextDepthMatchingCloseSymbol(const uint64 symbolPosition, const char* const closeSymbolString, uint64& outSymbolPosition) const noexcept
		{
			const SymbolTableItem& openSymbol = getSymbol(symbolPosition);
			if (openSymbol._symbolClassifier != SymbolClassifier::Grouper_Open)
			{
				FS_ASSERT("김장원", false, "symbolPosition 에 있는 Symbol 은 Grouper_Open 이어야 합니다!!!");
				return false;
			}

			int32 depth = 0;
			for (uint64 symbolIter = symbolPosition + 1; symbolIter < _symbolTable.size(); ++symbolIter)
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
						FS_ASSERT("김장원", symbol._symbolClassifier == SymbolClassifier::Grouper_Close, "Symbol 은 찾았지만 Grouper_Close 가 아닙니다!!!");
						return true;
					}

					--depth;
				}
			}

			return false;
		}

		void IParser::reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType)
		{
			_errorMessageArray.emplace_back(symbolTableItem, errorType);
		}

		void IParser::reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation)
		{
			_errorMessageArray.emplace_back(symbolTableItem, errorType, additionalExplanation);
		}

		const bool IParser::hasReportedErrors() const noexcept
		{
			return !_errorMessageArray.empty();
		}

		fs::TreeNodeAccessor<SyntaxTreeItem> IParser::getSyntaxTreeRootNode() noexcept
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

