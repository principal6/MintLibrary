#include <stdafx.h>
#include <MintLanguage/Include/ILexer.h>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/HashMap.hpp>


namespace mint
{
    namespace Language
    {
        GrouperTableItem::GrouperTableItem()
            : _input{ '\0' }
            , _grouperClassifier{ GrouperClassifier::COUNT }
        {
            __noop;
        }

        GrouperTableItem::GrouperTableItem(const char input, const GrouperClassifier grouperClassifier)
            : _input{ input }
            , _grouperClassifier{ grouperClassifier }
        {
            __noop;
        }


        OperatorTableItem::OperatorTableItem()
            : _length{ 0 }
            , _operatorClassifier{ OperatorClassifier::COUNT }
        {
            __noop;
        }

        OperatorTableItem::OperatorTableItem(const char* const string, const OperatorClassifier operatorClassifier)
            : _string{ string }
            , _operatorClassifier{ operatorClassifier }
        {
            _length = static_cast<uint32>(_string.length());
        }


        LineSkipperTableItem::LineSkipperTableItem()
            : _groupID{ kUint16Max }
            , _lineSkipperSemantic{ LineSkipperSemantic::COUNT }
            , _lineSkipperClassifier{ LineSkipperClassifier::COUNT }
        {
            __noop;
        }

        LineSkipperTableItem::LineSkipperTableItem(const char* const string, const LineSkipperSemantic lineSkipperSemantic, const LineSkipperClassifier lineSkipperClassifier, const uint16 groupID)
            : _string{ string }
            , _groupID{ groupID }
            , _lineSkipperSemantic{ lineSkipperSemantic }
            , _lineSkipperClassifier{ lineSkipperClassifier }
        {
            __noop;
        }


        ILexer::ILexer()
            : _totalTimeMs{ 0 }
            , _escaper{ '\\' }
            , _statementTerminator{ '\0' }
            , _parsePlainEscaper{ false }
            , _defaultSymbolClassifier{ SymbolClassifier::Identifier }
            , _lineSkipperNextGroupID{ 0 }
        {
            __noop;
        }

        void ILexer::setSource(const std::string& source)
        {
            _source = source;
        }

        void ILexer::setEscaper(const char escaper)
        {
            _escaper = escaper;
        }

        void ILexer::setStatementTerminator(const char statementTerminator)
        {
            _statementTerminator = statementTerminator;
        }

        void ILexer::registerDelimiter(const char delimiter)
        {
            if (_delimiterUmap.find(delimiter).isValid() == false)
            {
                _delimiterUmap.insert(delimiter, 1);
            }
        }

        void ILexer::registerLineSkipper(const char* const lineSkipperOpen, const char* const lineSkipperClose, const LineSkipperSemantic lineSkipperSemantic)
        {
            const uint32 lengthOpen = StringUtil::length(lineSkipperOpen);
            const uint32 lengthClose = StringUtil::length(lineSkipperClose);
            if ((lengthOpen == 0 || lengthOpen > 2) || (lengthClose == 0 || lengthClose > 2))
            {
                MINT_LOG_ERROR("lineSkipper 의 길이가 잘못되었습니다!! 현재 길이: Open[%d] Close[%d]", lengthOpen, lengthClose);
                return;
            }

            // OpenClose
            if (StringUtil::compare(lineSkipperOpen, lineSkipperClose) == true)
            {
                const uint64 keyOpenClose = (lengthOpen == 1) ? lineSkipperOpen[0] : static_cast<uint64>(lineSkipperOpen[1]) * 255 + lineSkipperOpen[0];
                if (_lineSkipperUmap.find(keyOpenClose).isValid() == false)
                {
                    _lineSkipperTable.push_back(LineSkipperTableItem(lineSkipperOpen, lineSkipperSemantic, LineSkipperClassifier::OpenCloseMarker, 0));
                    const uint32 lineSkipperIndex = _lineSkipperTable.size() - 1;
                    _lineSkipperUmap.insert(keyOpenClose, lineSkipperIndex);
                }
                return;
            }

            // Open & Close
            {
                const uint16 nextGroupID = getLineSkipperNextGroupID();
                const uint64 keyOpen = (lengthOpen == 1) ? lineSkipperOpen[0] : static_cast<uint64>(lineSkipperOpen[1]) * 255 + lineSkipperOpen[0];
                if (_lineSkipperUmap.find(keyOpen).isValid() == false)
                {
                    _lineSkipperTable.push_back(LineSkipperTableItem(lineSkipperOpen, lineSkipperSemantic, LineSkipperClassifier::OpenMarker, nextGroupID));
                    const uint32 lineSkipperIndex = _lineSkipperTable.size() - 1;
                    _lineSkipperUmap.insert(keyOpen, lineSkipperIndex);
                }

                const uint64 keyClose = (lengthClose == 1) ? lineSkipperClose[0] : static_cast<uint64>(lineSkipperClose[1]) * 255 + lineSkipperClose[0];
                if (_lineSkipperUmap.find(keyClose).isValid() == false)
                {
                    _lineSkipperTable.push_back(LineSkipperTableItem(lineSkipperClose, lineSkipperSemantic, LineSkipperClassifier::CloseMarker, nextGroupID));
                    const uint32 lineSkipperIndex = _lineSkipperTable.size() - 1;
                    _lineSkipperUmap.insert(keyClose, lineSkipperIndex);
                }
            }
        }

        void ILexer::registerLineSkipper(const char* const lineSkipper, const LineSkipperSemantic lineSkipperSemantic)
        {
            const uint32 length = StringUtil::length(lineSkipper);
            if (length == 0 || length > 2)
            {
                MINT_LOG_ERROR("lineSkipper 의 길이가 잘못되었습니다!! 현재 길이: %d", length);
                return;
            }

            const uint64 key = (length == 1) ? lineSkipper[0] : static_cast<uint64>(lineSkipper[1]) * 255 + lineSkipper[0];
            if (_lineSkipperUmap.find(key).isValid() == false)
            {
                _lineSkipperTable.push_back(LineSkipperTableItem(lineSkipper, lineSkipperSemantic, LineSkipperClassifier::SingleMarker, 0));
                const uint32 lineSkipperIndex = _lineSkipperTable.size() - 1;
                _lineSkipperUmap.insert(key, lineSkipperIndex);
            }
        }

        void ILexer::registerKeyword(const char* const keyword)
        {
            const uint64 hash = computeHash(keyword);
            if (_keywordUmap.find(hash).isValid() == false)
            {
                _keywordTable.push_back(keyword);
                const uint32 keywordIndex = _keywordTable.size() - 1;
                _keywordUmap.insert(hash, keywordIndex);
            }
        }

        void ILexer::registerGrouper(const char grouperOpen, const char grouperClose)
        {
            if (_grouperUmap.find(grouperOpen).isValid() == false)
            {
                _grouperTable.push_back(GrouperTableItem(grouperOpen, GrouperClassifier::Open));
                const uint32 grouperOpenIndex = _grouperTable.size() - 1;

                _grouperTable.push_back(GrouperTableItem(grouperClose, GrouperClassifier::Close));
                const uint32 grouperCloseIndex = _grouperTable.size() - 1;

                _grouperUmap.insert(grouperOpen, grouperOpenIndex);
                _grouperUmap.insert(grouperClose, grouperCloseIndex);

                _grouperOpenToCloseMap.insert(grouperOpen, grouperClose);
            }
        }

        void ILexer::registerStringQuote(const char stringQuote)
        {
            if (_stringQuoteUmap.find(stringQuote).isValid() == false)
            {
                _stringQuoteUmap.insert(stringQuote, 1);
            }
        }

        void ILexer::registerPunctuator(const char* const punctuator)
        {
            const uint32 length = StringUtil::length(punctuator);
            if (length == 0 || length > 3)
            {
                MINT_LOG_ERROR("punctuator 의 길이가 잘못되었습니다!! 현재 길이: %d", length);
                return;
            }

            const uint64 key = computeHash(punctuator);
            if (_punctuatorUmap.find(key).isValid() == false)
            {
                _punctuatorTable.push_back(punctuator);

                const uint32 punctuatorIndex = _punctuatorTable.size() - 1;
                _punctuatorUmap.insert(key, punctuatorIndex);
            }
        }
        
        void ILexer::registerOperator(const char* const operator_, const OperatorClassifier operatorClassifier)
        {
            const uint32 length = StringUtil::length(operator_);
            if (length == 0 || length > 2)
            {
                MINT_LOG_ERROR("operator 의 길이가 잘못되었습니다!! 현재 길이: %d", length);
                return;
            }
            if (operatorClassifier == OperatorClassifier::COUNT)
            {
                MINT_LOG_ERROR("잘못된 OperatorClassifier!!");
                return;
            }

            const uint64 key = (length == 1) ? operator_[0] : static_cast<uint64>(operator_[1]) * 255 + operator_[0];
            if (_operatorUmap.find(key).isValid() == false)
            {
                _operatorTable.push_back(OperatorTableItem(operator_, operatorClassifier));
                const uint32 operatorIndex = _operatorTable.size() - 1;
                _operatorUmap.insert(key, operatorIndex);
            }
        }

        uint16 ILexer::getLineSkipperNextGroupID() noexcept
        {
            const uint16 result = _lineSkipperNextGroupID;
            ++_lineSkipperNextGroupID;
            return result;
        }

        bool ILexer::executeDefault() noexcept
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
                        if (lineSkipperTableItem.checkClassifier(LineSkipperClassifier::SingleMarker) == true)
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
                                while (prev.size() > 0)
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

                            if (lineSkipperTableItem.checkSemantic(LineSkipperSemantic::Preprocessor) == true)
                            {
                                // Preprocessor
                                //line;
                            }
                            else if (lineSkipperTableItem.checkSemantic(LineSkipperSemantic::Comment) == true)
                            {
                                // Comment
                                preprocessedSource.append(_source.substr(prevSourceAt, sourceAt - prevSourceAt));
                            }
                            else
                            {
                                MINT_ASSERT(false, "아직 지원되지 않는 LineSkipperSemantic 입니다!!!");
                                return false;
                            }
                        }
                        else
                        {
                            LineSkipperTableItem closeLineSkipperTableItem;
                            if (lineSkipperTableItem.checkClassifier(LineSkipperClassifier::OpenCloseMarker) == true)
                            {
                                uint32 sourceIter = sourceAt + 2;
                                while (continueExecution(sourceIter + 1) == true)
                                {
                                    if (isLineSkipper(_source.at(sourceIter), _source.at(static_cast<uint64>(sourceIter) + 1), closeLineSkipperTableItem) == true)
                                    {
                                        if (closeLineSkipperTableItem.isSameString(lineSkipperTableItem) == true)
                                        {
                                            isSuccess = true;
                                            prevSourceAt = sourceAt = sourceIter + 2;
                                            break;
                                        }
                                    }
                                    ++sourceIter;
                                }
                            }
                            else if (lineSkipperTableItem.checkClassifier(LineSkipperClassifier::OpenMarker) == true)
                            {
                                uint32 sourceIter = sourceAt + 2;
                                while (continueExecution(sourceIter + 1) == true)
                                {
                                    if (isLineSkipper(_source.at(sourceIter), _source.at(static_cast<uint64>(sourceIter) + 1), closeLineSkipperTableItem) == true)
                                    {
                                        if (closeLineSkipperTableItem.isSameGroup(lineSkipperTableItem) == true)
                                        {
                                            isSuccess = true;
                                            prevSourceAt = sourceAt = sourceIter + 2;
                                            break;
                                        }
                                    }
                                    ++sourceIter;
                                }
                            }
                            else if (lineSkipperTableItem.checkClassifier(LineSkipperClassifier::CloseMarker) == true)
                            {
                                MINT_LOG_ERROR("Open LineSkipper 가 없는데 Close LineSkipper 가 왔습니다!!!");
                                return false;
                            }
                        }

                        if (isSuccess == false)
                        {
                            MINT_LOG_ERROR("실패!! lineSkipperClassifier[%d] sourceAt[%d]", static_cast<int32>(lineSkipperTableItem.getClassifier()), sourceAt);
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
                executeInternalScanning(prevSourceAt, sourceAt);
            }

            if (prevSourceAt < sourceAt)
            {
                const uint32 tokenLength = sourceAt - prevSourceAt;
                std::string tokenString = _source.substr(prevSourceAt, tokenLength);
                _symbolTable.push_back(SymbolTableItem(_defaultSymbolClassifier, tokenString, sourceAt));
            }

            endExecution();
            return true;
        }

        bool ILexer::continueExecution(const uint32 sourceAt) const noexcept
        {
            return sourceAt < _source.length();
        }

        char ILexer::getCh0(const uint32 sourceAt) const noexcept
        {
            return _source.at(sourceAt);
        }

        char ILexer::getCh1(const uint32 sourceAt) const noexcept
        {
            return (static_cast<uint64>(sourceAt) + 1 < _source.length()) ? _source.at(static_cast<uint64>(sourceAt) + 1) : 0;
        }

        char ILexer::getCh2(const uint32 sourceAt) const noexcept
        {
            return (static_cast<uint64>(sourceAt) + 2 < _source.length()) ? _source.at(static_cast<uint64>(sourceAt) + 2) : 0;
        }

        void ILexer::executeInternalScanning(uint32& prevSourceAt, uint32& sourceAt)
        {
            const char ch0 = getCh0(sourceAt);
            const char ch1 = getCh1(sourceAt);
            const char ch2 = getCh2(sourceAt);
            
            uint32 advance = 0;
            SymbolClassifier symbolClassifier = _defaultSymbolClassifier;
            OperatorTableItem operatorTableItem;
            GrouperTableItem grouperTableItem;

            if (isDelimiter(ch0) == true)
            {
                symbolClassifier = SymbolClassifier::Delimiter;
                advance = 1;
            }
            else if (isGrouper(ch0, grouperTableItem) == true)
            {
                symbolClassifier = getSymbolClassifierFromGrouperClassifier(grouperTableItem.getClassifier());
                advance = 1;
            }
            else if (isStringQuote(ch0) == true)
            {
                symbolClassifier = SymbolClassifier::StringQuote;
                advance = 1;
            }
            else if (isPunctuator(ch0, ch1, ch2, advance) == true)
            {
                symbolClassifier = SymbolClassifier::Punctuator;
            }
            else if (isStatementTerminator(ch0) == true)
            {
                symbolClassifier = SymbolClassifier::StatementTerminator;
                advance = 1;
            }
            else if (isOperator(ch0, ch1, operatorTableItem) == true)
            {
                symbolClassifier = getSymbolClassifierFromOperatorClassifier(operatorTableItem.getClassifier());
                advance = operatorTableItem.getLength();
            }
            else if (parsePlainEscaper() == true && isEscaper(ch0) == true)
            {
                symbolClassifier = SymbolClassifier::Escaper;
                advance = 1;
            }

            advanceExecution(symbolClassifier, advance, prevSourceAt, sourceAt);
        }

        void ILexer::advanceExecution(const SymbolClassifier symbolClassifier, const uint32 advance, uint32& prevSourceAt, uint32& sourceAt)
        {
            if (advance > 0)
            {
                const char ch0 = getCh0(sourceAt);
                const char ch1 = getCh1(sourceAt);
                const char ch2 = getCh2(sourceAt);

                const uint32 tokenLength = sourceAt - prevSourceAt;
                if (symbolClassifier == SymbolClassifier::Delimiter)
                {
                    if (tokenLength == 0)
                    {
                        prevSourceAt = sourceAt + 1;
                        ++sourceAt;
                        return;
                    }
                    else if (tokenLength == 1)
                    {
                        if (isDelimiter(_source.at(prevSourceAt)) == true)
                        {
                            prevSourceAt = sourceAt + 1;
                            ++sourceAt;
                            return;
                        }
                    }
                }

                if (tokenLength > 0)
                {
                    std::string tokenString = _source.substr(prevSourceAt, tokenLength);
                    SymbolClassifier tokenSymbolClassifier = _defaultSymbolClassifier;
                    if (isNumber(tokenString) == true)
                    {
                        tokenSymbolClassifier = SymbolClassifier::NumberLiteral;
                    }
                    else if (isKeyword(tokenString) == true)
                    {
                        tokenSymbolClassifier = SymbolClassifier::Keyword;
                    }

                    _symbolTable.push_back(SymbolTableItem(tokenSymbolClassifier, tokenString, sourceAt));
                }

                // Delimiter 제외 자기 자신도 symbol 이다!!!
                if (symbolClassifier != SymbolClassifier::Delimiter)
                {
                    char symbolStringRaw[4] = { ch0, (advance == 2) ? ch1 : 0, (advance == 3) ? ch2 : 0, 0 };
                    _symbolTable.push_back(SymbolTableItem(symbolClassifier, symbolStringRaw, sourceAt));
                }

                prevSourceAt = sourceAt + advance;
                sourceAt += advance;

                return;
            }

            ++sourceAt;
        }

        void ILexer::endExecution()
        {
            setStringLiterals();

            setSymbolIndices();
        }

        void ILexer::setStringLiterals()
        {
            // StringQuote 사이에 있는 SymbolClassifier 를 _defaultSymbolClassifier 에서 StringLiteral 로 바꿔준다!!
            const uint32 symbolCount = _symbolTable.size();
            uint32 symbolIndex = 0;
            while (symbolIndex < symbolCount)
            {
                if (_symbolTable[symbolIndex]._symbolClassifier == SymbolClassifier::StringQuote)
                {
                    if (symbolIndex + 2 < symbolCount)
                    {
                        if (_symbolTable[symbolIndex + 2]._symbolClassifier == SymbolClassifier::StringQuote &&
                            _symbolTable[symbolIndex + 1]._symbolClassifier == _defaultSymbolClassifier)
                        {
                            _symbolTable[symbolIndex + 1]._symbolClassifier = SymbolClassifier::StringLiteral;

                            symbolIndex += 3;
                            continue;
                        }
                    }
                }

                ++symbolIndex;
            }
        }

        void ILexer::setSymbolIndices()
        {
            const uint32 symbolCount = _symbolTable.size();
            for (uint32 symbolIndex = 0; symbolIndex < symbolCount; ++symbolIndex)
            {
                _symbolTable[symbolIndex]._symbolIndex = symbolIndex;
            }
        }

        bool ILexer::isDelimiter(const char input) const noexcept
        {
            return _delimiterUmap.find(input).isValid() == true;
        }

        bool ILexer::isLineSkipper(const char ch0, const char ch1, LineSkipperTableItem& out) const noexcept
        {
            // 먼저 길이 2 LineSkipper 인지 확인 후
            // 아니라면 길이 1 LineSkipper 인지 확인
            const uint64 key = static_cast<uint64>(ch1) * 255 + ch0;
            auto found = _lineSkipperUmap.find(key);
            if (found.isValid() == false)
            {
                auto found0 = _lineSkipperUmap.find(ch0);
                if (found0.isValid() == false)
                {
                    return false;
                }

                out = _lineSkipperTable[*found0._value];
                return true;
            }

            out = _lineSkipperTable[*found._value];
            return true;
        }

        bool ILexer::isStatementTerminator(const char input) const noexcept
        {
            return (_statementTerminator == 0) ? false : (_statementTerminator == input);
        }

        bool ILexer::isGrouper(const char input, GrouperTableItem& out) const noexcept
        {
            auto found = _grouperUmap.find(input);
            if (found.isValid() == false)
            {
                return false;
            }

            out = _grouperTable[*found._value];
            return true;
        }

        bool ILexer::isStringQuote(const char input) const noexcept
        {
            return _stringQuoteUmap.find(input).isValid() == true;
        }

        bool ILexer::isPunctuator(const char ch0, const char ch1, const char ch2, uint32& outAdvance) const noexcept
        {
            const char keyString3[4]{ ch0, ch1, ch2, '\0' };
            const uint64 key3 = computeHash(keyString3);
            auto found3 = _punctuatorUmap.find(key3);
            if (found3.isValid() == true)
            {
                outAdvance = 3;
                return true;
            }

            const char keyString2[3]{ ch0, ch1, '\0' };
            const uint64 key2 = computeHash(keyString2);
            auto found2 = _punctuatorUmap.find(key2);
            if (found2.isValid() == true)
            {
                outAdvance = 2;
                return true;
            }

            const char keyString1[2]{ ch0, '\0' };
            const uint64 key1 = computeHash(keyString1);
            auto found1 = _punctuatorUmap.find(key1);
            if (found1.isValid() == true)
            {
                outAdvance = 1;
                return true;
            }

            outAdvance = 0;
            return false;
        }

        bool ILexer::isOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept
        {
            // 먼저 길이 2 Operator 인지 확인 후
            // 아니라면 길이 1 Operator 인지 확인
            const uint64 key = static_cast<uint64>(ch1) * 255 + ch0;
            auto found = _operatorUmap.find(key);
            if (found.isValid() == false)
            {
                auto found0 = _operatorUmap.find(ch0);
                if (found0.isValid() == false)
                {
                    return false;
                }

                out = _operatorTable[*found0._value];
                return true;
            }

            out = _operatorTable[*found._value];
            return true;
        }

        bool ILexer::isNumber(const std::string& input) const noexcept
        {
            if (input.empty() == true)
            {
                return false;
            }

            static constexpr char kNumberZero = '0';
            static constexpr char kNumberNine = '9';
            static constexpr char kPeriod = '.';
            const char firstCh = input.at(0);
            if (firstCh == kPeriod || ((kNumberZero <= firstCh) && (firstCh <= kNumberNine)))
            {
                const uint64 inputLength = input.length();
                for (uint64 inputAt = 1; inputAt < inputLength; ++inputAt)
                {
                    const char ch = input.at(inputAt);
                    if (ch == kPeriod || ((firstCh < kNumberZero) || (kNumberNine < firstCh)))
                    {
                        return false;
                    }
                }

                return true; // number!
            }

            return false;
        }

        bool ILexer::isKeyword(const std::string& input) const noexcept
        {
            return _keywordUmap.find(computeHash(input.c_str())).isValid() == true;
        }

        bool ILexer::isEscaper(const char input) const noexcept
        {
            return _escaper == input;
        }

        uint32 ILexer::getSymbolCount() const noexcept
        {
            return static_cast<uint32>(_symbolTable.size());
        }

        const Vector<SymbolTableItem>& ILexer::getSymbolTable() const noexcept
        {
            return _symbolTable;
        }

        const SymbolTableItem& ILexer::getSymbol(const uint32 symbolIndex) const noexcept
        {
            return _symbolTable[symbolIndex];
        }

        char ILexer::getGrouperClose(const char grouperOpen) const noexcept
        {
            return *_grouperOpenToCloseMap.find(grouperOpen)._value;
        }
    }
}
