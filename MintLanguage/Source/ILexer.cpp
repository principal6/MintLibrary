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
            : _groupId{ kUint16Max }
            , _lineSkipperSemantic{ LineSkipperSemantic::COUNT }
            , _lineSkipperClassifier{ LineSkipperClassifier::COUNT }
        {
            __noop;
        }

        LineSkipperTableItem::LineSkipperTableItem(const char* const string, const LineSkipperSemantic lineSkipperSemantic, const LineSkipperClassifier lineSkipperClassifier, const uint16 groupId)
            : _string{ string }
            , _groupId{ groupId }
            , _lineSkipperSemantic{ lineSkipperSemantic }
            , _lineSkipperClassifier{ lineSkipperClassifier }
        {
            __noop;
        }


        ILexer::ILexer()
            : _totalTimeMs{ 0 }
            , _escaper{ '\\' }
            , _statementTerminator{ '\0' }
            , _lineSkipperNextGroupId{ 0 }
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
            const uint32 lengthOpen = mint::StringUtil::strlen(lineSkipperOpen);
            const uint32 lengthClose = mint::StringUtil::strlen(lineSkipperClose);
            if ((0 == lengthOpen || 2 < lengthOpen) || (0 == lengthClose || 2 < lengthClose))
            {
                MINT_LOG_ERROR("�����", "lineSkipper �� ���̰� �߸��Ǿ����ϴ�!! ���� ����: Open[%d] Close[%d]", lengthOpen, lengthClose);
                return;
            }

            // OpenClose
            if (mint::StringUtil::strcmp(lineSkipperOpen, lineSkipperClose) == true)
            {
                const uint64 keyOpenClose = (1 == lengthOpen) ? lineSkipperOpen[0] : static_cast<uint64>(lineSkipperOpen[1]) * 255 + lineSkipperOpen[0];
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
                const uint16 nextGroupId = getLineSkipperNextGroupId();
                const uint64 keyOpen = (1 == lengthOpen) ? lineSkipperOpen[0] : static_cast<uint64>(lineSkipperOpen[1]) * 255 + lineSkipperOpen[0];
                if (_lineSkipperUmap.find(keyOpen).isValid() == false)
                {
                    _lineSkipperTable.push_back(LineSkipperTableItem(lineSkipperOpen, lineSkipperSemantic, LineSkipperClassifier::OpenMarker, nextGroupId));
                    const uint32 lineSkipperIndex = _lineSkipperTable.size() - 1;
                    _lineSkipperUmap.insert(keyOpen, lineSkipperIndex);
                }

                const uint64 keyClose = (1 == lengthClose) ? lineSkipperClose[0] : static_cast<uint64>(lineSkipperClose[1]) * 255 + lineSkipperClose[0];
                if (_lineSkipperUmap.find(keyClose).isValid() == false)
                {
                    _lineSkipperTable.push_back(LineSkipperTableItem(lineSkipperClose, lineSkipperSemantic, LineSkipperClassifier::CloseMarker, nextGroupId));
                    const uint32 lineSkipperIndex = _lineSkipperTable.size() - 1;
                    _lineSkipperUmap.insert(keyClose, lineSkipperIndex);
                }
            }
        }

        void ILexer::registerLineSkipper(const char* const lineSkipper, const LineSkipperSemantic lineSkipperSemantic)
        {
            const uint32 length = mint::StringUtil::strlen(lineSkipper);
            if (0 == length || 2 < length)
            {
                MINT_LOG_ERROR("�����", "lineSkipper �� ���̰� �߸��Ǿ����ϴ�!! ���� ����: %d", length);
                return;
            }

            const uint64 key = (1 == length) ? lineSkipper[0] : static_cast<uint64>(lineSkipper[1]) * 255 + lineSkipper[0];
            if (_lineSkipperUmap.find(key).isValid() == false)
            {
                _lineSkipperTable.push_back(LineSkipperTableItem(lineSkipper, lineSkipperSemantic, LineSkipperClassifier::SingleMarker, 0));
                const uint32 lineSkipperIndex = _lineSkipperTable.size() - 1;
                _lineSkipperUmap.insert(key, lineSkipperIndex);
            }
        }

        void ILexer::registerKeyword(const char* const keyword)
        {
            const uint64 hash = mint::computeHash(keyword);
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
            const uint32 length = mint::StringUtil::strlen(punctuator);
            if (0 == length || 3 < length)
            {
                MINT_LOG_ERROR("�����", "punctuator �� ���̰� �߸��Ǿ����ϴ�!! ���� ����: %d", length);
                return;
            }

            const uint64 key = mint::computeHash(punctuator);
            if (_punctuatorUmap.find(key).isValid() == false)
            {
                _punctuatorTable.push_back(punctuator);

                const uint32 punctuatorIndex = _punctuatorTable.size() - 1;
                _punctuatorUmap.insert(key, punctuatorIndex);
            }
        }
        
        void ILexer::registerOperator(const char* const operator_, const OperatorClassifier operatorClassifier)
        {
            const uint32 length = mint::StringUtil::strlen(operator_);
            if (0 == length || 2 < length)
            {
                MINT_LOG_ERROR("�����", "operator �� ���̰� �߸��Ǿ����ϴ�!! ���� ����: %d", length);
                return;
            }
            if (operatorClassifier == OperatorClassifier::COUNT)
            {
                MINT_LOG_ERROR("�����", "�߸��� OperatorClassifier!!");
                return;
            }

            const uint64 key = (1 == length) ? operator_[0] : static_cast<uint64>(operator_[1]) * 255 + operator_[0];
            if (_operatorUmap.find(key).isValid() == false)
            {
                _operatorTable.push_back(OperatorTableItem(operator_, operatorClassifier));
                const uint32 operatorIndex = _operatorTable.size() - 1;
                _operatorUmap.insert(key, operatorIndex);
            }
        }

        const uint16 ILexer::getLineSkipperNextGroupId() noexcept
        {
            const uint16 result = _lineSkipperNextGroupId;
            ++_lineSkipperNextGroupId;
            return result;
        }

        const bool ILexer::executeDefault() noexcept
        {
            // Preprocessor
            // line ���� parsing
            // comment �� �Ÿ���!
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
                                MINT_ASSERT("�����", false, "���� �������� �ʴ� LineSkipperSemantic �Դϴ�!!!");
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
                                MINT_LOG_ERROR("�����", "Open LineSkipper �� ���µ� Close LineSkipper �� �Խ��ϴ�!!!");
                                return false;
                            }
                        }

                        if (isSuccess == false)
                        {
                            MINT_LOG_ERROR("�����", "����!! lineSkipperClassifier[%d] sourceAt[%d]", static_cast<int32>(lineSkipperTableItem.getClassifier()), sourceAt);
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

            endExecution();
            return true;
        }

        const bool ILexer::continueExecution(const uint32 sourceAt) const noexcept
        {
            return sourceAt < _source.length();
        }

        const char ILexer::getCh0(const uint32 sourceAt) const noexcept
        {
            return _source.at(sourceAt);
        }

        const char ILexer::getCh1(const uint32 sourceAt) const noexcept
        {
            return (static_cast<uint64>(sourceAt) + 1 < _source.length()) ? _source.at(static_cast<uint64>(sourceAt) + 1) : 0;
        }

        const char ILexer::getCh2(const uint32 sourceAt) const noexcept
        {
            return (static_cast<uint64>(sourceAt) + 2 < _source.length()) ? _source.at(static_cast<uint64>(sourceAt) + 2) : 0;
        }

        void ILexer::executeInternalScanning(uint32& prevSourceAt, uint32& sourceAt)
        {
            const char ch0 = getCh0(sourceAt);
            const char ch1 = getCh1(sourceAt);
            const char ch2 = getCh2(sourceAt);
            
            uint32 advance = 0;
            SymbolClassifier symbolClassifier = SymbolClassifier::Identifier;
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

            advanceExecution(symbolClassifier, advance, prevSourceAt, sourceAt);
        }

        void ILexer::advanceExecution(const SymbolClassifier symbolClassifier, const uint32 advance, uint32& prevSourceAt, uint32& sourceAt)
        {
            if (0 < advance)
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

                if (0 < tokenLength)
                {
                    std::string tokenString = _source.substr(prevSourceAt, tokenLength);
                    SymbolClassifier tokenSymbolClassifier = SymbolClassifier::Identifier;
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

                // Delimiter ���� �ڱ� �ڽŵ� symbol �̴�!!!
                if (symbolClassifier != SymbolClassifier::Delimiter)
                {
                    char symbolStringRaw[4] = { ch0, (2 == advance) ? ch1 : 0, (3 == advance) ? ch2 : 0, 0 };
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
            // String Literal
            {
                uint32 symbolIndex = 0;
                const uint32 symbolCount = _symbolTable.size();
                while (symbolIndex < symbolCount)
                {
                    if (_symbolTable[symbolIndex]._symbolClassifier == SymbolClassifier::StringQuote)
                    {
                        if (symbolIndex + 2 < symbolCount)
                        {
                            if (_symbolTable[symbolIndex + 2]._symbolClassifier == SymbolClassifier::StringQuote &&
                                _symbolTable[symbolIndex + 1]._symbolClassifier == SymbolClassifier::Identifier)
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

            updateSymbolIndex();
        }

        void ILexer::updateSymbolIndex()
        {
            const uint32 symbolCount = _symbolTable.size();
            for (uint32 symbolIndex = 0; symbolIndex < symbolCount; ++symbolIndex)
            {
                _symbolTable[symbolIndex]._symbolIndex = symbolIndex;
            }
        }

        const bool ILexer::isDelimiter(const char input) const noexcept
        {
            return _delimiterUmap.find(input).isValid() == true;
        }

        const bool ILexer::isLineSkipper(const char ch0, const char ch1, LineSkipperTableItem& out) const noexcept
        {
            // ���� ���� 2 LineSkipper ���� Ȯ�� ��
            // �ƴ϶�� ���� 1 LineSkipper ���� Ȯ��
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

        const bool ILexer::isStatementTerminator(const char input) const noexcept
        {
            return (0 == _statementTerminator) ? false : (_statementTerminator == input);
        }

        const bool ILexer::isGrouper(const char input, GrouperTableItem& out) const noexcept
        {
            auto found = _grouperUmap.find(input);
            if (found.isValid() == false)
            {
                return false;
            }

            out = _grouperTable[*found._value];
            return true;
        }

        const bool ILexer::isStringQuote(const char input) const noexcept
        {
            return _stringQuoteUmap.find(input).isValid() == true;
        }

        const bool ILexer::isPunctuator(const char ch0, const char ch1, const char ch2, uint32& outAdvance) const noexcept
        {
            const char keyString3[4]{ ch0, ch1, ch2, '\0' };
            const uint64 key3 = mint::computeHash(keyString3);
            auto found3 = _punctuatorUmap.find(key3);
            if (found3.isValid() == true)
            {
                outAdvance = 3;
                return true;
            }

            const char keyString2[3]{ ch0, ch1, '\0' };
            const uint64 key2 = mint::computeHash(keyString2);
            auto found2 = _punctuatorUmap.find(key2);
            if (found2.isValid() == true)
            {
                outAdvance = 2;
                return true;
            }

            const char keyString1[2]{ ch0, '\0' };
            const uint64 key1 = mint::computeHash(keyString1);
            auto found1 = _punctuatorUmap.find(key1);
            if (found1.isValid() == true)
            {
                outAdvance = 1;
                return true;
            }

            outAdvance = 0;
            return false;
        }

        const bool ILexer::isOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept
        {
            // ���� ���� 2 Operator ���� Ȯ�� ��
            // �ƴ϶�� ���� 1 Operator ���� Ȯ��
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

        const bool ILexer::isNumber(const std::string& input) const noexcept
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

        const bool ILexer::isKeyword(const std::string& input) const noexcept
        {
            return _keywordUmap.find(mint::computeHash(input.c_str())).isValid() == true;
        }

        const uint32 ILexer::getSymbolCount() const noexcept
        {
            return static_cast<uint32>(_symbolTable.size());
        }

        const mint::Vector<SymbolTableItem>& ILexer::getSymbolTable() const noexcept
        {
            return _symbolTable;
        }

        const SymbolTableItem& ILexer::getSymbol(const uint32 symbolIndex) const noexcept
        {
            return _symbolTable[symbolIndex];
        }

        const char ILexer::getGrouperClose(const char grouperOpen) const noexcept
        {
            return *_grouperOpenToCloseMap.find(grouperOpen)._value;
        }
    }
}