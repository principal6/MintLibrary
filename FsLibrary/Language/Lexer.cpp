#include <stdafx.h>
#include <Language/Lexer.h>

#include <Container/Vector.hpp>
#include <Container/DynamicString.hpp>
#include <Container/StringUtil.hpp>


namespace fs
{
	namespace Language
	{
		Lexer::Lexer(const std::string& source)
			: _source{ source }
			, _totalTimeMs{ 0 }
			, _escaper{ '\\' }
			, _stateMarker{ ';' }
		{
			__noop;
		}

		void Lexer::setEscaper(const char escaper)
		{
			_escaper = escaper;
		}

		void Lexer::setStateMarker(const char stateMarker)
		{
			_stateMarker = stateMarker;
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

		void Lexer::registerScoper(const char scoper, const ScoperClassifier scoperClassifier)
		{
			if (_scoperUmap.find(scoper) == _scoperUmap.end())
			{
				_scoperTable.emplace_back(ScoperTableItem(scoper, scoperClassifier));
				const uint64 scoperIndex = _scoperTable.size() - 1;

				_scoperUmap.insert(std::make_pair(scoper, scoperIndex));
			}
		}

		void Lexer::registerStringQuote(const char stringQuote)
		{
			if (_stringQuoteUmap.find(stringQuote) == _stringQuoteUmap.end())
			{
				_stringQuoteUmap.insert(std::make_pair(stringQuote, 1));
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
			if (operatorClassifier == OperatorClassifier::OperatorClassifier_COUNT)
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

		void Lexer::execute()
		{
			const uint64 sourceLength = _source.length();
			uint64 prevSourceAt = 0;
			uint64 sourceAt = 0;
			while (sourceAt < sourceLength)
			{
				const char ch0 = _source.at(sourceAt);
				const char ch1 = (sourceAt + 1 < sourceLength) ? _source.at(sourceAt + 1) : 0;

				SymbolClassifier symbolClassifier = SymbolClassifier::SymbolClassifier_Identifier;
				uint64 advance = 0;
				OperatorTableItem operatorTableItem;
				ScoperTableItem scoperTableItem;
				if (isDelimiter(ch0) == true)
				{
					symbolClassifier = SymbolClassifier::SymbolClassifier_Delimiter;
					advance = 1;
				}
				else if (isScoper(ch0, scoperTableItem) == true)
				{
					symbolClassifier = getSymbolClassifierFromScoperClassifier(scoperTableItem._scoperClassifier);
					advance = 1;
				}
				else if (isStringQuote(ch0) == true)
				{
					symbolClassifier = SymbolClassifier::SymbolClassifier_StringQuote;
					advance = 1;
				}
				else if (isStateMarker(ch0) == true)
				{
					symbolClassifier = SymbolClassifier::SymbolClassifier_StateMarker;
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
					if (symbolClassifier == SymbolClassifier::SymbolClassifier_Delimiter)
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
						const uint64 stringIndex = getStringIndex(tokenString);
						SymbolClassifier tokenSymbolClassifier = SymbolClassifier::SymbolClassifier_Identifier;
						if (isNumber(tokenString) == true)
						{
							tokenSymbolClassifier = SymbolClassifier::SymbolClassifier_NumberLiteral;
						}
						else if (isKeyword(tokenString) == true)
						{
							tokenSymbolClassifier = SymbolClassifier::SymbolClassifier_Keyword;
						}

#if defined FS_DEBUG
						_symbolTable.emplace_back(SymbolTableItem(stringIndex, tokenString, sourceAt, tokenSymbolClassifier));
#else
						_symbolTable.emplace_back(SymbolTableItem(stringIndex, sourceAt, symbolClassifier));
#endif
					}
					
					// Delimiter 제외 자기 자신도 symbol 이다!!!
					if (symbolClassifier != SymbolClassifier::SymbolClassifier_Delimiter)
					{
						const uint64 stringIndex = getStringIndex(ch0, (2 == advance) ? ch1 : 0);
#if defined FS_DEBUG
						char symbolStringRaw[3] = { ch0, (2 == advance) ? ch1 : 0, 0 };
						_symbolTable.emplace_back(SymbolTableItem(stringIndex, symbolStringRaw, sourceAt, symbolClassifier));
#else
						_symbolTable.emplace_back(SymbolTableItem(stringIndex, sourceAt, symbolClassifier));
#endif
					}

					prevSourceAt = sourceAt + 1;
					sourceAt += advance;

					continue;
				}

				++sourceAt;
			}
		}

		const bool Lexer::isDelimiter(const char input) const noexcept
		{
			return _delimiterUmap.find(input) != _delimiterUmap.end();
		}

		const bool Lexer::isStateMarker(const char input) const noexcept
		{
			return (0 == _stateMarker) ? false : (_stateMarker == input);
		}

		const bool Lexer::isScoper(const char input, ScoperTableItem& out) const noexcept
		{
			auto found = _scoperUmap.find(input);
			if (found == _scoperUmap.end())
			{
				return false;
			}

			out = _scoperTable[found->second];
			return true;
		}

		const bool Lexer::isStringQuote(const char input) const noexcept
		{
			return _stringQuoteUmap.find(input) != _stringQuoteUmap.end();
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

		const uint64 Lexer::getStringIndex(const char ch0, const char ch1) noexcept
		{
			const uint64 key = static_cast<uint64>(ch1) * 255 + ch0;
			if (_stringTableUmap.find(key) == _stringTableUmap.end())
			{
				{
					const char inputString[3] = { ch0, ch1, 0 };
					_stringTable.emplace_back(inputString);
				}
				
				const uint64 stringIndex = _stringTable.size() - 1;
				_stringTableUmap.insert(std::make_pair(key, stringIndex));

				return stringIndex;
			}

			return _stringTableUmap.at(ch0);
		}

		const uint64 Lexer::getStringIndex(const std::string& input) noexcept
		{
			const uint64 key = fs::StringUtil::hashRawString64(input.c_str());
			if (_stringTableUmap.find(key) == _stringTableUmap.end())
			{
				_stringTable.emplace_back(input);
				const uint64 stringIndex = _stringTable.size() - 1;
				_stringTableUmap.insert(std::make_pair(key, stringIndex));

				return stringIndex;
			}

			return _stringTableUmap.at(key);
		}
	}
}
