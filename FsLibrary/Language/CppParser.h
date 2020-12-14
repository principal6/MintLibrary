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
			CppSyntaxClassifier_Preprocessor,
			CppSyntaxClassifier_Preprocessor_Include,

			CppSyntaxClassifier_Literal_Number,
			CppSyntaxClassifier_Literal_String,
			CppSyntaxClassifier_Literal_TrueFalse,
			CppSyntaxClassifier_Literal_Nullptr,

			CppSyntaxClassifier_ClassStruct_Keyword,
			CppSyntaxClassifier_ClassStruct_Identifier,
			CppSyntaxClassifier_ClassStruct_AccessModifier,
			CppSyntaxClassifier_ClassStruct_Constructor,
			CppSyntaxClassifier_ClassStruct_Constructor_InitializerList,
			CppSyntaxClassifier_ClassStruct_Destructor,
			CppSyntaxClassifier_ClassStruct_MemberVariable,
			CppSyntaxClassifier_ClassStruct_MemberVariableIdentifier,
			CppSyntaxClassifier_Alignas,
			
			CppSyntaxClassifier_Type,
			CppSyntaxClassifier_Type_Specification,
			CppSyntaxClassifier_Type_PointerType,
			CppSyntaxClassifier_Type_ReferenceType,
			CppSyntaxClassifier_Type_RvalueReferenceType,
			CppSyntaxClassifier_Type_Value,
			CppSyntaxClassifier_Type_Alias,
			
			CppSyntaxClassifier_Function_Name,
			CppSyntaxClassifier_Function_Instructions,
			CppSyntaxClassifier_Function_Parameter,
			CppSyntaxClassifier_Function_Return,
			CppSyntaxClassifier_Function_Return_Value,

			CppSyntaxClassifier_INVALID,
		};
		
		enum class CppClassStructAccessModifier : uint8
		{
			Public,
			Protected,
			Private,
		};

		enum class CppTypeOf : uint8
		{
			INVALID,
			BuiltInType,
			LiteralType,
			UserDefinedType,
		};

		enum class CppTypeNodeParsingMethod : uint8
		{
			Expression,
			ClassStructMember,
			FunctionParameter,
		};

		enum class CppUserDefinedTypeInfo : uint8
		{
			Default,
			Abstract,
			Derived,
			DerivedFinal,
		};

		enum CppAdditionalInfo_TypeFlags : SyntaxAdditionalInfoType
		{
			CppAdditionalInfo_TypeFlags_NONE			= 0,

			CppAdditionalInfo_TypeFlags_Const			= 1 <<  0,
			CppAdditionalInfo_TypeFlags_Constexpr		= 1 <<  1,
			CppAdditionalInfo_TypeFlags_Mutable			= 1 <<  2,
			CppAdditionalInfo_TypeFlags_Static			= 1 <<  3,
			CppAdditionalInfo_TypeFlags_ThreadLocal		= 1 <<  4,
			CppAdditionalInfo_TypeFlags_Short			= 1 <<  5,
			CppAdditionalInfo_TypeFlags_Long			= 1 <<  6,
			CppAdditionalInfo_TypeFlags_LongLong		= 1 <<  7,
			CppAdditionalInfo_TypeFlags_Unsigned		= 1 <<  8,
		};

		enum CppAdditionalInfo_FunctionAttributeFlags : SyntaxAdditionalInfoType
		{
			CppAdditionalInfo_FunctionAttributeFlags_NONE		= 0,

			CppAdditionalInfo_FunctionAttributeFlags_Const		= 1 << 0,
			CppAdditionalInfo_FunctionAttributeFlags_Noexcept	= 1 << 1,
			CppAdditionalInfo_FunctionAttributeFlags_Override	= 1 << 2,
			CppAdditionalInfo_FunctionAttributeFlags_Final		= 1 << 3,
			CppAdditionalInfo_FunctionAttributeFlags_Abstract	= 1 << 4,
			CppAdditionalInfo_FunctionAttributeFlags_Default	= 1 << 5,
			CppAdditionalInfo_FunctionAttributeFlags_Delete		= 1 << 6,
		};


		static const CppClassStructAccessModifier		convertStringToCppClassStructAccessModifier(const std::string& input);
		static const std::string&						convertCppClassStructAccessModifierToString(const CppClassStructAccessModifier input);


		class CppTypeTableItem final
		{
		public:
											CppTypeTableItem()	= default;
											CppTypeTableItem(const SymbolTableItem& typeSymbol, const CppAdditionalInfo_TypeFlags& typeFlags);
											CppTypeTableItem(const SymbolTableItem& typeSymbol, const CppUserDefinedTypeInfo& userDefinedTypeInfo);
											CppTypeTableItem(const SyntaxTreeItem& typeSyntax);
											~CppTypeTableItem()	= default;

		public:
			const std::string&				getTypeName() const noexcept;

		private:
			SymbolTableItem					_typeSymbol;
			CppAdditionalInfo_TypeFlags		_typeFlags;
			CppUserDefinedTypeInfo			_userDefinedTypeInfo;
		};


		struct CppTypeModifierSet
		{
		public:
			bool	_isConst		= false;	// const 는 중복 가능!!
			bool	_isConstexpr	= false;	// For non-Parameter
			bool	_isMutable		= false;	// For ClassStruct
			bool	_isStatic		= false;	// For non-Parameter
			bool	_isThreadLocal	= false;	// For Expression
			bool	_isShort		= false;
			uint8	_longState		= 0;		// 0: none, 1: long, 2: long long
			uint8	_signState		= 0;		// 0: default signed, 1: explicit signed, 2: explicit unsgined (signed, unsigned 는 중복 가능하나 교차는 불가!)

		public:
			const CppAdditionalInfo_TypeFlags			getTypeFlags() const noexcept;
		};


		class CppParser final : public IParser
		{
		public:
														CppParser(Lexer& lexer);
			virtual										~CppParser();

		public:
			virtual const bool							execute() override final;

		private:
			const bool									parsePreprocessor(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
		
		private:
			const bool									parseClassStruct(const bool isStruct, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
			const bool									parseClassStructMember(const SymbolTableItem& classIdentifierSymbol, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, CppClassStructAccessModifier& inOutAccessModifier, uint64& outAdvanceCount, bool& outContinueParsing);
			const bool									parseClassStructInitializerList(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
			const bool									parseClassStructInitializerList_Item(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount, bool& outContinueParsing);
		
		private:
			const bool									parseFunctionParameters(const bool isDeclaration, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode);
			const bool									parseFunctionParameters_Item(const bool isDeclaration, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
			const bool									parseFunctionInstructions(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
		
		private:
			const bool									parseExpression(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
		
		private:
			const bool									isTypeChunk(const uint64 symbolPosition, uint64& outPostTypeChunkPosition);
			
			// Identifier 전까지 파싱
			const bool									parseTypeNode(const CppTypeNodeParsingMethod parsingMethod, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, TreeNodeAccessor<SyntaxTreeItem>& outTypeNode, uint64& outAdvanceCount);
			const bool									parseTypeNode_CheckModifiers(const CppTypeNodeParsingMethod parsingMethod, const uint64 symbolPosition, CppTypeModifierSet& outTypeModifierSet, uint64& outAdvanceCount);
		private:
			const bool									parseAlignas(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
			
			// TODO
			const bool									parseUsing(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);

		private:
			static const CppSyntaxClassifier			convertSymbolToAccessModifierSyntax(const SymbolTableItem& symbol) noexcept;
			static const CppSyntaxClassifier			convertLiteralSymbolToSyntax(const SymbolTableItem& symbol) noexcept;
			static const SymbolTableItem&				getClassStructAccessModifierSymbol(const CppClassStructAccessModifier cppClassStructAccessModifier) noexcept;
		
		private:
			const uint64								registerType(const CppTypeTableItem& type);
			const bool									registerTypeAlias(const std::string& typeAlias, const uint64 typeIndex);
			const bool									isSymbolType(const SymbolTableItem& symbol) const noexcept;
			const bool									isBuiltInTypeXXX(const std::string& symbolString) const noexcept;
			const bool									isUserDefinedTypeXXX(const std::string& symbolString) const noexcept;
			const std::string&							getUnaliasedSymbolStringXXX(const SymbolTableItem& symbol) const noexcept;
			const CppTypeOf								getTypeOf(const SymbolTableItem& symbol) const noexcept;

		private:
			std::vector<CppTypeTableItem>				_typeTable;
			std::unordered_map<std::string, uint64>		_typeTableUmap;

			std::unordered_map<std::string, uint64>		_typeAliasTableUmap;
		
		private:
			std::unordered_map<std::string, int8>		_builtInTypeUmap;
		
		private:
			static const SymbolTableItem				kClassStructAccessModifierSymbolArray[3];
			static const SymbolTableItem				kInitializerListSymbol;
			static const SymbolTableItem				kMemberVariableSymbol;
			static const SymbolTableItem				kParameterListSymbol;
			static const SymbolTableItem				kInstructionListSymbol;
			static const SymbolTableItem				kInvalidGrammarSymbol;
			static const SymbolTableItem				kImplicitIntTypeSymbol;
		};
	}
}


#include <Language/CppParser.inl>


#endif // !FS_CPP_PARSER_H
