#include <stdafx.h>
#include <FsLibrary/Language/ILexer.h>

#include <FsLibrary/ContiguousContainer/ContiguousVector.hpp>
#include <FsLibrary/ContiguousContainer/ContiguousString.hpp>
#include <FsLibrary/Container/StringUtil.hpp>


namespace fs
{
	namespace Language
	{
		ILexer::ILexer()
			: _totalTimeMs{ 0 }
			, _escaper{ '\\' }
			, _statementTerminator{ ';' }
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
			if (_delimiterUmap.find(delimiter) == _delimiterUmap.end())
			{
				_delimiterUmap.insert(std::make_pair(delimiter, 1));
			}
		}

		void ILexer::registerLineSkipper(const char* const lineSkipperOpen, const char* const lineSkipperClose)
		{
			const uint32 lengthOpen = fs::StringUtil::strlen(lineSkipperOpen);
			const uint32 lengthClose = fs::StringUtil::strlen(lineSkipperClose);
			if ((0 == lengthOpen || 2 < lengthOpen) || (0 == lengthClose || 2 < lengthClose))
			{
				FS_LOG_ERROR("김장원", "lineSkipper 의 길이가 잘못되었습니다!! 현재 길이: Open[%d] Close[%d]", lengthOpen, lengthClose);
				return;
			}

			// OpenClose
			if (fs::StringUtil::strcmp(lineSkipperOpen, lineSkipperClose) == true)
			{
				const uint64 keyOpenClose = (1 == lengthOpen) ? lineSkipperOpen[0] : static_cast<uint64>(lineSkipperOpen[1]) * 255 + lineSkipperOpen[0];
				if (_lineSkipperUmap.find(keyOpenClose) == _lineSkipperUmap.end())
				{
					_lineSkipperTable.emplace_back(LineSkipperTableItem(lineSkipperOpen, LineSkipperClassifier::OpenCloseMarker, 0));
					const uint64 lineSkipperIndex = _lineSkipperTable.size() - 1;
					_lineSkipperUmap.insert(std::make_pair(keyOpenClose, lineSkipperIndex));
				}
				return;
			}

			// Open & Close
			{
				const uint16 nextGroupId = LineSkipperTableItem::getNextGroupId();
				const uint64 keyOpen = (1 == lengthOpen) ? lineSkipperOpen[0] : static_cast<uint64>(lineSkipperOpen[1]) * 255 + lineSkipperOpen[0];
				if (_lineSkipperUmap.find(keyOpen) == _lineSkipperUmap.end())
				{
					_lineSkipperTable.emplace_back(LineSkipperTableItem(lineSkipperOpen, LineSkipperClassifier::OpenMarker, nextGroupId));
					const uint64 lineSkipperIndex = _lineSkipperTable.size() - 1;
					_lineSkipperUmap.insert(std::make_pair(keyOpen, lineSkipperIndex));
				}

				const uint64 keyClose = (1 == lengthClose) ? lineSkipperClose[0] : static_cast<uint64>(lineSkipperClose[1]) * 255 + lineSkipperClose[0];
				if (_lineSkipperUmap.find(keyClose) == _lineSkipperUmap.end())
				{
					_lineSkipperTable.emplace_back(LineSkipperTableItem(lineSkipperClose, LineSkipperClassifier::CloseMarker, nextGroupId));
					const uint64 lineSkipperIndex = _lineSkipperTable.size() - 1;
					_lineSkipperUmap.insert(std::make_pair(keyClose, lineSkipperIndex));
				}
			}
		}

		void ILexer::registerLineSkipper(const char* const lineSkipper)
		{
			const uint32 length = fs::StringUtil::strlen(lineSkipper);
			if (0 == length || 2 < length)
			{
				FS_LOG_ERROR("김장원", "lineSkipper 의 길이가 잘못되었습니다!! 현재 길이: %d", length);
				return;
			}

			const uint64 key = (1 == length) ? lineSkipper[0] : static_cast<uint64>(lineSkipper[1]) * 255 + lineSkipper[0];
			if (_lineSkipperUmap.find(key) == _lineSkipperUmap.end())
			{
				_lineSkipperTable.emplace_back(LineSkipperTableItem(lineSkipper, LineSkipperClassifier::SingleMarker, 0));
				const uint64 lineSkipperIndex = _lineSkipperTable.size() - 1;
				_lineSkipperUmap.insert(std::make_pair(key, lineSkipperIndex));
			}
		}

		void ILexer::registerKeyword(const char* const keyword)
		{
			const uint64 hash = fs::StringUtil::hashRawString64(keyword);
			if (_keywordUmap.find(hash) == _keywordUmap.end())
			{
				_keywordTable.emplace_back(keyword);
				const uint64 keywordIndex = _keywordTable.size() - 1;
				_keywordUmap.insert(std::make_pair(hash, keywordIndex));
			}
		}

		void ILexer::registerGrouper(const char grouper, const GrouperClassifier grouperClassifier)
		{
			if (_grouperUmap.find(grouper) == _grouperUmap.end())
			{
				_grouperTable.emplace_back(GrouperTableItem(grouper, grouperClassifier));
				const uint64 grouperIndex = _grouperTable.size() - 1;

				_grouperUmap.insert(std::make_pair(grouper, grouperIndex));
			}
		}

		void ILexer::registerStringQuote(const char stringQuote)
		{
			if (_stringQuoteUmap.find(stringQuote) == _stringQuoteUmap.end())
			{
				_stringQuoteUmap.insert(std::make_pair(stringQuote, 1));
			}
		}

		void ILexer::registerPunctuator(const char* const punctuator)
		{
			const uint32 length = fs::StringUtil::strlen(punctuator);
			if (0 == length || 3 < length)
			{
				FS_LOG_ERROR("김장원", "punctuator 의 길이가 잘못되었습니다!! 현재 길이: %d", length);
				return;
			}

			const uint64 key = fs::StringUtil::hashRawString64(punctuator);
			if (_punctuatorUmap.find(key) == _punctuatorUmap.end())
			{
				_punctuatorTable.emplace_back(punctuator);

				const uint64 punctuatorIndex = _punctuatorTable.size() - 1;
				_punctuatorUmap.insert(std::make_pair(key, punctuatorIndex));
			}
		}
		
		void ILexer::registerOperator(const char* const operator_, const OperatorClassifier operatorClassifier)
		{
			const uint32 length = fs::StringUtil::strlen(operator_);
			if (0 == length || 2 < length)
			{
				FS_LOG_ERROR("김장원", "operator 의 길이가 잘못되었습니다!! 현재 길이: %d", length);
				return;
			}
			if (operatorClassifier == OperatorClassifier::COUNT)
			{
				FS_LOG_ERROR("김장원", "잘못된 OperatorClassifier!!");
				return;
			}

			const uint64 key = (1 == length) ? operator_[0] : static_cast<uint64>(operator_[1]) * 255 + operator_[0];
			if (_operatorUmap.find(key) == _operatorUmap.end())
			{
				_operatorTable.emplace_back(OperatorTableItem(operator_, operatorClassifier));
				const uint64 operatorIndex = _operatorTable.size() - 1;
				_operatorUmap.insert(std::make_pair(key, operatorIndex));
			}
		}

		const bool ILexer::continueExecution(const uint64 sourceAt) const noexcept
		{
			return sourceAt < _source.length();
		}

		const char ILexer::getCh0(const uint64 sourceAt) const noexcept
		{
			return _source.at(sourceAt);
		}

		const char ILexer::getCh1(const uint64 sourceAt) const noexcept
		{
			return (sourceAt + 1 < _source.length()) ? _source.at(sourceAt + 1) : 0;
		}

		const char ILexer::getCh2(const uint64 sourceAt) const noexcept
		{
			return (sourceAt + 2 < _source.length()) ? _source.at(sourceAt + 2) : 0;
		}

		void ILexer::executeDefault(uint64& prevSourceAt, uint64& sourceAt)
		{
			const uint64 sourceLength = _source.length();
			const char ch0 = getCh0(sourceAt);
			const char ch1 = getCh1(sourceAt);
			const char ch2 = getCh2(sourceAt);
			
			uint64 advance = 0;
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
				symbolClassifier = getSymbolClassifierFromGrouperClassifier(grouperTableItem._grouperClassifier);
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
				symbolClassifier = getSymbolClassifierFromOperatorClassifier(operatorTableItem._operatorClassifier);
				advance = operatorTableItem._length;
			}

			advanceExecution(symbolClassifier, advance, prevSourceAt, sourceAt);
		}

		void ILexer::advanceExecution(const SymbolClassifier symbolClassifier, const uint64 advance, uint64& prevSourceAt, uint64& sourceAt)
		{
			if (0 < advance)
			{
				const uint64 sourceLength = _source.length();
				const char ch0 = getCh0(sourceAt);
				const char ch1 = getCh1(sourceAt);
				const char ch2 = getCh2(sourceAt);

				const uint64 tokenLength = sourceAt - prevSourceAt;
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

					_symbolTable.emplace_back(SymbolTableItem(tokenSymbolClassifier, tokenString, sourceAt));
				}

				// Delimiter 제외 자기 자신도 symbol 이다!!!
				if (symbolClassifier != SymbolClassifier::Delimiter)
				{
					char symbolStringRaw[4] = { ch0, (2 == advance) ? ch1 : 0, (3 == advance) ? ch2 : 0, 0 };
					_symbolTable.emplace_back(SymbolTableItem(symbolClassifier, symbolStringRaw, sourceAt));
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
				uint64 symbolIndex = 0;
				const uint64 symbolCount = _symbolTable.size();
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
			const uint64 symbolCount = _symbolTable.size();
			for (uint64 symbolIndex = 0; symbolIndex < symbolCount; ++symbolIndex)
			{
				_symbolTable[symbolIndex]._symbolIndex = symbolIndex;
			}
		}

		const bool ILexer::isDelimiter(const char input) const noexcept
		{
			return _delimiterUmap.find(input) != _delimiterUmap.end();
		}

		const bool ILexer::isLineSkipper(const char ch0, const char ch1, LineSkipperTableItem& out) const noexcept
		{
			// 먼저 길이 2 LineSkipper 인지 확인 후
			// 아니라면 길이 1 LineSkipper 인지 확인
			const uint64 key = static_cast<uint64>(ch1) * 255 + ch0;
			auto found = _lineSkipperUmap.find(key);
			if (found == _lineSkipperUmap.end())
			{
				auto found0 = _lineSkipperUmap.find(ch0);
				if (found0 == _lineSkipperUmap.end())
				{
					return false;
				}

				out = _lineSkipperTable[found0->second];
				return true;
			}

			out = _lineSkipperTable[found->second];
			return true;
		}

		const bool ILexer::isStatementTerminator(const char input) const noexcept
		{
			return (0 == _statementTerminator) ? false : (_statementTerminator == input);
		}

		const bool ILexer::isGrouper(const char input, GrouperTableItem& out) const noexcept
		{
			auto found = _grouperUmap.find(input);
			if (found == _grouperUmap.end())
			{
				return false;
			}

			out = _grouperTable[found->second];
			return true;
		}

		const bool ILexer::isStringQuote(const char input) const noexcept
		{
			return _stringQuoteUmap.find(input) != _stringQuoteUmap.end();
		}

		const bool ILexer::isPunctuator(const char ch0, const char ch1, const char ch2, uint64& outAdvance) const noexcept
		{
			const char keyString3[4]{ ch0, ch1, ch2, '\0' };
			const uint64 key3 = fs::StringUtil::hashRawString64(keyString3);
			auto found3 = _punctuatorUmap.find(key3);
			if (found3 != _punctuatorUmap.end())
			{
				outAdvance = 3;
				return true;
			}

			const char keyString2[3]{ ch0, ch1, '\0' };
			const uint64 key2 = fs::StringUtil::hashRawString64(keyString2);
			auto found2 = _punctuatorUmap.find(key2);
			if (found2 != _punctuatorUmap.end())
			{
				outAdvance = 2;
				return true;
			}

			const char keyString1[2]{ ch0, '\0' };
			const uint64 key1 = fs::StringUtil::hashRawString64(keyString1);
			auto found1 = _punctuatorUmap.find(key1);
			if (found1 != _punctuatorUmap.end())
			{
				outAdvance = 1;
				return true;
			}

			outAdvance = 0;
			return false;
		}

		const bool ILexer::isOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept
		{
			// 먼저 길이 2 Operator 인지 확인 후
			// 아니라면 길이 1 Operator 인지 확인
			const uint64 key = static_cast<uint64>(ch1) * 255 + ch0;
			auto found = _operatorUmap.find(key);
			if (found == _operatorUmap.end())
			{
				auto found0 = _operatorUmap.find(ch0);
				if (found0 == _operatorUmap.end())
				{
					return false;
				}

				out = _operatorTable[found0->second];
				return true;
			}

			out = _operatorTable[found->second];
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
			return _keywordUmap.find(fs::StringUtil::hashRawString64(input.c_str())) != _keywordUmap.end();
		}

		const uint32 ILexer::getSymbolCount() const noexcept
		{
			return static_cast<uint32>(_symbolTable.size());
		}

		const std::vector<SymbolTableItem>& ILexer::getSymbolTable() const noexcept
		{
			return _symbolTable;
		}

		const SymbolTableItem& ILexer::getSymbol(const uint32 symbolIndex) const noexcept
		{
			return _symbolTable[symbolIndex];
		}
	}
}
