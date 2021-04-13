#pragma once


#ifndef FS_TOKENIZER_H
#define FS_TOKENIZER_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsLibrary/Include/ContiguousString.h>


//#define FS_USE_TOKEN_STRING_IN_RELEASE


namespace fs
{
    namespace Language
    {
        class Tokenizer
        {
        public:
                                            Tokenizer(const std::string& source) : _source{ source }, _totalTimeMs{ 0 } { __noop; }
                                            ~Tokenizer() = default;
    
        public:
            void                            insertDelimiter(const char delimiter);
            void                            insertTokenIdentifier(const char tokenIdentifier);
            const uint32                    getDelimiterCount() const noexcept;

        public:
            void                            tokenize();

            const uint32                    getTokenCount() const noexcept;
            const StringRange&              getTokenData(const uint32 tokenIndex) const noexcept;

        public:
            const std::string&              getSource() const noexcept;

        private:
            std::string                     _source;
            uint64                          _totalTimeMs;

        private:
            std::unordered_map<char, int8>  _delimiterUmap;
            std::unordered_map<char, int8>  _tokenIdentifierUmap;
        
        private:
            std::vector<StringRange>        _tokenArray;

#if defined FS_DEBUG || defined FS_USE_TOKEN_STRING_IN_RELEASE
        private:
            std::vector<std::string>        _tokenStringArray;
#endif
        };
    }
}


#endif // !FS_TOKENIZER_H
