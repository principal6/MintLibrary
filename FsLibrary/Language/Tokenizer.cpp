#include <stdafx.h>
#include <Language/Tokenizer.h>

#include <Container/Vector.hpp>
#include <Container/DynamicString.hpp>


namespace fs
{
	namespace Language
	{
		void Tokenizer::insertDelimiter(const char delimiter)
		{
			const uint32 delimiterCount = _delimiterArray.size();
			for (uint32 delimiterIndex = 0; delimiterIndex < delimiterCount; ++delimiterIndex)
			{
				if (_delimiterArray.get(delimiterIndex) == delimiter)
				{
					return;
				}
			}

			_delimiterArray.push_back(delimiter);
			_delimiterUmap.insert(std::make_pair(delimiter, 1));
		}

		void Tokenizer::insertTokenIdentifier(const char tokenIdentifier)
		{
			const uint32 tokenIdentifierCount = _tokenIdentifierArray.size();
			for (uint32 tokenIdentifierIndex = 0; tokenIdentifierIndex < tokenIdentifierCount; ++tokenIdentifierIndex)
			{
				if (_tokenIdentifierArray.get(tokenIdentifierIndex) == tokenIdentifier)
				{
					return;
				}
			}

			_tokenIdentifierArray.push_back(tokenIdentifier);
			_tokenIdentifierUmap.insert(std::make_pair(tokenIdentifier, 1));
		}

		const uint32 Tokenizer::getDelimiterCount() const noexcept
		{
			return _delimiterArray.size();
		}

		void Tokenizer::tokenize()
		{
			std::chrono::steady_clock clock;
			const uint64 beginTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch()).count();
			_tokenArray.clear();

			if (_source.empty() == true)
			{
				_totalTimeMs = 0;
				return;
			}

			_tokenArray.reserve(512);
#if defined FS_DEBUG || defined FS_USE_TOKEN_STRING_IN_RELEASE
			_tokenStringArray.reserve(512);
#endif

			uint32 sourcePrevAt = 0;
			const uint32 tokenIdentifierCount = _tokenIdentifierArray.size();
			const uint32 delimiterCount = _delimiterArray.size();
			const uint32 sourceLength = static_cast<uint32>(_source.length());
			for (uint32 sourceAt = 0; sourceAt < sourceLength; ++sourceAt)
			{
#if defined FS_TEST_USE_STD_STRING
				const char sourceChar = _source[sourceAt];
#else
				const char sourceChar = _source.getChar(sourceAt);
#endif

				auto tokenIdentifierFound = _tokenIdentifierUmap.find(sourceChar);
				if (tokenIdentifierFound != _tokenIdentifierUmap.end())
				{
					if (sourcePrevAt < sourceAt)
					{
						_tokenArray.push_back(StringRange(sourcePrevAt, sourceAt - sourcePrevAt));
						_tokenArray.push_back(StringRange(sourceAt, 1));
					}
					else if (sourcePrevAt == sourceAt)
					{
						_tokenArray.push_back(StringRange(sourceAt, 1));
					}

					sourcePrevAt = sourceAt + 1;
					continue;
				}

				auto delimiterFound = _delimiterUmap.find(sourceChar);
				if (delimiterFound != _delimiterUmap.end())
				{
					if (sourcePrevAt < sourceAt)
					{
						_tokenArray.push_back(StringRange(sourcePrevAt, sourceAt - sourcePrevAt));
					}

					sourcePrevAt = sourceAt + 1;
				}
			}

			if (sourcePrevAt < sourceLength)
			{
				_tokenArray.push_back(StringRange(sourcePrevAt, sourceLength - sourcePrevAt));
			}

#if defined FS_DEBUG || defined FS_USE_TOKEN_STRING_IN_RELEASE
			const uint32 tokenCount = _tokenArray.size();
			_tokenStringArray.reserve(tokenCount);
			for (uint32 tokenIndex = 0; tokenIndex < tokenCount; ++tokenIndex)
			{
				const StringRange& token = _tokenArray.get(tokenIndex);
				_tokenStringArray.push_back(_source.substr(token._offset, token._length));
			}
#endif

			const uint64 endTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch()).count();
			_totalTimeMs = endTime - beginTime;
		}

		const uint32 Tokenizer::getTokenCount() const noexcept
		{
			return _tokenArray.size();
		}

		const StringRange& Tokenizer::getToken(const uint32 tokenIndex) const noexcept
		{
			return _tokenArray.get(tokenIndex);
		}
	}
}
