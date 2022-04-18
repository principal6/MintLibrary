#pragma once


#ifndef MINT_CPP_LEXER_H
#define MINT_CPP_LEXER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintLanguage/Include/ILexer.h>


namespace mint
{
    namespace Language
    {
        namespace CppHlsl
        {
            class Lexer final : public ILexer
            {
            public:
                Lexer();
                Lexer(const std::string& source);
                virtual                                 ~Lexer() = default;

            public:
                virtual bool                            execute() noexcept override final;
            };
        }
    }
}


#endif // !MINT_CPP_LEXER_H
