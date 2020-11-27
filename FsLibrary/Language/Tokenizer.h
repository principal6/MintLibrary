#pragma once


#ifndef FS_TOKENIZER_H
#define FS_TOKENIZER_H


#include <CommonDefinitions.h>

#include <Container/DynamicString.h>
#include <Container/Vector.h>


//#define FS_TEST_USE_STD_STRING
//#define FS_USE_TOKEN_STRING_IN_RELEASE


namespace fs
{
	namespace Language
	{
		class Tokenizer
		{
		public:
#if defined FS_TEST_USE_STD_STRING
											Tokenizer(const std::string& source) : _source{ source }, _totalTimeMs{ 0 } { __noop; }
#else
											Tokenizer(const fs::DynamicStringA& source) : _source{ source }, _totalTimeMs{ 0 } { __noop; }
#endif
											~Tokenizer() = default;
	
		public:
			void							insertDelimiter(const char delimiter);
			void							insertTokenIdentifier(const char tokenIdentifier);
			const uint32					getDelimiterCount() const noexcept;

		public:
			void							tokenize();

			const uint32					getTokenCount() const noexcept;
			const StringRange&				getTokenData(const uint32 tokenIndex) const noexcept;

		public:
#if defined FS_TEST_USE_STD_STRING
			const std::string				getSource() const noexcept;
#else
			const fs::DynamicStringA		getSource() const noexcept;
#endif

		private:
#if defined FS_TEST_USE_STD_STRING
			std::string						_source;
#else
			fs::DynamicStringA				_source;
#endif

			uint64							_totalTimeMs;

		private:
			std::unordered_map<char, int8>	_delimiterUmap;
			std::unordered_map<char, int8>	_tokenIdentifierUmap;
		
		private:
			fs::Vector<StringRange>			_tokenArray;

#if defined FS_DEBUG || defined FS_USE_TOKEN_STRING_IN_RELEASE
		private:
#if defined FS_TEST_USE_STD_STRING
			fs::Vector<std::string>			_tokenStringArray;
#else
			fs::Vector<fs::DynamicStringA>	_tokenStringArray;
#endif
#endif
		};
	}
}


#endif // !FS_TOKENIZER_H
