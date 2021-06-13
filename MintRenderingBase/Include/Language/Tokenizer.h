#pragma once


#ifndef MINT_TOKENIZER_H
#define MINT_TOKENIZER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StringUtil.h>
#include <MintContainer/Include/HashMap.h>


//#define MINT_USE_TOKEN_STRING_IN_RELEASE


namespace mint
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
            const mint::StringRange&          getTokenData(const uint32 tokenIndex) const noexcept;

        public:
            const std::string&              getSource() const noexcept;

        private:
            std::string                     _source;
            uint64                          _totalTimeMs;

        private:
            mint::HashMap<char, int8>         _delimiterUmap;
            mint::HashMap<char, int8>         _tokenIdentifierUmap;
        
        private:
            mint::Vector<mint::StringRange>     _tokenArray;

#if defined MINT_DEBUG || defined MINT_USE_TOKEN_STRING_IN_RELEASE
        private:
            mint::Vector<std::string>         _tokenStringArray;
#endif
        };
    }
}


#endif // !MINT_TOKENIZER_H
