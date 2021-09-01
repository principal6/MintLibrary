#include <stdafx.h>
#include <MintRenderingBase/Include/CppHlsl/Lexer.h>

#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/HashMap.hpp>


namespace mint
{
    namespace CppHlsl
    {
        Lexer::Lexer()
            : ILexer()
        {
            setStatementTerminator(';');
            setEscaper('\\');

            registerDelimiter(' ');
            registerDelimiter('\t');
            registerDelimiter('\r');
            registerDelimiter('\n');

            registerLineSkipper("#", LineSkipperSemantic::Preprocessor);
            registerLineSkipper("//", LineSkipperSemantic::Comment);
            registerLineSkipper("/*", "*/", LineSkipperSemantic::Comment);

            registerGrouper('(', ')');
            registerGrouper('{', '}');
            registerGrouper('[', ']');

            registerStringQuote('\'');
            registerStringQuote('\"');

            registerPunctuator(",");
            registerPunctuator("#");
            registerPunctuator("::");

            registerKeyword("struct");
            registerKeyword("using");
            registerKeyword("namespace");
            registerKeyword("alignas");
        }

        Lexer::Lexer(const std::string& source)
            : Lexer()
        {
            setSource(source);
        }

        const bool Lexer::execute() noexcept
        {
            return __super::executeDefault();
        }
    }
}
