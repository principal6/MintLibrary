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

		void ILexer::SetSource(const std::string& source)
		{
			_source = source;
		}

		void ILexer::SetEscaper(const char escaper)
		{
			_escaper = escaper;
		}

		void ILexer::SetStatementTerminator(const char statementTerminator)
		{
			_statementTerminator = statementTerminator;
		}

		void ILexer::RegisterDelimiter(const char delimiter)
		{
			if (_delimiterUmap.Find(delimiter).IsValid() == false)
			{
				_delimiterUmap.Insert(delimiter, 1);
			}
		}

		void ILexer::RegisterLineSkipper(const char* const lineSkipperOpen, const char* const lineSkipperClose, const LineSkipperSemantic lineSkipperSemantic)
		{
			const uint32 lengthOpen = StringUtil::Length(lineSkipperOpen);
			const uint32 lengthClose = StringUtil::Length(lineSkipperClose);
			if ((lengthOpen == 0 || lengthOpen > 2) || (lengthClose == 0 || lengthClose > 2))
			{
				MINT_LOG_ERROR("lineSkipper 의 길이가 잘못되었습니다!! 현재 길이: Open[%d] Close[%d]", lengthOpen, lengthClose);
				return;
			}

			// OpenClose
			if (StringUtil::Equals(lineSkipperOpen, lineSkipperClose) == true)
			{
				const uint64 keyOpenClose = (lengthOpen == 1) ? lineSkipperOpen[0] : static_cast<uint64>(lineSkipperOpen[1]) * 255 + lineSkipperOpen[0];
				if (_lineSkipperUmap.Find(keyOpenClose).IsValid() == false)
				{
					_lineSkipperTable.PushBack(LineSkipperTableItem(lineSkipperOpen, lineSkipperSemantic, LineSkipperClassifier::OpenCloseMarker, 0));
					const uint32 lineSkipperIndex = _lineSkipperTable.Size() - 1;
					_lineSkipperUmap.Insert(keyOpenClose, lineSkipperIndex);
				}
				return;
			}

			// Open & Close
			{
				const uint16 nextGroupID = GetLineSkipperNextGroupID();
				const uint64 keyOpen = (lengthOpen == 1) ? lineSkipperOpen[0] : static_cast<uint64>(lineSkipperOpen[1]) * 255 + lineSkipperOpen[0];
				if (_lineSkipperUmap.Find(keyOpen).IsValid() == false)
				{
					_lineSkipperTable.PushBack(LineSkipperTableItem(lineSkipperOpen, lineSkipperSemantic, LineSkipperClassifier::OpenMarker, nextGroupID));
					const uint32 lineSkipperIndex = _lineSkipperTable.Size() - 1;
					_lineSkipperUmap.Insert(keyOpen, lineSkipperIndex);
				}

				const uint64 keyClose = (lengthClose == 1) ? lineSkipperClose[0] : static_cast<uint64>(lineSkipperClose[1]) * 255 + lineSkipperClose[0];
				if (_lineSkipperUmap.Find(keyClose).IsValid() == false)
				{
					_lineSkipperTable.PushBack(LineSkipperTableItem(lineSkipperClose, lineSkipperSemantic, LineSkipperClassifier::CloseMarker, nextGroupID));
					const uint32 lineSkipperIndex = _lineSkipperTable.Size() - 1;
					_lineSkipperUmap.Insert(keyClose, lineSkipperIndex);
				}
			}
		}

		void ILexer::RegisterLineSkipper(const char* const lineSkipper, const LineSkipperSemantic lineSkipperSemantic)
		{
			const uint32 length = StringUtil::Length(lineSkipper);
			if (length == 0 || length > 2)
			{
				MINT_LOG_ERROR("lineSkipper 의 길이가 잘못되었습니다!! 현재 길이: %d", length);
				return;
			}

			const uint64 key = (length == 1) ? lineSkipper[0] : static_cast<uint64>(lineSkipper[1]) * 255 + lineSkipper[0];
			if (_lineSkipperUmap.Find(key).IsValid() == false)
			{
				_lineSkipperTable.PushBack(LineSkipperTableItem(lineSkipper, lineSkipperSemantic, LineSkipperClassifier::SingleMarker, 0));
				const uint32 lineSkipperIndex = _lineSkipperTable.Size() - 1;
				_lineSkipperUmap.Insert(key, lineSkipperIndex);
			}
		}

		void ILexer::RegisterKeyword(const char* const keyword)
		{
			const uint64 hash = ComputeHash(keyword);
			if (_keywordUmap.Find(hash).IsValid() == false)
			{
				_keywordTable.PushBack(keyword);
				const uint32 keywordIndex = _keywordTable.Size() - 1;
				_keywordUmap.Insert(hash, keywordIndex);
			}
		}

		void ILexer::RegisterGrouper(const char grouperOpen, const char grouperClose)
		{
			if (_grouperUmap.Find(grouperOpen).IsValid() == false)
			{
				_grouperTable.PushBack(GrouperTableItem(grouperOpen, GrouperClassifier::Open));
				const uint32 grouperOpenIndex = _grouperTable.Size() - 1;

				_grouperTable.PushBack(GrouperTableItem(grouperClose, GrouperClassifier::Close));
				const uint32 grouperCloseIndex = _grouperTable.Size() - 1;

				_grouperUmap.Insert(grouperOpen, grouperOpenIndex);
				_grouperUmap.Insert(grouperClose, grouperCloseIndex);

				_grouperOpenToCloseMap.Insert(grouperOpen, grouperClose);
			}
		}

		void ILexer::RegisterStringQuote(const char stringQuote)
		{
			if (_stringQuoteUmap.Find(stringQuote).IsValid() == false)
			{
				_stringQuoteUmap.Insert(stringQuote, 1);
			}
		}

		void ILexer::RegisterPunctuator(const char* const punctuator)
		{
			const uint32 length = StringUtil::Length(punctuator);
			if (length == 0 || length > 3)
			{
				MINT_LOG_ERROR("punctuator 의 길이가 잘못되었습니다!! 현재 길이: %d", length);
				return;
			}

			const uint64 key = ComputeHash(punctuator);
			if (_punctuatorUmap.Find(key).IsValid() == false)
			{
				_punctuatorTable.PushBack(punctuator);

				const uint32 punctuatorIndex = _punctuatorTable.Size() - 1;
				_punctuatorUmap.Insert(key, punctuatorIndex);
			}
		}

		void ILexer::RegisterOperator(const char* const operator_, const OperatorClassifier operatorClassifier)
		{
			const uint32 length = StringUtil::Length(operator_);
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
			if (_operatorUmap.Find(key).IsValid() == false)
			{
				_operatorTable.PushBack(OperatorTableItem(operator_, operatorClassifier));
				const uint32 operatorIndex = _operatorTable.Size() - 1;
				_operatorUmap.Insert(key, operatorIndex);
			}
		}

		uint16 ILexer::GetLineSkipperNextGroupID() noexcept
		{
			const uint16 result = _lineSkipperNextGroupID;
			++_lineSkipperNextGroupID;
			return result;
		}

		bool ILexer::ExecuteDefault() noexcept
		{
			// Preprocessor
			// line 단위 parsing
			// comment 도 거르기!
			{
				std::string preprocessedSource;

				uint32 prevSourceAt = 0;
				uint32 sourceAt = 0;

				while (ContinuesExecution(sourceAt) == true)
				{
					const char ch0 = GetCh0(sourceAt);
					const char ch1 = GetCh1(sourceAt);

					LineSkipperTableItem lineSkipperTableItem;
					if (IsLineSkipper(ch0, ch1, lineSkipperTableItem) == true)
					{
						bool isSuccess = false;
						if (lineSkipperTableItem.CheckClassifier(LineSkipperClassifier::SingleMarker) == true)
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
							for (uint32 sourceIter = sourceAt + 2; ContinuesExecution(sourceIter) == true; ++sourceIter)
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

							if (lineSkipperTableItem.CheckSemantic(LineSkipperSemantic::Preprocessor) == true)
							{
								// Preprocessor
								//line;
							}
							else if (lineSkipperTableItem.CheckSemantic(LineSkipperSemantic::Comment) == true)
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
							if (lineSkipperTableItem.CheckClassifier(LineSkipperClassifier::OpenCloseMarker) == true)
							{
								uint32 sourceIter = sourceAt + 2;
								while (ContinuesExecution(sourceIter + 1) == true)
								{
									if (IsLineSkipper(_source.at(sourceIter), _source.at(static_cast<uint64>(sourceIter) + 1), closeLineSkipperTableItem) == true)
									{
										if (closeLineSkipperTableItem.IsSameString(lineSkipperTableItem) == true)
										{
											isSuccess = true;
											prevSourceAt = sourceAt = sourceIter + 2;
											break;
										}
									}
									++sourceIter;
								}
							}
							else if (lineSkipperTableItem.CheckClassifier(LineSkipperClassifier::OpenMarker) == true)
							{
								uint32 sourceIter = sourceAt + 2;
								while (ContinuesExecution(sourceIter + 1) == true)
								{
									if (IsLineSkipper(_source.at(sourceIter), _source.at(static_cast<uint64>(sourceIter) + 1), closeLineSkipperTableItem) == true)
									{
										if (closeLineSkipperTableItem.IsSameGroup(lineSkipperTableItem) == true)
										{
											isSuccess = true;
											prevSourceAt = sourceAt = sourceIter + 2;
											break;
										}
									}
									++sourceIter;
								}
							}
							else if (lineSkipperTableItem.CheckClassifier(LineSkipperClassifier::CloseMarker) == true)
							{
								MINT_LOG_ERROR("Open LineSkipper 가 없는데 Close LineSkipper 가 왔습니다!!!");
								return false;
							}
						}

						if (isSuccess == false)
						{
							MINT_LOG_ERROR("실패!! lineSkipperClassifier[%d] sourceAt[%d]", static_cast<int32>(lineSkipperTableItem.GetClassifier()), sourceAt);
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
			while (ContinuesExecution(sourceAt) == true)
			{
				ExecuteInternalScanning(prevSourceAt, sourceAt);
			}

			if (prevSourceAt < sourceAt)
			{
				const uint32 tokenLength = sourceAt - prevSourceAt;
				std::string tokenString = _source.substr(prevSourceAt, tokenLength);
				_symbolTable.PushBack(SymbolTableItem(_defaultSymbolClassifier, tokenString, sourceAt));
			}

			EndExecution();
			return true;
		}

		bool ILexer::ContinuesExecution(const uint32 sourceAt) const noexcept
		{
			return sourceAt < _source.length();
		}

		char ILexer::GetCh0(const uint32 sourceAt) const noexcept
		{
			return _source.at(sourceAt);
		}

		char ILexer::GetCh1(const uint32 sourceAt) const noexcept
		{
			return (static_cast<uint64>(sourceAt) + 1 < _source.length()) ? _source.at(static_cast<uint64>(sourceAt) + 1) : 0;
		}

		char ILexer::GetCh2(const uint32 sourceAt) const noexcept
		{
			return (static_cast<uint64>(sourceAt) + 2 < _source.length()) ? _source.at(static_cast<uint64>(sourceAt) + 2) : 0;
		}

		void ILexer::ExecuteInternalScanning(uint32& prevSourceAt, uint32& sourceAt)
		{
			const char ch0 = GetCh0(sourceAt);
			const char ch1 = GetCh1(sourceAt);
			const char ch2 = GetCh2(sourceAt);

			uint32 advance = 0;
			SymbolClassifier symbolClassifier = _defaultSymbolClassifier;
			OperatorTableItem operatorTableItem;
			GrouperTableItem grouperTableItem;

			if (IsDelimiter(ch0) == true)
			{
				symbolClassifier = SymbolClassifier::Delimiter;
				advance = 1;
			}
			else if (IsGrouper(ch0, grouperTableItem) == true)
			{
				symbolClassifier = GetSymbolClassifierFromGrouperClassifier(grouperTableItem.GetClassifier());
				advance = 1;
			}
			else if (IsStringQuote(ch0) == true)
			{
				symbolClassifier = SymbolClassifier::StringQuote;
				advance = 1;
			}
			else if (IsPunctuator(ch0, ch1, ch2, advance) == true)
			{
				symbolClassifier = SymbolClassifier::Punctuator;
			}
			else if (IsStatementTerminator(ch0) == true)
			{
				symbolClassifier = SymbolClassifier::StatementTerminator;
				advance = 1;
			}
			else if (IsOperator(ch0, ch1, operatorTableItem) == true)
			{
				symbolClassifier = GetSymbolClassifierFromOperatorClassifier(operatorTableItem.GetClassifier());
				advance = operatorTableItem.GetLength();
			}
			else if (ParsePlainEscaper() == true && IsEscaper(ch0) == true)
			{
				symbolClassifier = SymbolClassifier::Escaper;
				advance = 1;
			}

			AdvanceExecution(symbolClassifier, advance, prevSourceAt, sourceAt);
		}

		void ILexer::AdvanceExecution(const SymbolClassifier symbolClassifier, const uint32 advance, uint32& prevSourceAt, uint32& sourceAt)
		{
			if (advance > 0)
			{
				const char ch0 = GetCh0(sourceAt);
				const char ch1 = GetCh1(sourceAt);
				const char ch2 = GetCh2(sourceAt);

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
						if (IsDelimiter(_source.at(prevSourceAt)) == true)
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
					if (IsNumber(tokenString) == true)
					{
						tokenSymbolClassifier = SymbolClassifier::NumberLiteral;
					}
					else if (IsKeyword(tokenString) == true)
					{
						tokenSymbolClassifier = SymbolClassifier::Keyword;
					}

					_symbolTable.PushBack(SymbolTableItem(tokenSymbolClassifier, tokenString, sourceAt));
				}

				// Delimiter 제외 자기 자신도 symbol 이다!!!
				if (symbolClassifier != SymbolClassifier::Delimiter)
				{
					char symbolStringRaw[4] = { ch0, (advance == 2) ? ch1 : 0, (advance == 3) ? ch2 : 0, 0 };
					_symbolTable.PushBack(SymbolTableItem(symbolClassifier, symbolStringRaw, sourceAt));
				}

				prevSourceAt = sourceAt + advance;
				sourceAt += advance;

				return;
			}

			++sourceAt;
		}

		void ILexer::EndExecution()
		{
			SetStringLiterals();

			SetSymbolIndices();
		}

		void ILexer::SetStringLiterals()
		{
			// StringQuote 사이에 있는 SymbolClassifier 를 _defaultSymbolClassifier 에서 StringLiteral 로 바꿔준다!!
			const uint32 symbolCount = _symbolTable.Size();
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

		void ILexer::SetSymbolIndices()
		{
			const uint32 symbolCount = _symbolTable.Size();
			for (uint32 symbolIndex = 0; symbolIndex < symbolCount; ++symbolIndex)
			{
				_symbolTable[symbolIndex]._symbolIndex = symbolIndex;
			}
		}

		bool ILexer::IsDelimiter(const char input) const noexcept
		{
			return _delimiterUmap.Find(input).IsValid() == true;
		}

		bool ILexer::IsLineSkipper(const char ch0, const char ch1, LineSkipperTableItem& out) const noexcept
		{
			// 먼저 길이 2 LineSkipper 인지 확인 후
			// 아니라면 길이 1 LineSkipper 인지 확인
			const uint64 key = static_cast<uint64>(ch1) * 255 + ch0;
			auto found = _lineSkipperUmap.Find(key);
			if (found.IsValid() == false)
			{
				auto found0 = _lineSkipperUmap.Find(ch0);
				if (found0.IsValid() == false)
				{
					return false;
				}

				out = _lineSkipperTable[*found0._value];
				return true;
			}

			out = _lineSkipperTable[*found._value];
			return true;
		}

		bool ILexer::IsStatementTerminator(const char input) const noexcept
		{
			return (_statementTerminator == 0) ? false : (_statementTerminator == input);
		}

		bool ILexer::IsGrouper(const char input, GrouperTableItem& out) const noexcept
		{
			auto found = _grouperUmap.Find(input);
			if (found.IsValid() == false)
			{
				return false;
			}

			out = _grouperTable[*found._value];
			return true;
		}

		bool ILexer::IsStringQuote(const char input) const noexcept
		{
			return _stringQuoteUmap.Find(input).IsValid() == true;
		}

		bool ILexer::IsPunctuator(const char ch0, const char ch1, const char ch2, uint32& outAdvance) const noexcept
		{
			const char keyString3[4]{ ch0, ch1, ch2, '\0' };
			const uint64 key3 = ComputeHash(keyString3);
			auto found3 = _punctuatorUmap.Find(key3);
			if (found3.IsValid() == true)
			{
				outAdvance = 3;
				return true;
			}

			const char keyString2[3]{ ch0, ch1, '\0' };
			const uint64 key2 = ComputeHash(keyString2);
			auto found2 = _punctuatorUmap.Find(key2);
			if (found2.IsValid() == true)
			{
				outAdvance = 2;
				return true;
			}

			const char keyString1[2]{ ch0, '\0' };
			const uint64 key1 = ComputeHash(keyString1);
			auto found1 = _punctuatorUmap.Find(key1);
			if (found1.IsValid() == true)
			{
				outAdvance = 1;
				return true;
			}

			outAdvance = 0;
			return false;
		}

		bool ILexer::IsOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept
		{
			// 먼저 길이 2 Operator 인지 확인 후
			// 아니라면 길이 1 Operator 인지 확인
			const uint64 key = static_cast<uint64>(ch1) * 255 + ch0;
			auto found = _operatorUmap.Find(key);
			if (found.IsValid() == false)
			{
				auto found0 = _operatorUmap.Find(ch0);
				if (found0.IsValid() == false)
				{
					return false;
				}

				out = _operatorTable[*found0._value];
				return true;
			}

			out = _operatorTable[*found._value];
			return true;
		}

		bool ILexer::IsNumber(const std::string& input) const noexcept
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

		bool ILexer::IsKeyword(const std::string& input) const noexcept
		{
			return _keywordUmap.Find(ComputeHash(input.c_str())).IsValid() == true;
		}

		bool ILexer::IsEscaper(const char input) const noexcept
		{
			return _escaper == input;
		}

		uint32 ILexer::GetSymbolCount() const noexcept
		{
			return static_cast<uint32>(_symbolTable.Size());
		}

		const Vector<SymbolTableItem>& ILexer::GetSymbolTable() const noexcept
		{
			return _symbolTable;
		}

		const SymbolTableItem& ILexer::GetSymbol(const uint32 symbolIndex) const noexcept
		{
			return _symbolTable[symbolIndex];
		}

		char ILexer::GetGrouperClose(const char grouperOpen) const noexcept
		{
			return *_grouperOpenToCloseMap.Find(grouperOpen)._value;
		}
	}
}
