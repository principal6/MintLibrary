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

			CppSyntaxClassifier_ClassStruct_Definition,
			CppSyntaxClassifier_ClassStruct_Declaration,
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
			
			CppSyntaxClassifier_Function_Name,
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

		enum CppAdditionalInfo_TypeFlags : SyntaxAdditionalInfoType
		{
			CppAdditionalInfo_TypeFlags_NONE			= 0,
			CppAdditionalInfo_TypeFlags_Const			= 1 <<  0,
			CppAdditionalInfo_TypeFlags_Constexpr		= 1 <<  1,
			CppAdditionalInfo_TypeFlags_Mutable			= 1 <<  2,
			CppAdditionalInfo_TypeFlags_Static			= 1 <<  3,
			CppAdditionalInfo_TypeFlags_ThreadLocal		= 1 <<  4,
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

		/*
		enum class CppTypeClassifier
		{
			BuiltIn_Void,

			BuiltIn_Char,
			BuiltIn_SignedChar_,		// =?= BuiltIn_Char
			BuiltIn_UnsignedChar,
			BuiltIn_Bool,
			BuiltIn_WCharT,
			BuiltIn_SignedShort,		// short, short int, signed short, signed short int
			BuiltIn_UnsignedShort,		// unsigned short, unsigned short int
			BuiltIn_SignedInt,			// int, signed, signed int
			BuiltIn_UnsignedInt,		// unsigned, unsigned int
			BuiltIn_SignedLong,			// long, long int, signed long, signed long int
			BuiltIn_UnsignedLong,		// unsigned long, unsigned long int
			BuiltIn_SignedLongLong,		// long long, long long int, signed long long, signed long long int
			BuiltIn_UnsignedLongLong,	// unsigned long long, unsigned long long int
			BuiltIn_Float,
			BuiltIn_Double,
			BuiltIn_LongDouble_,		// =?= BuiltIn_Double

			// nullptr_t

			UserDefined,

			INVALID
		};
		*/

		class CppTypeTableItem final
		{
		public:
										CppTypeTableItem()	= default;
										CppTypeTableItem(const std::string& typeName);
										~CppTypeTableItem()	= default;

		public:
			const std::string&			getTypeName() const noexcept;

		private:
			std::string					_typeName;
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
			const bool									parseFunctionArguments_Item(const bool isDeclaration, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
			//const bool									parseVariableDeclaration(const bool isDeclaration, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode);
			const bool									parseFunctionInstructions(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
		
		private:
			const bool									parseExpression(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
		
		private:
			const bool									isTypeChunk(const uint64 symbolPosition, uint64& outPostTypeChunkPosition);
			const bool									parseTypeNode(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, TreeNodeAccessor<SyntaxTreeItem>& outTypeNode, uint64& outAdvanceCount);

		private:
			const bool									parseAlignas(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);

		private:
			static const CppSyntaxClassifier			convertSymbolToAccessModifierSyntax(const SymbolTableItem& symbol) noexcept;
			static const CppSyntaxClassifier			convertLiteralSymbolToSyntax(const SymbolTableItem& symbol) noexcept;
			static const SymbolTableItem&				getClassStructAccessModifierSymbol(const CppClassStructAccessModifier cppClassStructAccessModifier) noexcept;
		
		private:
			void										registerUserDefinedType(const CppTypeTableItem& userDefinedType);
			const bool									isSymbolType(const SymbolTableItem& symbol) const noexcept;
			const bool									isBuiltInType(const std::string& symbolString) const noexcept;
			const bool									isUserDefinedType(const std::string& symbolString) const noexcept;
			const CppTypeOf								getTypeOf(const SymbolTableItem& symbol) const noexcept;

		private:
			std::vector<CppTypeTableItem>				_typeTable;
			std::unordered_map<std::string, uint64>		_typeTableUmap;
		
		private:
			std::unordered_map<std::string, int8>		_builtInTypeUmap;
		
		private:
			static const SymbolTableItem				kClassStructAccessModifierSymbolArray[3];
			static const SymbolTableItem				kInitializerListSymbol;
			static const SymbolTableItem				kMemberVariableSymbol;
		};
	}
}


#include <Language/CppParser.inl>


#endif // !FS_CPP_PARSER_H
