#include <stdafx.h>
#include <Language/CppHlslLexer.h>

#include <Container/StringUtil.hpp>


namespace fs
{
	namespace Language
	{
		uint16 LineSkipperTableItem::_lineSkipperNextGroupId = 0;
		CppHlslLexer::CppHlslLexer(const std::string& source)
			: ILexer(source)
		{
			setStatementTerminator(';');
			setEscaper('\\');

			registerDelimiter(' ');
			registerDelimiter('\t');
			registerDelimiter('\r');
			registerDelimiter('\n');

			registerLineSkipper("#"); // For preprocessor
			registerLineSkipper("//");
			registerLineSkipper("/*", "*/");

			registerGrouper('(', fs::Language::GrouperClassifier::Open);
			registerGrouper('{', fs::Language::GrouperClassifier::Open);
			registerGrouper('[', fs::Language::GrouperClassifier::Open);
			registerGrouper(')', fs::Language::GrouperClassifier::Close);
			registerGrouper('}', fs::Language::GrouperClassifier::Close);
			registerGrouper(']', fs::Language::GrouperClassifier::Close);

			registerStringQuote('\'');
			registerStringQuote('\"');

			registerPunctuator(",");
			registerPunctuator("#");
			registerPunctuator("::");
			registerPunctuator("...");

			registerOperator("=", fs::Language::OperatorClassifier::AssignmentOperator);
			registerOperator("+=", fs::Language::OperatorClassifier::AssignmentOperator);
			registerOperator("-=", fs::Language::OperatorClassifier::AssignmentOperator);
			registerOperator("*=", fs::Language::OperatorClassifier::AssignmentOperator);
			registerOperator("/=", fs::Language::OperatorClassifier::AssignmentOperator);
			registerOperator("%=", fs::Language::OperatorClassifier::AssignmentOperator);
			registerOperator("&=", fs::Language::OperatorClassifier::AssignmentOperator);
			registerOperator("|=", fs::Language::OperatorClassifier::AssignmentOperator);
			registerOperator("^=", fs::Language::OperatorClassifier::AssignmentOperator);

			registerOperator("<", fs::Language::OperatorClassifier::RelationalOperator);
			registerOperator("<=", fs::Language::OperatorClassifier::RelationalOperator);
			registerOperator(">", fs::Language::OperatorClassifier::RelationalOperator);
			registerOperator(">=", fs::Language::OperatorClassifier::RelationalOperator);
			registerOperator("==", fs::Language::OperatorClassifier::RelationalOperator);
			registerOperator("!=", fs::Language::OperatorClassifier::RelationalOperator);

			registerOperator("+", fs::Language::OperatorClassifier::ArithmeticOperator);
			registerOperator("-", fs::Language::OperatorClassifier::ArithmeticOperator);
			registerOperator("++", fs::Language::OperatorClassifier::ArithmeticOperator);
			registerOperator("--", fs::Language::OperatorClassifier::ArithmeticOperator);
			registerOperator("*", fs::Language::OperatorClassifier::ArithmeticOperator);
			registerOperator("/", fs::Language::OperatorClassifier::ArithmeticOperator);
			registerOperator("%", fs::Language::OperatorClassifier::ArithmeticOperator);

			registerOperator("&&", fs::Language::OperatorClassifier::LogicalOperator);
			registerOperator("||", fs::Language::OperatorClassifier::LogicalOperator);
			registerOperator("!", fs::Language::OperatorClassifier::LogicalOperator);

			registerOperator("&", fs::Language::OperatorClassifier::BitwiseOperator);
			registerOperator("|", fs::Language::OperatorClassifier::BitwiseOperator);
			registerOperator("^", fs::Language::OperatorClassifier::BitwiseOperator);
			registerOperator("~", fs::Language::OperatorClassifier::BitwiseOperator);

			registerOperator(".", fs::Language::OperatorClassifier::MemberAccessOperator);

			registerOperator("?", fs::Language::OperatorClassifier::OperatorCandiate);
			registerOperator(":", fs::Language::OperatorClassifier::OperatorCandiate);

			registerKeyword("class");
			registerKeyword("struct");
			registerKeyword("public");
			registerKeyword("protected");
			registerKeyword("private");
			registerKeyword("const");
			registerKeyword("constexpr");
			registerKeyword("static");
			registerKeyword("static_assert");
			registerKeyword("inline");
			registerKeyword("__forceinline");
			registerKeyword("__noop");
			registerKeyword("noexcept");
			registerKeyword("virtual");
			registerKeyword("override");
			registerKeyword("final");
			registerKeyword("using");
			registerKeyword("namespace");
			registerKeyword("friend");
			registerKeyword("enum");
			registerKeyword("sizeof");
			registerKeyword("nullptr");
			registerKeyword("signed");
			registerKeyword("unsigned");
			registerKeyword("void");
			registerKeyword("bool");
			registerKeyword("true");
			registerKeyword("false");
			registerKeyword("char");
			registerKeyword("wchar_t");
			registerKeyword("short");
			registerKeyword("int");
			registerKeyword("long");
			registerKeyword("float");
			registerKeyword("double");
			registerKeyword("auto");
			registerKeyword("return");
			registerKeyword("if");
			registerKeyword("else");
			registerKeyword("switch");
			registerKeyword("case");
			registerKeyword("default");
			registerKeyword("delete");
			registerKeyword("abstract");
			registerKeyword("for");
			registerKeyword("continue");
			registerKeyword("break");
			registerKeyword("while");
			registerKeyword("do");
			registerKeyword("static_cast");
			registerKeyword("dynamic_cast");
			registerKeyword("const_cast");
			registerKeyword("reinterpret_cast");
			registerKeyword("template");
			registerKeyword("typename");
			registerKeyword("decltype");
			registerKeyword("alignas");
			registerKeyword("alignof");
		}

		const bool CppHlslLexer::execute()
		{
			const uint64 sourceLength = _source.length();
			
			uint64 prevSourceAt = 0;
			uint64 sourceAt = 0;
			while (continueExecution(sourceAt) == true)
			{
				const char ch0 = getCh0(sourceAt);
				const char ch1 = getCh1(sourceAt);

				LineSkipperTableItem lineSkipperTableItem;
				if (isLineSkipper(ch0, ch1, lineSkipperTableItem) == true)
				{
					// Comment 는 SymbolTable 에 포함되지 않는다!

					bool isSuccess = false;
					if (lineSkipperTableItem._lineSkipperClassifier == LineSkipperClassifier::SingleMarker)
					{
						std::string line;
						for (uint64 sourceIter = sourceAt + 2; sourceIter < sourceLength; ++sourceIter)
						{
							if (_source.at(sourceIter) == '\n')
							{
								line = _source.substr(prevSourceAt, sourceIter - prevSourceAt);
								if (line.back() == '\r')
								{
									line.pop_back();
								}

								isSuccess = true;
								prevSourceAt = sourceAt = sourceIter + 1;
								break;
							}
						}

						if (line.front() == '#')
						{
							// preprocessor

							
						}
					}
					else
					{
						LineSkipperTableItem closeLineSkipperTableItem;
						if (lineSkipperTableItem._lineSkipperClassifier == LineSkipperClassifier::OpenCloseMarker)
						{
							uint64 sourceIter = sourceAt + 2;
							while (sourceIter + 1 < sourceLength)
							{
								if (isLineSkipper(_source.at(sourceIter), _source.at(sourceIter + 1), closeLineSkipperTableItem) == true)
								{
									if (closeLineSkipperTableItem._string == lineSkipperTableItem._string)
									{
										isSuccess = true;
										prevSourceAt = sourceAt = sourceIter + 2;
										break;
									}
								}
								++sourceIter;
							}
						}
						else if (lineSkipperTableItem._lineSkipperClassifier == LineSkipperClassifier::OpenMarker)
						{
							uint64 sourceIter = sourceAt + 2;
							while (sourceIter + 1 < sourceLength)
							{
								if (isLineSkipper(_source.at(sourceIter), _source.at(sourceIter + 1), closeLineSkipperTableItem) == true)
								{
									if (closeLineSkipperTableItem._lineSkipperGroupId == lineSkipperTableItem._lineSkipperGroupId)
									{
										isSuccess = true;
										prevSourceAt = sourceAt = sourceIter + 2;
										break;
									}
								}
								++sourceIter;
							}
						}
						else if (lineSkipperTableItem._lineSkipperClassifier == LineSkipperClassifier::CloseMarker)
						{
							FS_LOG_ERROR("김장원", "Open LineSkipper 가 없는데 Close LineSkipper 가 왔습니다!!!");
							return false;
						}
					}

					if (isSuccess == false)
					{
						return false;
					}
					else
					{
						continue;
					}
				}
				else
				{
					executeDefault(prevSourceAt, sourceAt);
				}
			}

			endExecution();

			return true;
		}
	}
}
