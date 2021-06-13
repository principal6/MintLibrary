#pragma once


#ifndef MINT_CPP_LEXER_H
#define MINT_CPP_LEXER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/Language/ILexer.h>


namespace mint
{
    namespace Language
    {
        class CppHlslLexer final : public ILexer
        {
        public:
                                                    CppHlslLexer();
                                                    CppHlslLexer(const std::string& source);
            virtual                                 ~CppHlslLexer() = default;

        public:
            virtual const bool                      execute() override final;
        };
    }
}


#endif // !MINT_CPP_LEXER_H
