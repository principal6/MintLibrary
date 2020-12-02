#include <stdafx.h>
#include <Language/CppParser.h>

#include <Language/Lexer.h>

#include <Container/StringUtil.hpp>
#include <Container/Tree.hpp>


namespace fs
{
	namespace Language
	{
		CppParser::CppParser(Lexer& lexer)
			: IParser(lexer)
		{
			__noop;
		}

		CppParser::~CppParser()
		{
		}

		const bool CppParser::execute()
		{
			reset();

			while (needToContinueParsing() == true)
			{
				const SymbolTableItem& statementBeginningSymbol = getCurrentSymbol();
				if (statementBeginningSymbol._symbolString == "#")
				{
					FS_RETURN_FALSE_IF_NOT(parsePreprocessor(getSymbolPosition()) == true);
				}
				else if (statementBeginningSymbol._symbolString == "class")
				{
					FS_RETURN_FALSE_IF_NOT(parseClass(getSymbolPosition(), false) == true);
				}
			}

			return true;
		}

		const bool CppParser::parsePreprocessor(const uint64 symbolPosition)
		{
			FS_RETURN_FALSE_IF_NOT(hasNextSymbols(symbolPosition, 1) == true);

			const SymbolTableItem& directive = getSymbol(symbolPosition + 1);
			if (directive._symbolString == "include")
			{
				FS_RETURN_FALSE_IF_NOT(hasNextSymbols(symbolPosition, 3) == true);

				const SymbolTableItem& openSymbol = getSymbol(symbolPosition + 2);
				if ((openSymbol._symbolString == "<" || openSymbol._symbolString == "\""))
				{
					// Find a close symbol
					uint64 closeSymbolPosition = 0;
					if (findNextSymbolEither(symbolPosition, ">", "\"", closeSymbolPosition) == true)
					{
						const SymbolTableItem& closeSymbol = getSymbol(closeSymbolPosition);
						if (openSymbol._symbolString == "<" && closeSymbol._symbolString != ">")
						{
							reportError(closeSymbol, ErrorType::GrouperMismatch);
							return false;
						}
						else if (openSymbol._symbolString == "\"" && closeSymbol._symbolString != "\"")
						{
							reportError(closeSymbol, ErrorType::GrouperMismatch);
							return false;
						}

						SymbolTableItem& fileNameSymbol = getSymbol(symbolPosition + 3);
						const uint64 betweenSymbolCount = closeSymbolPosition - (symbolPosition + 3);
						if (1 < betweenSymbolCount)
						{
							for (uint64 betweenSymbolIndex = 1; betweenSymbolIndex < betweenSymbolCount; ++betweenSymbolIndex)
							{
								SymbolTableItem& currentSymbol = getSymbol(symbolPosition + 3 + betweenSymbolIndex);
								fileNameSymbol._symbolString += currentSymbol._symbolString;
								
								currentSymbol.clearData();
							}
						}

						TreeNodeAccessor directiveNode = _syntaxTreeCurrentParentNode.insertChildNode(SyntaxTreeItem(directive, CppSyntaxClassifier::CppSyntaxClassifier_Preprocessor_Include));
						directiveNode.insertChildNode(SyntaxTreeItem(fileNameSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Literal_String));

						advancePositionTo(closeSymbolPosition + 1);
						return true;
					}
					else
					{
						reportError(openSymbol, ErrorType::NoMatchingGrouper);
						return false;
					}
				}
				else
				{
					reportError(openSymbol, ErrorType::WrongSuccessor, "#include 뒤에는 '<' 나 '\"' 가 와야 합니다.");
					return false;
				}
			}
			else
			{
				// TODO ...
			}

			return false;
		}

		const bool CppParser::parseClass(const uint64 symbolPosition, const bool isStruct)
		{
			// class A;
			// class alignas(4) A;
			// class A {...};
			FS_RETURN_FALSE_IF_NOT(hasNextSymbols(symbolPosition, 2) == true);

			uint64 identifierOffset = 0;
			TreeNodeAccessor<SyntaxTreeItem> alignasAncestorNode;
			{
				const SymbolTableItem& symbol = getSymbol(symbolPosition + 1);
				if (symbol._symbolClassifier != SymbolClassifier::Identifier)
				{
					if (symbol._symbolClassifier != SymbolClassifier::Keyword)
					{
						reportError(symbol, ErrorType::WrongSuccessor, "class 나 struct 뒤에는 identifier 나 keyword 만 올 수 있습니다.");
						return false;
					}

					if (symbol._symbolString == "alignas")
					{
						FS_RETURN_FALSE_IF_NOT(parseAlignas(symbolPosition + 1, alignasAncestorNode) == true);
						identifierOffset = 1;
					}
					else
					{
						// TODO: ?? 뭐가 더 있지
						return false;
					}
				}
			}
			
			{
				const SymbolTableItem& symbol = getSymbol(symbolPosition + identifierOffset + 2);
				if (symbol._symbolClassifier == SymbolClassifier::StatementTerminator)
				{
					// TODO
					// Forward declaration
					return false;

					//return true;
				}
				else if (symbol._symbolClassifier == SymbolClassifier::Grouper_Open)
				{
					// TODO
					// Definition

					return false;

					//return true;
				}
				else
				{
					reportError(symbol, ErrorType::WrongSuccessor, "';' 나 '{' 가 와야 합니다.");
					return false;
				}
			}
			return false;
		}

		const bool CppParser::parseAlignas(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& outAncestorNode)
		{
			// TODO

			FS_RETURN_FALSE_IF_NOT(hasNextSymbols(symbolPosition, 4) == true);
			FS_RETURN_FALSE_IF_NOT(getSymbol(symbolPosition + 2)._symbolString == "(");
			FS_RETURN_FALSE_IF_NOT(getSymbol(symbolPosition + 4)._symbolString == ")");
			return false;
		}

	}
}
