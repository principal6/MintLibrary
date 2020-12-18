#include <stdafx.h>
#include <Language/Tokenizer.h>


namespace fs
{
	namespace Language
	{
		void Tokenizer::insertDelimiter(const char delimiter)
		{
			_delimiterUmap.insert(std::make_pair(delimiter, 1));
		}

		void Tokenizer::insertTokenIdentifier(const char tokenIdentifier)
		{
			_tokenIdentifierUmap.insert(std::make_pair(tokenIdentifier, 1));
		}

		const uint32 Tokenizer::getDelimiterCount() const noexcept
		{
			return static_cast<uint32>(_delimiterUmap.size());
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
			const uint32 sourceLength = static_cast<uint32>(_source.length());
			for (uint32 sourceAt = 0; sourceAt < sourceLength; ++sourceAt)
			{
				const char sourceChar = _source[sourceAt];

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
			const uint32 tokenCount = static_cast<uint32>(_tokenArray.size());
			_tokenStringArray.reserve(tokenCount);
			for (uint32 tokenIndex = 0; tokenIndex < tokenCount; ++tokenIndex)
			{
				const StringRange& token = _tokenArray[tokenIndex];
				_tokenStringArray.push_back(_source.substr(token._offset, token._length));
			}
#endif

			const uint64 endTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch()).count();
			_totalTimeMs = endTime - beginTime;
		}

		const uint32 Tokenizer::getTokenCount() const noexcept
		{
			return static_cast<uint32>(_tokenArray.size());
		}

		const StringRange& Tokenizer::getTokenData(const uint32 tokenIndex) const noexcept
		{
			return _tokenArray[tokenIndex];
		}

		const std::string& Tokenizer::getSource() const noexcept
		{
			return _source;
		}
	}
}
