#include <MintRenderingBase/Include/CppHlsl/Lexer.h>

#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/HashMap.hpp>


namespace mint
{
	namespace Language
	{
		namespace CppHlsl
		{
			Lexer::Lexer()
				: ILexer()
			{
				SetStatementTerminator(';');
				SetEscaper('\\');

				RegisterDelimiter(' ');
				RegisterDelimiter('\t');
				RegisterDelimiter('\r');
				RegisterDelimiter('\n');

				RegisterLineSkipper("#", LineSkipperSemantic::Preprocessor);
				RegisterLineSkipper("//", LineSkipperSemantic::Comment);
				RegisterLineSkipper("/*", "*/", LineSkipperSemantic::Comment);

				RegisterGrouper('(', ')');
				RegisterGrouper('{', '}');
				RegisterGrouper('[', ']');

				RegisterStringQuote('\'');
				RegisterStringQuote('\"');

				RegisterPunctuator(",");
				RegisterPunctuator("#");
				RegisterPunctuator("::");

				RegisterKeyword("struct");
				RegisterKeyword("using");
				RegisterKeyword("namespace");
				RegisterKeyword("alignas");
			}

			Lexer::Lexer(const StringA& source)
				: Lexer()
			{
				SetSource(source);
			}

			bool Lexer::Execute() noexcept
			{
				return __super::ExecuteDefault();
			}
		}
	}
}
