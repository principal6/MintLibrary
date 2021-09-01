#include <stdafx.h>
#include <MintRenderingBase/Include/CppHlsl/Lexer.h>

#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/HashMap.hpp>


namespace mint
{
    namespace CppHlsl
    {
        Lexer::Lexer()
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

            registerGrouper('(', ')');
            registerGrouper('{', '}');
            registerGrouper('[', ']');

            registerStringQuote('\'');
            registerStringQuote('\"');

            registerPunctuator(",");
            registerPunctuator("#");
            registerPunctuator("::");

            registerKeyword("struct");
            registerKeyword("using");
            registerKeyword("namespace");
            registerKeyword("alignas");
        }

        Lexer::Lexer(const std::string& source)
            : Lexer()
        {
            setSource(source);
        }

        const bool Lexer::execute()
        {
            // Preprocessor
            // line 단위 parsing
            // comment 도 거르기!
            {
                std::string preprocessedSource;

                uint32 prevSourceAt = 0;
                uint32 sourceAt = 0;

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
                                uint32 trimFront = 0;
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
                            for (uint32 sourceIter = sourceAt + 2; continueExecution(sourceIter) == true; ++sourceIter)
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
                                uint32 sourceIter = sourceAt + 2;
                                while (continueExecution(sourceIter + 1) == true)
                                {
                                    if (isLineSkipper(_source.at(sourceIter), _source.at(static_cast<uint64>(sourceIter) + 1), closeLineSkipperTableItem) == true)
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
                                uint32 sourceIter = sourceAt + 2;
                                while (continueExecution(sourceIter + 1) == true)
                                {
                                    if (isLineSkipper(_source.at(sourceIter), _source.at(static_cast<uint64>(sourceIter) + 1), closeLineSkipperTableItem) == true)
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
                                MINT_LOG_ERROR("김장원", "Open LineSkipper 가 없는데 Close LineSkipper 가 왔습니다!!!");
                                return false;
                            }
                        }

                        if (isSuccess == false)
                        {
                            MINT_LOG_ERROR("김장원", "실패!! lineSkipperClassifier[%d] sourceAt[%d]", (int)lineSkipperTableItem._lineSkipperClassifier, sourceAt);
                            return false;
                        }
                    }

                    ++sourceAt;
                }

                preprocessedSource.append(_source.substr(prevSourceAt, sourceAt - prevSourceAt));

                std::swap(_source, preprocessedSource);
            }


            uint32 prevSourceAt = 0;
            uint32 sourceAt = 0;
            while (continueExecution(sourceAt) == true)
            {
                executeDefault(prevSourceAt, sourceAt);
            }

            endExecution();

            return true;
        }
    }
}
