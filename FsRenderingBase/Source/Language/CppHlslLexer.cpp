#include <stdafx.h>
#include <FsRenderingBase/Include/Language/CppHlslLexer.h>

#include <FsContainer/Include/StringUtil.hpp>


namespace fs
{
	namespace Language
	{
		uint16 LineSkipperTableItem::_lineSkipperNextGroupId = 0;
		CppHlslLexer::CppHlslLexer()
			: ILexer()
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

		CppHlslLexer::CppHlslLexer(const std::string& source)
			: CppHlslLexer()
		{
			setSource(source);
		}

		const bool CppHlslLexer::execute()
		{
			// Preprocessor
			// line 단위 parsing
			// comment 도 거르기!
			{
				std::string preprocessedSource;

				uint64 prevSourceAt = 0;
				uint64 sourceAt = 0;

				while (continueExecution(sourceAt) == true)
				{
					const char ch0 = getCh0(sourceAt);
					const char ch1 = getCh1(sourceAt);

					LineSkipperTableItem lineSkipperTableItem;
					if (isLineSkipper(ch0, ch1, lineSkipperTableItem) == true)
					{
						bool isSuccess = false;
						if (lineSkipperTableItem._lineSkipperClassifier == LineSkipperClassifier::SingleMarker)
						{
							std::string prev = _source.substr(prevSourceAt, sourceAt - prevSourceAt);

							// Trim
							{
								// Front
								uint64 trimFront = 0;
								while (trimFront < prev.size())
								{
									if (prev[trimFront] == '\r' || prev[trimFront] == '\n')
									{
										++trimFront;
									}
									else
									{
										break;
									}
								}
								prev = prev.substr(trimFront);

								// Back
								while (0 < prev.size())
								{
									if (prev.back() == '\r' || prev.back() == '\n')
									{
										prev.pop_back();
									}
									else
									{
										break;
									}
								}
							}
							
							preprocessedSource.append(prev);

							std::string line;
							for (uint64 sourceIter = sourceAt + 2; continueExecution(sourceIter) == true; ++sourceIter)
							{
								if (_source.at(sourceIter) == '\n')
								{
									line = _source.substr(prevSourceAt, sourceIter - prevSourceAt);
									if (line.back() == '\r')
									{
										line.pop_back();
									}

									isSuccess = true;
									prevSourceAt = sourceAt = sourceIter;
									break;
								}
							}

							if (ch0 == '#')
							{
								// Preprocessor

								//line;
							}
							else
							{
								// Comment
								preprocessedSource.append(_source.substr(prevSourceAt, sourceAt - prevSourceAt));
							}
						}
						else
						{
							LineSkipperTableItem closeLineSkipperTableItem;
							if (lineSkipperTableItem._lineSkipperClassifier == LineSkipperClassifier::OpenCloseMarker)
							{
								uint64 sourceIter = sourceAt + 2;
								while (continueExecution(sourceIter + 1) == true)
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
								while (continueExecution(sourceIter + 1) == true)
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
							FS_LOG_ERROR("김장원", "실패!! lineSkipperClassifier[%d] sourceAt[%d]", (int)lineSkipperTableItem._lineSkipperClassifier, sourceAt);
							return false;
						}
					}

					++sourceAt;
				}

				preprocessedSource.append(_source.substr(prevSourceAt, sourceAt - prevSourceAt));

				std::swap(_source, preprocessedSource);
			}


			uint64 prevSourceAt = 0;
			uint64 sourceAt = 0;
			while (continueExecution(sourceAt) == true)
			{
				executeDefault(prevSourceAt, sourceAt);
			}

			endExecution();

			return true;
		}
	}
}
