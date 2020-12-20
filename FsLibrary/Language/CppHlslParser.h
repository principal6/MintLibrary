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
		class ILexer;


		enum CppHlslSyntaxClassifier : SyntaxClassifierEnumType
		{
			CppHlslSyntaxClassifier_Preprocessor_Include,

			CppHlslSyntaxClassifier_Literal_Number,
			CppHlslSyntaxClassifier_Literal_String,
			CppHlslSyntaxClassifier_Literal_TrueFalse,
			CppHlslSyntaxClassifier_Literal_Nullptr,

			CppHlslSyntaxClassifier_ClassStruct_Keyword,
			CppHlslSyntaxClassifier_ClassStruct_Identifier,
			CppHlslSyntaxClassifier_ClassStruct_AccessModifier,
			CppHlslSyntaxClassifier_ClassStruct_Constructor,
			CppHlslSyntaxClassifier_ClassStruct_Constructor_InitializerList,
			CppHlslSyntaxClassifier_ClassStruct_Destructor,
			CppHlslSyntaxClassifier_ClassStruct_MemberVariable,
			CppHlslSyntaxClassifier_ClassStruct_MemberVariableIdentifier,
			CppHlslSyntaxClassifier_Alignas,
			CppHlslSyntaxClassifier_Alignas_Alignment,
			
			CppHlslSyntaxClassifier_Type,
			CppHlslSyntaxClassifier_Type_Specification,
			CppHlslSyntaxClassifier_Type_PointerType,
			CppHlslSyntaxClassifier_Type_ReferenceType,
			CppHlslSyntaxClassifier_Type_RvalueReferenceType,
			CppHlslSyntaxClassifier_Type_Value,
			CppHlslSyntaxClassifier_Type_Alias,
			
			CppHlslSyntaxClassifier_Function_Name,
			CppHlslSyntaxClassifier_Function_Instructions,
			CppHlslSyntaxClassifier_Function_Parameter,
			CppHlslSyntaxClassifier_Function_Return,
			CppHlslSyntaxClassifier_Function_Return_Value,

			CppHlslSyntaxClassifier_Namespace,

			CppHlslSyntaxClassifier_INVALID,
		};
		

		enum class CppHlslMainInfo_TypeModifierFlags : SyntaxMainInfoType
		{
			NONE			= 0,

			Const			= 1 <<  0,
			Constexpr		= 1 <<  1,
			Mutable			= 1 <<  2,
			Static			= 1 <<  3,
			ThreadLocal		= 1 <<  4,
			Short			= 1 <<  5,
			Long			= 1 <<  6,
			LongLong		= 1 <<  7,
			Unsigned		= 1 <<  8,
		};

		enum class CppHlslMainInfo_FunctionAttributeFlags : SyntaxMainInfoType
		{
			NONE			= 0,

			Const			= 1 << 0,
			Noexcept		= 1 << 1,
			Override		= 1 << 2,
			Final			= 1 << 3,
			Abstract		= 1 << 4,
			Default			= 1 << 5,
			Delete			= 1 << 6,
		};


		enum class CppHlslSubInfo_AccessModifier : SyntaxSubInfoType
		{
			NONE		= 0,
			
			Public		= 1,
			Protected	= 2,
			Private		= 3,
		};


		enum class CppHlslTypeOf : uint8
		{
			INVALID,
			BuiltInType,
			LiteralType,
			UserDefinedType,
		};

		enum class CppHlslTypeNodeParsingMethod : uint8
		{
			Expression,
			ClassStructMember,
			FunctionParameter,
		};

		enum class CppHlslUserDefinedTypeInfo : uint8
		{
			Default,
			Abstract,
			Derived,
			DerivedFinal,
		};


		static const CppHlslSubInfo_AccessModifier	convertStringToCppClassStructAccessModifier(const std::string& input);


		class CppHlslTypeInfo final
		{
		public:
												CppHlslTypeInfo();
												~CppHlslTypeInfo() = default;
		
		public:
			void								setDefaultInfoXXX(const bool isBuiltIn, const std::string& typeName);
			void								setDeclNameXXX(const std::string& declName);
			void								setSizeXXX(const uint32 size);
			void								setByteOffsetXXX(const uint32 byteOffset);
			void								pushMemberXXX(const CppHlslTypeInfo& member);

		public:
			const uint32						getSize() const noexcept;
			const uint32						getByteOffset() const noexcept;
			const std::string&					getTypeName() const noexcept;
			const std::string&					getDeclName() const noexcept;
			const uint32						getMemberCount() const noexcept;
			const CppHlslTypeInfo&				getMember(const uint32 memberIndex) const noexcept;
		
		public:
			static const CppHlslTypeInfo		kInvalidTypeInfo;

		private:
			bool								_isBuiltIn;
			std::string							_typeName;		// namespace + name
			std::string							_declName;
			uint32								_size;			// Byte count
			uint32								_byteOffset;
			std::vector<CppHlslTypeInfo>		_memberArray;	// Member variables
		};


		class CppHlslTypeTableItem final
		{
		public:
												CppHlslTypeTableItem()	= default;
												CppHlslTypeTableItem(const SymbolTableItem& typeSymbol, const CppHlslMainInfo_TypeModifierFlags& typeModifierFlags);	// Built-in
												CppHlslTypeTableItem(const SymbolTableItem& typeSymbol, const CppHlslUserDefinedTypeInfo& userDefinedTypeInfo);			// User-defined
												CppHlslTypeTableItem(const SyntaxTreeItem& typeSyntax);
												~CppHlslTypeTableItem()	= default;

		public:
			const std::string&					getTypeName() const noexcept;

		public:
			void								setTypeSize(const uint32 typeSize) noexcept;
			const uint32						getTypeSize() const noexcept;

		private:
			SymbolTableItem						_typeSymbol;
			CppHlslMainInfo_TypeModifierFlags	_typeModifierFlags;
			CppHlslUserDefinedTypeInfo			_userDefinedTypeInfo;

		private:
			uint32								_typeSize;
		};


		struct CppHlslTypeModifierSet
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
			const CppHlslMainInfo_TypeModifierFlags		getTypeModifierFlags() const noexcept;
		};


		class CppHlslParser final : public IParser
		{
		public:
														CppHlslParser(ILexer& lexer);
			virtual										~CppHlslParser();

		public:
			virtual const bool							execute() override final;

		private:
			const bool									parseCode(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, uint64& outAdvanceCount);

		private:
			void										generateTypeInfo(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const TreeNodeAccessor<SyntaxTreeItem>& classStructNode);

		private:
			const bool									parseClassStruct(const bool isStruct, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, uint64& outAdvanceCount);
			const bool									parseClassStructMember(const SymbolTableItem& classIdentifierSymbol, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, CppHlslSubInfo_AccessModifier& inOutAccessModifier, uint64& outAdvanceCount, bool& outContinueParsing);
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
			const bool									parseTypeNode(const CppHlslTypeNodeParsingMethod parsingMethod, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, TreeNodeAccessor<SyntaxTreeItem>& outTypeNode, uint64& outAdvanceCount);
			const bool									parseTypeNode_CheckModifiers(const CppHlslTypeNodeParsingMethod parsingMethod, const uint64 symbolPosition, CppHlslTypeModifierSet& outTypeModifierSet, uint64& outAdvanceCount);
		
		private:
			const bool									parseAlignas(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount);
			const bool									parseUsing(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, uint64& outAdvanceCount);
		
		private:
			const bool									parseNamespace(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, uint64& outAdvanceCount);

		private:
			TreeNodeAccessor<SyntaxTreeItem>			findNamespaceNode(const std::string& namespaceFullIdentifier) const noexcept;
			std::string									getNamespaceNodeFullIdentifier(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode) const noexcept;
			std::string									getNamespaceNodeFullIdentifier(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const std::string& subNamespaceIdentifier) const noexcept;

		private:
			TreeNodeAccessor<SyntaxTreeItem>			findNamespaceNodeInternal(const TreeNodeAccessor<SyntaxTreeItem>& parentnamespaceNode, const std::string& namespaceIdentifier) const noexcept;
			const bool									isNamespaceNode(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode) const noexcept;

		private:
			static const CppHlslSyntaxClassifier		convertSymbolToAccessModifierSyntax(const SymbolTableItem& symbol) noexcept;
			static const CppHlslSyntaxClassifier		convertLiteralSymbolToSyntax(const SymbolTableItem& symbol) noexcept;
		
		public:
			void										registerTypeTemplate(const std::string& typeFullIdentifier, const uint32 typeSize);
			
		private:
			void										registerTypeTemplateInternal(const bool isBuiltIn, const std::string& typeFullIdentifier, const uint32 typeSize);
		
		private:
			const uint64								registerType(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const CppHlslTypeTableItem& type);
			std::string									getTypeFullIdentifier(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const std::string& typeIdentifier) const noexcept;
			std::string									getTypeInfoIdentifierXXX(const std::string& typeFullIdentifier) const noexcept;
			static std::string							extractPureTypeName(const std::string& typeFullIdentifier) noexcept;

		private:
			const bool									registerTypeAlias(const std::string& typeAlias, const uint64 typeIndex);
		
		private:
			const bool									isSymbolType(const SymbolTableItem& typeSymbol) const noexcept;
			const bool									isSymbolTypeInternal(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const SymbolTableItem& typeSymbol) const noexcept;
			const bool									isIdentifierType(const std::string& typeFullIdentifier) const noexcept;
			const bool									isBuiltInTypeXXX(const std::string& symbolString) const noexcept;
			const bool									isUserDefinedTypeXXX(const std::string& typeFullIdentifier) const noexcept;
			const std::string&							getUnaliasedSymbolStringXXX(const SymbolTableItem& symbol) const noexcept;
			const CppHlslTypeOf							getTypeOfSymbol(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const SymbolTableItem& symbol) const noexcept;
			const CppHlslTypeTableItem&					getType(const std::string& typeFullIdentifier) const noexcept;

		public:
			const CppHlslTypeInfo&						getTypeInfo(const uint64 typeIndex) const noexcept;
			const CppHlslTypeInfo&						getTypeInfo(const std::string& typeName) const noexcept;
			const uint32								getTypeInfoCount() const noexcept;

		public:
			static const DXGI_FORMAT					convertCppHlslTypeToDxgiFormat(const CppHlslTypeInfo& typeInfo);
			static std::string							convertDeclarationNameToHlslSemanticName(const std::string& declarationName);
			static std::string							serializeCppHlslTypeToHlslStruct(const CppHlslTypeInfo& typeInfo);
			static std::string							serializeCppHlslTypeToHlslCbuffer(const CppHlslTypeInfo& typeInfo, const uint32 registerIndex);

		private:
			TreeNodeAccessor<SyntaxTreeItem>			_globalNamespaceNode;
			TreeNodeAccessor<SyntaxTreeItem>			_currentScopeNamespaceNode;

		private:
			std::vector<CppHlslTypeTableItem>			_typeTable;
			std::unordered_map<std::string, uint64>		_typeTableUmap;

			std::unordered_map<std::string, uint64>		_typeAliasTableUmap;
			
		private:
			std::vector<CppHlslTypeInfo>				_typeInfoArray;
			std::unordered_map<std::string, uint64>		_typeInfoUmap;
		
		private:
			std::unordered_map<std::string, uint64>		_builtInTypeUmap;
		
		private:
			static const SymbolTableItem				kInitializerListSymbol;
			static const SymbolTableItem				kMemberVariableListSymbol;
			static const SymbolTableItem				kParameterListSymbol;
			static const SymbolTableItem				kInstructionListSymbol;
			static const SymbolTableItem				kInvalidGrammarSymbol;
			static const SymbolTableItem				kImplicitIntTypeSymbol;
			static const SymbolTableItem				kGlobalNamespaceSymbol;
		};
	}
}


#include <Language/CppHlslParser.inl>


#endif // !FS_CPP_PARSER_H
