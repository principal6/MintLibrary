#pragma once


#ifndef FS_CPP_PARSER_H
#define FS_CPP_PARSER_H


#include <CommonDefinitions.h>

#include <Language/IParser.h>
#include <Language/LanguageCommon.h>


namespace fs
{
	namespace Language
	{
		class Lexer;

		enum CppSyntaxClassifier : SyntaxClassifierEnumType
		{
			CppSyntaxClassifier_Preprocessor_Include,

			CppSyntaxClassifier_Literal_String,
		};

		class CppParser final : public IParser
		{
		public:
														CppParser(Lexer& lexer);
			virtual										~CppParser();

		public:
			virtual const bool							execute() override final;

		private:
			const bool									parsePreprocessor(const uint64 symbolPosition);
			const bool									parseClass(const uint64 symbolPosition, const bool isStruct);

		private:
			const bool									parseAlignas(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& outAncestorNode);
		};
	}
}


#endif // !FS_CPP_PARSER_H
