#include <stdafx.h>
#include <Language/Lexer.h>

#include <Container/Vector.hpp>
#include <Container/DynamicString.hpp>
#include <Container/StringUtil.hpp>


namespace fs
{
	namespace Language
	{
		uint16 CommentMarkerTableItem::_commentMarkerNextGroupId = 0;

		Lexer::Lexer(const std::string& source)
			: _source{ source }
			, _totalTimeMs{ 0 }
			, _escaper{ '\\' }
			, _statementTerminator{ ';' }
		{
			__noop;
		}

		void Lexer::setEscaper(const char escaper)
		{
			_escaper = escaper;
		}

		void Lexer::setStatementTerminator(const char statementTerminator)
		{
			_statementTerminator = statementTerminator;
		}

		void Lexer::registerCommentMarker(const char* const commentMarker)
		{
			const uint32 length = fs::StringUtil::strlen(commentMarker);
			if (0 == length || 2 < length)
			{
				FS_LOG_ERROR("김장원", "commentMarker 의 길이가 잘못되었습니다!! 현재 길이: %d", length);
				return;
			}

			const uint64 key = (1 == length) ? commentMarker[0] : static_cast<uint64>(commentMarker[1]) * 255 + commentMarker[0];
			if (_commentMarkerUmap.find(key) == _commentMarkerUmap.end())
			{
				_commentMarkerTable.emplace_back(CommentMarkerTableItem(commentMarker, CommentMarkerClassifier::SingleMarker, 0));
				const uint64 commentMarkerIndex = _commentMarkerTable.size() - 1;
				_commentMarkerUmap.insert(std::make_pair(key, commentMarkerIndex));
			}
		}

		void Lexer::registerCommentMarker(const char* const commentMarkerOpen, const char* const commentMarkerClose)
		{
			const uint32 lengthOpen = fs::StringUtil::strlen(commentMarkerOpen);
			const uint32 lengthClose = fs::StringUtil::strlen(commentMarkerClose);
			if ((0 == lengthOpen || 2 < lengthOpen) || (0 == lengthClose || 2 < lengthClose))
			{
				FS_LOG_ERROR("김장원", "commentMarker 의 길이가 잘못되었습니다!! 현재 길이: Open[%d] Close[%d]", lengthOpen, lengthClose);
				return;
			}

			// OpenClose
			if (fs::StringUtil::strcmp(commentMarkerOpen, commentMarkerClose) == true)
			{
				const uint64 keyOpenClose = (1 == lengthOpen) ? commentMarkerOpen[0] : static_cast<uint64>(commentMarkerOpen[1]) * 255 + commentMarkerOpen[0];
				if (_commentMarkerUmap.find(keyOpenClose) == _commentMarkerUmap.end())
				{
					_commentMarkerTable.emplace_back(CommentMarkerTableItem(commentMarkerOpen, CommentMarkerClassifier::OpenCloseMarker, 0));
					const uint64 commentMarkerIndex = _commentMarkerTable.size() - 1;
					_commentMarkerUmap.insert(std::make_pair(keyOpenClose, commentMarkerIndex));
				}
				return;
			}

			// Open & Close
			{
				const uint16 nextGroupId = CommentMarkerTableItem::getNextGroupId();
				const uint64 keyOpen = (1 == lengthOpen) ? commentMarkerOpen[0] : static_cast<uint64>(commentMarkerOpen[1]) * 255 + commentMarkerOpen[0];
				if (_commentMarkerUmap.find(keyOpen) == _commentMarkerUmap.end())
				{
					_commentMarkerTable.emplace_back(CommentMarkerTableItem(commentMarkerOpen, CommentMarkerClassifier::OpenMarker, nextGroupId));
					const uint64 commentMarkerIndex = _commentMarkerTable.size() - 1;
					_commentMarkerUmap.insert(std::make_pair(keyOpen, commentMarkerIndex));
				}

				const uint64 keyClose = (1 == lengthClose) ? commentMarkerClose[0] : static_cast<uint64>(commentMarkerClose[1]) * 255 + commentMarkerClose[0];
				if (_commentMarkerUmap.find(keyClose) == _commentMarkerUmap.end())
				{
					_commentMarkerTable.emplace_back(CommentMarkerTableItem(commentMarkerClose, CommentMarkerClassifier::CloseMarker, nextGroupId));
					const uint64 commentMarkerIndex = _commentMarkerTable.size() - 1;
					_commentMarkerUmap.insert(std::make_pair(keyClose, commentMarkerIndex));
				}
			}
		}

		void Lexer::registerDelimiter(const char delimiter)
		{
			if (_delimiterUmap.find(delimiter) == _delimiterUmap.end())
			{
				_delimiterUmap.insert(std::make_pair(delimiter, 1));
			}
		}

		void Lexer::registerKeyword(const char* const keyword)
		{
			const uint64 hash = fs::StringUtil::hashRawString64(keyword);
			if (_keywordUmap.find(hash) == _keywordUmap.end())
			{
				_keywordTable.emplace_back(keyword);
				const uint64 keywordIndex = _keywordTable.size() - 1;
				_keywordUmap.insert(std::make_pair(hash, keywordIndex));
			}
		}

		void Lexer::registerGrouper(const char grouper, const GrouperClassifier grouperClassifier)
		{
			if (_grouperUmap.find(grouper) == _grouperUmap.end())
			{
				_grouperTable.emplace_back(GrouperTableItem(grouper, grouperClassifier));
				const uint64 grouperIndex = _grouperTable.size() - 1;

				_grouperUmap.insert(std::make_pair(grouper, grouperIndex));
			}
		}

		void Lexer::registerStringQuote(const char stringQuote)
		{
			if (_stringQuoteUmap.find(stringQuote) == _stringQuoteUmap.end())
			{
				_stringQuoteUmap.insert(std::make_pair(stringQuote, 1));
			}
		}

		void Lexer::registerPunctuator(const char punctuator)
		{
			if (_punctuatorUmap.find(punctuator) == _punctuatorUmap.end())
			{
				_punctuatorUmap.insert(std::make_pair(punctuator, 1));
			}
		}
		
		void Lexer::registerOperator(const char* const operator_, const OperatorClassifier operatorClassifier)
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

		const bool Lexer::execute()
		{
			const uint64 sourceLength = _source.length();
			uint64 prevSourceAt = 0;
			uint64 sourceAt = 0;
			while (sourceAt < sourceLength)
			{
				const char ch0 = _source.at(sourceAt);
				const char ch1 = (sourceAt + 1 < sourceLength) ? _source.at(sourceAt + 1) : 0;

				SymbolClassifier symbolClassifier = SymbolClassifier::Identifier;
				uint64 advance = 0;
				OperatorTableItem operatorTableItem;
				GrouperTableItem grouperTableItem;
				CommentMarkerTableItem commentMarkerTableItem;
				if (isCommentMarker(ch0, ch1, commentMarkerTableItem) == true)
				{
					// Comment 는 SymbolTable 에 포함되지 않는다!

					bool isSuccess = false;
					if (commentMarkerTableItem._commentMarkerClassifier == CommentMarkerClassifier::SingleMarker)
					{
						for (uint64 sourceIter = sourceAt + 2; sourceIter < sourceLength; ++sourceIter)
						{
							if (_source.at(sourceIter) == '\n')
							{
								isSuccess = true;
								prevSourceAt = sourceAt = sourceIter + 1;
								break;
							}
						}
					}
					else
					{
						CommentMarkerTableItem closeCommentMarkerTableItem;
						if (commentMarkerTableItem._commentMarkerClassifier == CommentMarkerClassifier::OpenCloseMarker)
						{
							uint64 sourceIter = sourceAt + 2;
							while (sourceIter + 1 < sourceLength)
							{
								if (isCommentMarker(_source.at(sourceIter), _source.at(sourceIter + 1), closeCommentMarkerTableItem) == true)
								{
									if (closeCommentMarkerTableItem._string == commentMarkerTableItem._string)
									{
										isSuccess = true;
										prevSourceAt = sourceAt = sourceIter + 2;
										break;
									}
								}
								++sourceIter;
							}
						}
						else if (commentMarkerTableItem._commentMarkerClassifier == CommentMarkerClassifier::OpenMarker)
						{
							uint64 sourceIter = sourceAt + 2;
							while (sourceIter + 1 < sourceLength)
							{
								if (isCommentMarker(_source.at(sourceIter), _source.at(sourceIter + 1), closeCommentMarkerTableItem) == true)
								{
									if (closeCommentMarkerTableItem._commentMarkerGroupId == commentMarkerTableItem._commentMarkerGroupId)
									{
										isSuccess = true;
										prevSourceAt = sourceAt = sourceIter + 2;
										break;
									}
								}
								++sourceIter;
							}
						}
						else if (commentMarkerTableItem._commentMarkerClassifier == CommentMarkerClassifier::CloseMarker)
						{
							FS_LOG_ERROR("김장원", "Open CommentMarker 가 없는데 Close CommentMarker 가 왔습니다!!!");
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
				else if (isDelimiter(ch0) == true)
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
				else if (isPunctuator(ch0) == true)
				{
					symbolClassifier = SymbolClassifier::Punctuator;
					advance = 1;
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

				if (0 < advance)
				{
					const uint64 tokenLength = sourceAt - prevSourceAt;
					if (symbolClassifier == SymbolClassifier::Delimiter)
					{
						if (tokenLength == 0)
						{
							prevSourceAt = sourceAt + 1;
							++sourceAt;
							continue;
						}
						else if (tokenLength == 1)
						{
							if (isDelimiter(_source.at(prevSourceAt)) == true)
							{
								prevSourceAt = sourceAt + 1;
								++sourceAt;
								continue;
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

						_symbolTable.emplace_back(SymbolTableItem(sourceAt, tokenSymbolClassifier, tokenString));
					}
					
					// Delimiter 제외 자기 자신도 symbol 이다!!!
					if (symbolClassifier != SymbolClassifier::Delimiter)
					{
						char symbolStringRaw[3] = { ch0, (2 == advance) ? ch1 : 0, 0 };
						_symbolTable.emplace_back(SymbolTableItem(sourceAt, symbolClassifier, symbolStringRaw));
					}

					prevSourceAt = sourceAt + 1;
					sourceAt += advance;

					continue;
				}

				++sourceAt;
			}

			const uint64 symbolCount = _symbolTable.size();
			for (uint64 symbolIndex = 0; symbolIndex < symbolCount; ++symbolIndex)
			{
				_symbolTable[symbolIndex]._symbolIndex = symbolIndex;
			}

			return true;
		}

		const bool Lexer::isCommentMarker(const char ch0, const char ch1, CommentMarkerTableItem& out) const noexcept
		{
			// 먼저 길이 2 CommentMarker 인지 확인 후
			// 아니라면 길이 1 CommentMarker 인지 확인
			const uint64 key = static_cast<uint64>(ch1) * 255 + ch0;
			auto found = _commentMarkerUmap.find(key);
			if (found == _commentMarkerUmap.end())
			{
				auto found0 = _commentMarkerUmap.find(ch0);
				if (found0 == _commentMarkerUmap.end())
				{
					return false;
				}

				out = _commentMarkerTable[found0->second];
				return true;
			}

			out = _commentMarkerTable[found->second];
			return true;
		}

		const bool Lexer::isDelimiter(const char input) const noexcept
		{
			return _delimiterUmap.find(input) != _delimiterUmap.end();
		}

		const bool Lexer::isStatementTerminator(const char input) const noexcept
		{
			return (0 == _statementTerminator) ? false : (_statementTerminator == input);
		}

		const bool Lexer::isGrouper(const char input, GrouperTableItem& out) const noexcept
		{
			auto found = _grouperUmap.find(input);
			if (found == _grouperUmap.end())
			{
				return false;
			}

			out = _grouperTable[found->second];
			return true;
		}

		const bool Lexer::isStringQuote(const char input) const noexcept
		{
			return _stringQuoteUmap.find(input) != _stringQuoteUmap.end();
		}

		const bool Lexer::isPunctuator(const char input) const noexcept
		{
			return _punctuatorUmap.find(input) != _punctuatorUmap.end();
		}

		const bool Lexer::isOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept
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

		const bool Lexer::isNumber(const std::string& input) const noexcept
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

		const bool Lexer::isKeyword(const std::string& input) const noexcept
		{
			return _keywordUmap.find(fs::StringUtil::hashRawString64(input.c_str())) != _keywordUmap.end();
		}

		const uint32 Lexer::getSymbolCount() const noexcept
		{
			return static_cast<uint32>(_symbolTable.size());
		}

		const std::vector<SymbolTableItem>& Lexer::getSymbolTable() const noexcept
		{
			return _symbolTable;
		}

		const SymbolTableItem& Lexer::getSymbol(const uint32 symbolIndex) const noexcept
		{
			return _symbolTable[symbolIndex];
		}
	}
}
