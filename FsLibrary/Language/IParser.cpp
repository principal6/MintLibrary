#include <stdafx.h>
#include <Language/IParser.h>

#include <Language/Lexer.h>
#include <Container/Tree.hpp>


namespace fs
{
	namespace Language
	{
		IParser::IParser(Lexer& lexer)
			: _lexer{ lexer }
			, _symbolTable{ lexer._symbolTable }
			, _symbolAt{ 0 }
		{
			_symbolCount = _symbolTable.size();
		}

		void IParser::reset()
		{
			_symbolAt = 0;
			_syntaxTreeCurrentParentNode = _syntaxTree.createRootNode(SyntaxTreeItem());
		}

		const bool IParser::needToContinueParsing() const noexcept
		{
			return _symbolAt < _symbolCount;
		}

		void IParser::advancePosition(const uint64 advanceCount)
		{
			_symbolAt += max(advanceCount, static_cast<uint64>(1));
		}

		void IParser::advancePositionTo(const uint64 nextSymbolPosition)
		{
			_symbolAt = max(_symbolAt, nextSymbolPosition);
		}

		const bool IParser::hasNextSymbols(const uint64 nextSymbolCount) const noexcept
		{
			return (_symbolAt + nextSymbolCount < _symbolCount);
		}

		const bool IParser::hasNextSymbols(const uint64 symbolPosition, const uint64 nextSymbolCount) const noexcept
		{
			return (symbolPosition + nextSymbolCount < _symbolCount);
		}

		const uint64 IParser::getSymbolPosition() const noexcept
		{
			return _symbolAt;
		}

		SymbolTableItem& IParser::getSymbol(const uint64 symbolPosition) const noexcept
		{
			return _symbolTable[symbolPosition];
		}

		SymbolTableItem& IParser::getCurrentSymbol() const noexcept
		{
			return _symbolTable[_symbolAt];
		}

		const bool IParser::findNextSymbol(const uint64 symbolPosition, const char* const cmp, uint64& outSymbolPosition) const noexcept
		{
			for (uint64 symbolIter = symbolPosition + 1; symbolIter < _symbolCount; ++symbolIter)
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

		const bool IParser::findNextSymbolEither(const uint64 symbolPosition, const char* const cmp0, const char* const cmp1, uint64& outSymbolPosition) const noexcept
		{
			for (uint64 symbolIter = symbolPosition + 1; symbolIter < _symbolCount; ++symbolIter)
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

		void IParser::reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType)
		{
			_errorMessageArray.emplace_back(symbolTableItem, errorType);
		}

		void IParser::reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation)
		{
			_errorMessageArray.emplace_back(symbolTableItem, errorType, additionalExplanation);
		}


		IParser::ErrorMessage::ErrorMessage()
			: _sourceAt{ 0 }
		{
			__noop;
		}

		IParser::ErrorMessage::ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType)
			: _sourceAt{ symbolTableItem._sourceAt }
		{
			_message = convertErrorTypeToString(errorType);
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

