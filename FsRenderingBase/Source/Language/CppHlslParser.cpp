#include <stdafx.h>
#include <FsRenderingBase/Include/Language/CppHlslParser.h>

#include <FsContainer/Include/Vector.hpp>
#include <FsContainer/Include/StringUtil.hpp>
#include <FsContainer/Include/Tree.hpp>
#include <FsContainer/Include/HashMap.hpp>

#include <FsRenderingBase/Include/Language/ILexer.h>

#include <functional>


namespace fs
{
    namespace Language
    {
        const CppHlslTypeInfo CppHlslTypeInfo::kInvalidTypeInfo;
        const SymbolTableItem CppHlslParser::kInitializerListSymbol{ SymbolTableItem(SymbolClassifier::SPECIAL_USE, "InitializerList") };
        const SymbolTableItem CppHlslParser::kMemberVariableListSymbol{ SymbolTableItem(SymbolClassifier::SPECIAL_USE, "MemberVariableList") };
        const SymbolTableItem CppHlslParser::kParameterListSymbol{ SymbolTableItem(SymbolClassifier::SPECIAL_USE, "ParameterList") };
        const SymbolTableItem CppHlslParser::kInstructionListSymbol{ SymbolTableItem(SymbolClassifier::SPECIAL_USE, "InstructionList") };
        const SymbolTableItem CppHlslParser::kInvalidGrammarSymbol{ SymbolTableItem(SymbolClassifier::SPECIAL_USE, "InvalidGrammar") };
        const SymbolTableItem CppHlslParser::kImplicitIntTypeSymbol{ SymbolTableItem(SymbolClassifier::Keyword, "int") };
        const SymbolTableItem CppHlslParser::kGlobalNamespaceSymbol{ SymbolTableItem(SymbolClassifier::Identifier, "GlobalNamespace") };
        const SymbolTableItem CppHlslParser::kRegisterIndexSymbol{ SymbolTableItem(SymbolClassifier::SPECIAL_USE, getCppHlslDefinition(CppHlslDefinitionEnum::RegisterIndex)) };
        CppHlslParser::CppHlslParser(ILexer& lexer)
            : IParser(lexer)
        {
            registerTypeTemplateInternal(true, "void"       ,  0);
            registerTypeTemplateInternal(true, "bool"       ,  4); // 주의!!!
            registerTypeTemplateInternal(true, "int"        ,  4);
            registerTypeTemplateInternal(true, "int1"       ,  4);
            registerTypeTemplateInternal(true, "int2"       ,  8);
            registerTypeTemplateInternal(true, "int3"       , 12);
            registerTypeTemplateInternal(true, "int4"       , 16);
            registerTypeTemplateInternal(true, "uint"       ,  4);
            registerTypeTemplateInternal(true, "uint1"      ,  4);
            registerTypeTemplateInternal(true, "uint2"      ,  8);
            registerTypeTemplateInternal(true, "uint3"      , 12);
            registerTypeTemplateInternal(true, "uint4"      , 16);
            registerTypeTemplateInternal(true, "float"      ,  4);
            registerTypeTemplateInternal(true, "float1"     ,  4);
            registerTypeTemplateInternal(true, "float2"     ,  8);
            registerTypeTemplateInternal(true, "float3"     , 12);
            registerTypeTemplateInternal(true, "float4"     , 16);
            registerTypeTemplateInternal(true, "float4x4"   , 64);
        }

        CppHlslParser::~CppHlslParser()
        {
            __noop;
        }

        const bool CppHlslParser::execute()
        {
            reset();

            TreeNodeAccessor<SyntaxTreeItem> syntaxTreeRootNode = getSyntaxTreeRootNode();
            _globalNamespaceNode = syntaxTreeRootNode.insertChildNode(SyntaxTreeItem(kGlobalNamespaceSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Namespace));

            uint32 advanceCount = 0;
            while (needToContinueParsing() == true)
            {
                if (parseCode(getSymbolPosition(), _globalNamespaceNode, advanceCount) == false)
                {
                    break;
                }

                advanceSymbolPositionXXX(advanceCount);
            }

            if (hasReportedErrors() == true)
            {
                FS_LOG_ERROR("김장원", "에러가 있었습니다!!!");

                return false;
            }

            return true;
        }

        const bool CppHlslParser::parseCode(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, uint32& outAdvanceCount)
        {
            _currentScopeNamespaceNode = namespaceNode;

            const SymbolTableItem& fisrtSymbol = getSymbol(symbolPosition);
            if (fisrtSymbol._symbolString == "class")
            {
                FS_RETURN_FALSE_IF_NOT(parseClassStruct(false, symbolPosition, namespaceNode, outAdvanceCount) == true);
                
                const uint32 childNodeCount = namespaceNode.getChildNodeCount();
                const TreeNodeAccessor classStructNode = namespaceNode.getChildNode(childNodeCount - 1);
                generateTypeInfo(namespaceNode, classStructNode);
                return true;
            }
            else if (fisrtSymbol._symbolString == "struct")
            {
                FS_RETURN_FALSE_IF_NOT(parseClassStruct(true, symbolPosition, namespaceNode, outAdvanceCount) == true);

                const uint32 childNodeCount = namespaceNode.getChildNodeCount();
                const TreeNodeAccessor classStructNode = namespaceNode.getChildNode(childNodeCount - 1);
                generateTypeInfo(namespaceNode, classStructNode);
                return true;
            }
            else if (fisrtSymbol._symbolString == "using")
            {
                FS_RETURN_FALSE_IF_NOT(parseUsing(symbolPosition, namespaceNode, outAdvanceCount) == true);
                return true;
            }
            else if (fisrtSymbol._symbolString == "namespace")
            {
                FS_RETURN_FALSE_IF_NOT(parseNamespace(symbolPosition, namespaceNode, outAdvanceCount) == true);
                return true;
            }
            else
            {
                // expression


            }
            return false;
        }

        void CppHlslParser::generateTypeInfo(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const TreeNodeAccessor<SyntaxTreeItem>& classStructNode)
        {
            CppHlslTypeInfo typeInfo;
            
            bool isTypeNameSet = false;
            uint32 typeSize = 0;
            const uint32 childNodeCount = classStructNode.getChildNodeCount();
            for (uint32 childNodeIndex = 0; childNodeIndex < childNodeCount; ++childNodeIndex)
            {
                const TreeNodeAccessor childNode = classStructNode.getChildNode(childNodeIndex);
                if (childNode.getNodeData()._symbolTableItem._symbolString == "alignas")
                {
                    continue;
                }

                if (isTypeNameSet == false)
                {
                    const std::string typeFullIdentifier = getTypeFullIdentifier(namespaceNode, childNode.getNodeData()._symbolTableItem._symbolString);
                    typeInfo.setDefaultInfo(false, getTypeInfoIdentifierXXX(typeFullIdentifier));
                    isTypeNameSet = true;
                    continue;
                }

                if (isCppHlslDefinition(CppHlslDefinitionEnum::RegisterIndex, childNode.getNodeData()._symbolTableItem._symbolString) == true)
                {
                    const int32 registerIndex = std::stoi(childNode.getChildNode(0).getNodeDataXXX()._symbolTableItem._symbolString);
                    typeInfo.setRegisterIndex(registerIndex);
                }
                else if (childNode.getNodeData()._symbolTableItem == kMemberVariableListSymbol)
                {
                    const uint32 memberCount = childNode.getChildNodeCount();
                    for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
                    {
                        const TreeNodeAccessor memberNode = childNode.getChildNode(memberIndex);
                        
                        const std::string& memberTypeName = memberNode.getNodeData()._symbolTableItem._symbolString;
                        const bool isMemberBuiltInType = isBuiltInTypeXXX(memberTypeName);
                        const std::string memberTypeFullIdentifier = (isMemberBuiltInType == true) ? getTypeFullIdentifier(_globalNamespaceNode, memberTypeName) : getTypeFullIdentifier(namespaceNode, memberNode.getNodeData()._symbolTableItem._symbolString);
                        const CppHlslTypeTableItem& typeTableItem = getType(memberTypeFullIdentifier);

                        const uint32 memberNodeChildCount = memberNode.getChildNodeCount();
                        const TreeNodeAccessor memberDeclNameNode = memberNode.getChildNode(0);
                        CppHlslTypeInfo memberTypeInfo;
                        memberTypeInfo.setDefaultInfo(isMemberBuiltInType, getTypeInfoIdentifierXXX(memberTypeFullIdentifier));
                        memberTypeInfo.setDeclName(memberDeclNameNode.getNodeData()._symbolTableItem._symbolString);
                        if (1 < memberNodeChildCount)
                        {
                            // Has semantic name
                            memberTypeInfo.setSemanticName(memberNode.getChildNode(1).getNodeDataXXX()._symbolTableItem._symbolString);
                        }
                        memberTypeInfo.setSize(typeTableItem.getTypeSize());
                        memberTypeInfo.setByteOffset(typeSize);

                        typeInfo.pushMember(memberTypeInfo);

                        typeSize += typeTableItem.getTypeSize();
                    }
                }
            }

            if (typeSize == 0)
            {
                FS_LOG_ERROR("김장원", "typeSize 가 0 입니다!!!");
                return;
            }

            // Apply HLSL packing
            const uint32 correctedTypeSize = (((typeSize - 1) / 16) + 1) * 16;
            typeInfo.setSize(correctedTypeSize);

            _typeInfoArray.push_back(typeInfo);
            const uint32 typeInfoIndex = _typeInfoArray.size() - 1;
            _typeInfoUmap.insert(typeInfo.getTypeName(), typeInfoIndex);
        }

        const bool CppHlslParser::parseClassStruct(const bool isStruct, const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, uint32& outAdvanceCount)
        {
            // class A;
            // class alignas(4) A;
            // class A {...};
            outAdvanceCount = 0;
            
            const SymbolTableItem& currentSymbol = getSymbol(symbolPosition);
            if (hasSymbol(symbolPosition + 2) == false)
            {
                reportError(currentSymbol, ErrorType::LackOfCode);
                return false;
            }

            TreeNodeAccessor classStructNode = namespaceNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_Keyword));

            uint32 identifierOffset = 1;
            {
                const SymbolTableItem& symbol = getSymbol(symbolPosition + 1);
                if (symbol._symbolClassifier != SymbolClassifier::Identifier)
                {
                    if (symbol._symbolClassifier != SymbolClassifier::Keyword)
                    {
                        reportError(symbol, ErrorType::WrongSuccessor, "class 나 struct 뒤에는 identifier 나 keyword 만 올 수 있습니다.");
                        return false;
                    }

                    if (symbol._symbolString == "alignas")
                    {
                        uint32 alignasAdvanceCount = 1;
                        FS_RETURN_FALSE_IF_NOT(parseAlignas(symbolPosition + 1, classStructNode, alignasAdvanceCount) == true);
                        identifierOffset += alignasAdvanceCount;
                    }
                    else
                    {
                        // TODO: ?? 뭐가 더 있지
                        return false;
                    }
                }
            }
            
            {
                const uint32 identifierSymbolPosition = symbolPosition + identifierOffset;
                const SymbolTableItem& classIdentifierSymbol = getSymbol(identifierSymbolPosition);

                const uint32 typeIndex = registerType(namespaceNode, CppHlslTypeTableItem(classIdentifierSymbol, CppHlslUserDefinedTypeInfo::Default));
                
                classStructNode.insertChildNode(SyntaxTreeItem(classIdentifierSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_Identifier));

                // TODO: final, abstract 등의 postfix 키워드...
                const uint32 postIdentifierSymbolPosition = identifierSymbolPosition + 1;
                const SymbolTableItem& postIdentifierSymbol = getSymbol(postIdentifierSymbolPosition);
                uint32 openSymbolPosition = postIdentifierSymbolPosition;
                if (isCppHlslDefinition(CppHlslDefinitionEnum::RegisterIndex, postIdentifierSymbol._symbolString) == true)
                {
                    const SymbolTableItem& registerIndexSymbol = getSymbol(postIdentifierSymbolPosition + 2);
                    TreeNodeAccessor registerIndexNode = classStructNode.insertChildNode(SyntaxTreeItem(kRegisterIndexSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_RegisterIndex));
                    const TreeNodeAccessor registerIndexIndexNode = registerIndexNode.insertChildNode(SyntaxTreeItem(registerIndexSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_RegisterIndex_Index));

                    openSymbolPosition = postIdentifierSymbolPosition + 4;
                }

                const SymbolTableItem& openSymbol = getSymbol(openSymbolPosition);
                if (openSymbol._symbolClassifier == SymbolClassifier::StatementTerminator)
                {
                    // Declaration

                    outAdvanceCount = identifierOffset + 2;

                    return true;
                }
                else if (openSymbol._symbolClassifier == SymbolClassifier::Grouper_Open)
                {
                    // Definition

                    if (openSymbol._symbolString != "{")
                    {
                        reportError(openSymbol, ErrorType::WrongSuccessor, "'{' 가 와야 합니다.");
                        return false;
                    }

                    uint32 depthMathcingCloseSymbolPosition = 0;
                    if (findNextDepthMatchingCloseSymbol(openSymbolPosition, "}", depthMathcingCloseSymbolPosition) == false)
                    {
                        reportError(postIdentifierSymbol, ErrorType::NoMatchingGrouper, "'}' 를 찾지 못했습니다.");
                        return false;
                    }

                    const SymbolTableItem& statementTerminatorSymbol = getSymbol(depthMathcingCloseSymbolPosition + 1);
                    if (statementTerminatorSymbol._symbolString != ";")
                    {
                        reportError(postIdentifierSymbol, ErrorType::WrongSuccessor, "';' 가 와야 합니다.");
                        return false;
                    }

                    uint32 currentPosition = openSymbolPosition + 1;
                    CppHlslSubInfo_AccessModifier currentAccessModifier = (true == isStruct) ? CppHlslSubInfo_AccessModifier::Public : CppHlslSubInfo_AccessModifier::Private;
                    bool continueParsingMember = false;
                    while (true)
                    {
                        uint32 advanceCount = 0;
                        FS_RETURN_FALSE_IF_NOT(parseClassStructMember(classIdentifierSymbol, currentPosition, classStructNode, currentAccessModifier, advanceCount, continueParsingMember) == true);
                        currentPosition += advanceCount;

                        if (continueParsingMember == false)
                        {
                            break;
                        }
                    }

                    outAdvanceCount = currentPosition - symbolPosition;

                    return true;
                }
                else
                {
                    reportError(postIdentifierSymbol, ErrorType::WrongSuccessor, "';' 나 '{' 가 와야 합니다.");
                    return false;
                }
            }
            return false;
        }

        const bool CppHlslParser::parseClassStructMember(const SymbolTableItem& classIdentifierSymbol, const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, CppHlslSubInfo_AccessModifier& inOutAccessModifier, uint32& outAdvanceCount, bool& outContinueParsing)
        {
            outContinueParsing = true;
            outAdvanceCount = 0;

            if (hasSymbol(symbolPosition) == false)
            {
                return false;
            }

            // End
            {
                const SymbolTableItem& symbol = getSymbol(symbolPosition);
                if (symbol._symbolString == "}")
                {
                    const SymbolTableItem& nextSymbol = getSymbol(symbolPosition + 1);
                    if (nextSymbol._symbolString == ";")
                    {
                        outAdvanceCount = 2;
                        outContinueParsing = false;
                        return true;
                    }
                    else
                    {
                        reportError(nextSymbol, ErrorType::WrongSuccessor, "';' 가 와야 합니다.");
                        return false;
                    }
                }
            }

            // AccessModifier
            uint32 postAccessModifierOffset = 0;
            {
                const SymbolTableItem& symbol = getSymbol(symbolPosition);
                if (convertSymbolToAccessModifierSyntax(symbol) == CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_AccessModifier)
                {
                    const SymbolTableItem& nextSymbol = getSymbol(symbolPosition + 1);
                    if (nextSymbol._symbolString != ":")
                    {
                        reportError(nextSymbol, ErrorType::WrongSuccessor, "':' 가 와야 합니다.");
                        return false;
                    }

                    inOutAccessModifier = convertStringToCppClassStructAccessModifier(symbol._symbolString);
                    postAccessModifierOffset = 2;
                }
            }

            {
                const uint32 currentPosition = symbolPosition + postAccessModifierOffset;
                const SymbolTableItem& symbol = getSymbol(currentPosition);
                const bool isDestructor = (symbol._symbolString == "~");
                if (symbol._symbolString == classIdentifierSymbol._symbolString || isDestructor == true)
                {
                    // Constructor or Destructor
                    const SymbolTableItem& postSymbol = getSymbol(currentPosition + 1);

                    TreeNodeAccessor ctorOrDtorNode = ancestorNode.insertChildNode(SyntaxTreeItem((isDestructor == true) ? postSymbol : symbol, (isDestructor == true) ? CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_Destructor : CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_Constructor));
                    ctorOrDtorNode.getNodeDataXXX().setSubInfo(static_cast<SyntaxSubInfoType>(inOutAccessModifier));

                    const uint32 openParenthesisPosition = (isDestructor == true) ? currentPosition + 2 : currentPosition + 1;
                    const SymbolTableItem& openParenthesisSymbol = getSymbol(openParenthesisPosition);
                    if (openParenthesisSymbol._symbolString != "(")
                    {
                        reportError(openParenthesisSymbol, ErrorType::WrongSuccessor, "'(' 가 와야 합니다.");
                        return false;
                    }

                    uint32 closeParenthesisPosition = 0;
                    if (findNextDepthMatchingCloseSymbol(openParenthesisPosition, ")", closeParenthesisPosition) == false)
                    {
                        reportError(openParenthesisSymbol, ErrorType::NoMatchingGrouper, "')' 를 찾지 못했습니다.");
                        return false;
                    }

                    const SymbolTableItem& closeParenthesisSymbol = getSymbol(closeParenthesisPosition);
                    if (hasSymbol(closeParenthesisPosition + 1) == false)
                    {
                        reportError(closeParenthesisSymbol, ErrorType::LackOfCode);
                        return false;
                    }

                    const SymbolTableItem& postCloseParenthesisSymbol = getSymbol(closeParenthesisPosition + 1);
                    const bool isDeclaration = (postCloseParenthesisSymbol._symbolString == ";");
                    uint32 declarationEndPosition = closeParenthesisPosition + 1;
                    bool hasInitializerList = false;
                    if (isDestructor == false)
                    {
                        // Contructor

                        if (openParenthesisPosition + 1 < closeParenthesisPosition)
                        {
                            FS_RETURN_FALSE_IF_NOT(parseFunctionParameters(isDeclaration, openParenthesisPosition + 1, ctorOrDtorNode) == true);
                        }
                        
                        if (postCloseParenthesisSymbol._symbolString == ":")
                        {
                            uint32 initializeListAdvanceCount = 0;
                            FS_RETURN_FALSE_IF_NOT(parseClassStructInitializerList(closeParenthesisPosition + 2, ctorOrDtorNode, initializeListAdvanceCount) == true);
                            declarationEndPosition += initializeListAdvanceCount;
                            hasInitializerList = true;
                        }
                    }

                    {
                        const SymbolTableItem& symbol = getSymbol(declarationEndPosition);
                        if (symbol._symbolString == ";")
                        {
                            // No instructions

                            if (hasInitializerList == true)
                            {
                                reportError(symbol, ErrorType::LackOfCode, "Initializer list 뒤에는 반드시 정의가 와야 합니다.");
                                return false;
                            }

                            outAdvanceCount = declarationEndPosition - symbolPosition + 1;
                            return true;
                        }
                        else if (symbol._symbolString == "{")
                        {
                            // Instructions

                            uint32 postInstructionAdvance = 0;
                            FS_RETURN_FALSE_IF_NOT(parseFunctionInstructions(declarationEndPosition + 1, ctorOrDtorNode, postInstructionAdvance) == true);
                            outAdvanceCount = (declarationEndPosition + postInstructionAdvance) - symbolPosition + 1;
                            return true;
                        }
                        else
                        {
                            reportError(symbol, ErrorType::WrongSuccessor, "';' 나 '{' 가 와야 합니다.");
                            return false;
                        }
                    }
                }
                else
                {
                    // TODO
                    // Member function/variable

                    bool isFunction = false;
                    uint32 postTypeChunkPosition = 0;
                    if (isTypeChunk(currentPosition, postTypeChunkPosition) == true)
                    {
                        const uint32 openParenthesisPosition = postTypeChunkPosition + 1;
                        const SymbolTableItem& postPostTpeChunkSymbol = getSymbol(openParenthesisPosition);
                        if (postPostTpeChunkSymbol._symbolString == "(")
                        {
                            // Member function

                            isFunction = true;

                            uint32 closeParenthesisPosition = 0;
                            if (findNextDepthMatchingCloseSymbol(openParenthesisPosition, ")", closeParenthesisPosition) == false)
                            {
                                reportError(symbol, ErrorType::NoMatchingGrouper, "')' 를 찾지 못했습니다.");
                                return false;
                            }

                            const SymbolTableItem& identifierSymbol = getSymbol(postTypeChunkPosition);
                            TreeNodeAccessor memberFunctionNode = ancestorNode.insertChildNode(SyntaxTreeItem(identifierSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Function_Name));
                            memberFunctionNode.getNodeDataXXX().setSubInfo(static_cast<SyntaxSubInfoType>(inOutAccessModifier));

                            // function attribute
                            bool isConst    = false;
                            bool isNoexcept = false;
                            bool isOverride = false;
                            bool isFinal    = false;
                            bool isAbstract = false;
                            bool isDefault  = false;
                            bool isDelete   = false;
                            uint32 postAttributePosition    = closeParenthesisPosition + 1;
                            bool hasEqualSign               = false;
                            while (true)
                            {
                                const SymbolTableItem& symbol = getSymbol(postAttributePosition);
                                if (symbol._symbolString == "const")
                                {
                                    if (isConst == true)
                                    {
                                        reportError(symbol, ErrorType::RepetitionOfCode, "속성을 중복해서 지정하고 있습니다.");
                                        return false;
                                    }

                                    isConst = true;
                                    ++postAttributePosition;
                                }
                                else if (symbol._symbolString == "noexcept")
                                {
                                    if (isNoexcept == true)
                                    {
                                        reportError(symbol, ErrorType::RepetitionOfCode, "속성을 중복해서 지정하고 있습니다.");
                                        return false;
                                    }

                                    isNoexcept = true;
                                    ++postAttributePosition;
                                }
                                else if (symbol._symbolString == "override")
                                {
                                    if (isOverride == true)
                                    {
                                        reportError(symbol, ErrorType::RepetitionOfCode, "속성을 중복해서 지정하고 있습니다.");
                                        return false;
                                    }

                                    isOverride = true;
                                    ++postAttributePosition;
                                }
                                else if (symbol._symbolString == "final")
                                {
                                    if (isFinal == true)
                                    {
                                        reportError(symbol, ErrorType::RepetitionOfCode, "속성을 중복해서 지정하고 있습니다.");
                                        return false;
                                    }

                                    isFinal = true;
                                    ++postAttributePosition;
                                }
                                else if (symbol._symbolString == "abstract")
                                {
                                    if (isAbstract == true)
                                    {
                                        reportError(symbol, ErrorType::RepetitionOfCode, "속성을 중복해서 지정하고 있습니다.");
                                        return false;
                                    }

                                    isAbstract = true;
                                    ++postAttributePosition;
                                }
                                else if (symbol._symbolString == "=")
                                {
                                    if (hasEqualSign == true)
                                    {
                                        reportError(symbol, ErrorType::RepetitionOfCode, "'=' 을 중복해서 사용했습니다.");
                                        return false;
                                    }

                                    hasEqualSign = true;
                                    ++postAttributePosition;
                                }
                                else if (symbol._symbolString == "default")
                                {
                                    if (hasEqualSign == false)
                                    {
                                        reportError(symbol, ErrorType::WrongPredecessor, "'=' 을 먼저 써야 합니다.");
                                        return false;
                                    }

                                    if (isDefault == true)
                                    {
                                        reportError(symbol, ErrorType::RepetitionOfCode, "속성을 중복해서 지정하고 있습니다.");
                                        return false;
                                    }

                                    isDefault = true;
                                    ++postAttributePosition;
                                }
                                else if (symbol._symbolString == "delete")
                                {
                                    if (hasEqualSign == false)
                                    {
                                        reportError(symbol, ErrorType::WrongPredecessor, "'=' 을 먼저 써야 합니다.");
                                        return false;
                                    }

                                    if (isDelete == true)
                                    {
                                        reportError(symbol, ErrorType::RepetitionOfCode, "속성을 중복해서 지정하고 있습니다.");
                                        return false;
                                    }

                                    isDelete = true;
                                    ++postAttributePosition;
                                }
                                else if (symbol._symbolString == "0")
                                {
                                    if (hasEqualSign == false)
                                    {
                                        reportError(symbol, ErrorType::WrongPredecessor, "'=' 을 먼저 써야 합니다.");
                                        return false;
                                    }

                                    if (isAbstract == true)
                                    {
                                        reportError(symbol, ErrorType::RepetitionOfCode, "속성을 중복해서 지정하고 있습니다.");
                                        return false;
                                    }

                                    isAbstract = true;
                                    ++postAttributePosition;
                                }
                                else
                                {
                                    break;
                                }
                            }

                            const CppHlslMainInfo_FunctionAttributeFlags attributeFlags = static_cast<CppHlslMainInfo_FunctionAttributeFlags>(
                                static_cast<int32>(isConst)     * static_cast<int32>(CppHlslMainInfo_FunctionAttributeFlags::Const      ) |
                                static_cast<int32>(isNoexcept)  * static_cast<int32>(CppHlslMainInfo_FunctionAttributeFlags::Noexcept   ) |
                                static_cast<int32>(isOverride)  * static_cast<int32>(CppHlslMainInfo_FunctionAttributeFlags::Override   ) |
                                static_cast<int32>(isFinal)     * static_cast<int32>(CppHlslMainInfo_FunctionAttributeFlags::Final      ) |
                                static_cast<int32>(isAbstract)  * static_cast<int32>(CppHlslMainInfo_FunctionAttributeFlags::Abstract   ) |
                                static_cast<int32>(isDefault)   * static_cast<int32>(CppHlslMainInfo_FunctionAttributeFlags::Default    ) |
                                static_cast<int32>(isDelete)    * static_cast<int32>(CppHlslMainInfo_FunctionAttributeFlags::Delete     )
                                );
                            memberFunctionNode.getNodeDataXXX().setMainInfo(static_cast<SyntaxMainInfoType>(attributeFlags));

                            const SymbolTableItem& postAttributeSymbol = getSymbol(postAttributePosition);
                            if (postAttributeSymbol._symbolString == ";")
                            {
                                outAdvanceCount = closeParenthesisPosition - currentPosition + 2;
                                return true;
                            }
                            else if (postAttributeSymbol._symbolString == "{")
                            {
                                FS_RETURN_FALSE_IF_NOT(parseFunctionParameters(false, openParenthesisPosition + 1, memberFunctionNode) == true);

                                uint32 postInstructionAdvance = 0;
                                FS_RETURN_FALSE_IF_NOT(parseFunctionInstructions(postAttributePosition + 1, memberFunctionNode, postInstructionAdvance) == true);

                                outAdvanceCount = postAttributePosition + postInstructionAdvance - currentPosition + 1;
                                return true;
                            }
                            else
                            {
                                reportError(postAttributeSymbol, ErrorType::WrongSuccessor, "';' 나 '{' 가 와야 합니다.");
                                return false;
                            }
                        }
                        else
                        {
                            // Member variable

                            TreeNodeAccessor<SyntaxTreeItem> memberVariableListNode;
                            const uint32 childNodeCount = ancestorNode.getChildNodeCount();
                            for (uint32 childNodeIndex = 0; childNodeIndex < childNodeCount; ++childNodeIndex)
                            {
                                TreeNodeAccessor childNode = ancestorNode.getChildNode(childNodeIndex);
                                if (ancestorNode.getChildNode(childNodeIndex).getNodeData()._symbolTableItem == kMemberVariableListSymbol)
                                {
                                    memberVariableListNode = childNode;
                                    break;
                                }
                            }
                            if (memberVariableListNode.isValid() == false)
                            {
                                memberVariableListNode = ancestorNode.insertChildNode(SyntaxTreeItem(kMemberVariableListSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_MemberVariable));
                            }

                            uint32 postTypeNodeOffset = 0;
                            TreeNodeAccessor<SyntaxTreeItem> typeNode;
                            FS_RETURN_FALSE_IF_NOT(parseTypeNode(CppHlslTypeNodeParsingMethod::ClassStructMember, currentPosition, memberVariableListNode, typeNode, postTypeNodeOffset) == true);
                            typeNode.getNodeDataXXX().setSubInfo(static_cast<SyntaxSubInfoType>(inOutAccessModifier));

                            const SymbolTableItem& identifierSymbol = getSymbol(postTypeChunkPosition);
                            const TreeNodeAccessor identifierNode = typeNode.insertChildNode(SyntaxTreeItem(identifierSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_MemberVariableIdentifier));

                            const uint32 postIdentifierSymbolPosition = postTypeChunkPosition + 1;
                            const SymbolTableItem& postIdentifierSymbol = getSymbol(postTypeChunkPosition + 1);
                            if (postIdentifierSymbol._symbolString == ";")
                            {
                                // No initialization

                                outAdvanceCount = postIdentifierSymbolPosition - symbolPosition + 1;
                                return true;
                            }
                            else if (postIdentifierSymbol._symbolString == "(" || postIdentifierSymbol._symbolString == "{")
                            {
                                // TODO
                                // Initialization

                                uint32 closeSymbolPosition = 0;
                                if (findNextDepthMatchingCloseSymbol(postIdentifierSymbolPosition, (postIdentifierSymbol._symbolString == "(") ? ")" : "}", closeSymbolPosition) == false)
                                {
                                    reportError(postIdentifierSymbol, ErrorType::NoMatchingGrouper, "')' 나 '}' 를 찾지 못했습니다.");
                                    return false;
                                }

                                outAdvanceCount = closeSymbolPosition - symbolPosition + 2;
                                return true;
                            }
                            else
                            {
                                if (isCppHlslDefinition(CppHlslDefinitionEnum::SemanticName, postIdentifierSymbol._symbolString) == true)
                                {
                                    const SymbolTableItem& semanticNameSymbol = getSymbol(postTypeChunkPosition + 3);
                                    const TreeNodeAccessor semanticNameNode = typeNode.insertChildNode(SyntaxTreeItem(semanticNameSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_SemanticName));

                                    outAdvanceCount = postTypeChunkPosition + 6 - symbolPosition;
                                    return true;
                                }

                                // TODO ??
                                reportError(postIdentifierSymbol, ErrorType::WrongSuccessor, "';' 나 '=' 나 '{' 가 와야 합니다.");
                                return false;
                            }
                        }
                    }
                    else
                    {
                        reportError(symbol, ErrorType::WrongSuccessor, "함수 또는 변수가 와야 합니다!");
                        return false;
                    }
                }
            }
            return false;
        }

        const bool CppHlslParser::parseClassStructInitializerList(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint32& outAdvanceCount)
        {
            outAdvanceCount = 0;

            if (hasSymbol(symbolPosition + 3) == false)
            {
                const SymbolTableItem& firstMemberSymbol = getSymbol(symbolPosition);
                reportError(firstMemberSymbol, ErrorType::LackOfCode);
                return false;
            }

            TreeNodeAccessor initializerListNode = ancestorNode.insertChildNode(SyntaxTreeItem(kInitializerListSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_Constructor_InitializerList));
            uint32 currentOffset = 0;
            bool continueParsing = true;
            do
            {
                uint32 advanceCount = 0;
                FS_RETURN_FALSE_IF_NOT(parseClassStructInitializerList_Item(symbolPosition + currentOffset, initializerListNode, advanceCount, continueParsing) == true);

                currentOffset += advanceCount;
            } while (continueParsing == true);

            outAdvanceCount = currentOffset;
            return true;
        }

        const bool CppHlslParser::parseClassStructInitializerList_Item(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint32& outAdvanceCount, bool& outContinueParsing)
        {
            const SymbolTableItem& memberSymbol = getSymbol(symbolPosition);

            if (hasSymbol(symbolPosition + 3) == false)
            {
                reportError(memberSymbol, ErrorType::LackOfCode);
                return false;
            }

            const SymbolTableItem& openSymbol = getSymbol(symbolPosition + 1);
            const bool isOpenSymbolParenthesis = (openSymbol._symbolString == "(");
            if (isOpenSymbolParenthesis == true || openSymbol._symbolString == "{")
            {
                const SymbolTableItem& valueSymbol = getSymbol(symbolPosition + 2);
                if (valueSymbol._symbolClassifier == SymbolClassifier::Identifier)
                {
                    // Parameter

                    TreeNodeAccessor classStructCtorNode = ancestorNode.getParentNode();
                    uint32 parameterIndex = kUint32Max;
                    const uint32 childNodeCount = classStructCtorNode.getChildNodeCount();
                    {
                        TreeNodeAccessor parameterListNode = classStructCtorNode.getChildNode(0);
                        if (parameterListNode.getNodeData()._symbolTableItem != kParameterListSymbol)
                        {
                            reportError(valueSymbol, ErrorType::SymbolNotFound, "Parameter 가 없는 함수인데 parameter 를 이용해 초기화하고 있습니다");
                            return false;
                        }

                        const uint32 parameterCount = parameterListNode.getChildNodeCount();
                        for (uint32 parameterIter = 0; parameterIter < parameterCount; ++parameterIter)
                        {
                            TreeNodeAccessor parameterNode = parameterListNode.getChildNode(parameterIter);
                            const uint32 parameterNodeChildNodeCount = parameterNode.getChildNodeCount();
                            TreeNodeAccessor parameterIdentifierNode = parameterNode.getChildNode(parameterNodeChildNodeCount - 1);
                            if (parameterIdentifierNode.getNodeData()._symbolTableItem._symbolString == valueSymbol._symbolString)
                            {
                                parameterIndex = parameterIter;
                                break;
                            }
                        }
                    }

                    if (parameterIndex == kUint32Max)
                    {
                        reportError(valueSymbol, ErrorType::SymbolNotFound);
                        return false;
                    }

                    TreeNodeAccessor memberNode = ancestorNode.insertChildNode(SyntaxTreeItem(memberSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_MemberVariable));
                    memberNode.insertChildNode(SyntaxTreeItem(valueSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Function_Parameter));

                    const SymbolTableItem& postCloseSymbol = getSymbol(symbolPosition + 4);
                    if (postCloseSymbol._symbolString == ",")
                    {
                        outContinueParsing = true;
                        outAdvanceCount += 5;
                    }
                    else
                    {
                        outContinueParsing = false;
                        outAdvanceCount += 4;
                    }

                    return true;
                }
                else
                {
                    const SymbolTableItem& closeSymbol = getSymbol(symbolPosition + 3);
                    if (isOpenSymbolParenthesis == true)
                    {
                        if (closeSymbol._symbolString != ")")
                        {
                            reportError(closeSymbol, ErrorType::GrouperMismatch, "')' 가 와야 합니다.");
                            return false;
                        }
                    }
                    else
                    {
                        if (closeSymbol._symbolString != "}")
                        {
                            reportError(closeSymbol, ErrorType::GrouperMismatch, "'}' 가 와야 합니다.");
                            return false;
                        }
                    }

                    if (hasSymbol(symbolPosition + 4) == false)
                    {
                        reportError(closeSymbol, ErrorType::LackOfCode);
                        return false;
                    }

                    TreeNodeAccessor memberNode = ancestorNode.insertChildNode(SyntaxTreeItem(memberSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_MemberVariable));
                    memberNode.insertChildNode(SyntaxTreeItem(valueSymbol, convertLiteralSymbolToSyntax(valueSymbol)));

                    const SymbolTableItem& postCloseSymbol = getSymbol(symbolPosition + 4);
                    if (postCloseSymbol._symbolString == ",")
                    {
                        outContinueParsing = true;
                        outAdvanceCount += 6;
                    }
                    else
                    {
                        outContinueParsing = false;
                        outAdvanceCount += 5;
                    }
                    
                    return true;
                }
            }

            reportError(openSymbol, ErrorType::WrongSuccessor, "'(' 나 '{' 가 와야 합니다.");
            return false;
        }

        const bool CppHlslParser::parseFunctionParameters(const bool isDeclaration, const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode)
        {
            const SymbolTableItem& currentSymbol = getSymbol(symbolPosition);
            if (currentSymbol._symbolString == ")")
            {
                // No parameters.
                return true;
            }

            uint32 closeParenthesisPosition = 0;
            if (findNextSymbol(symbolPosition, ")", closeParenthesisPosition) == false)
            {
                reportError(currentSymbol, ErrorType::LackOfCode);
                return false;
            }

            TreeNodeAccessor<SyntaxTreeItem> parameterListNode = ancestorNode.insertChildNode(SyntaxTreeItem(kParameterListSymbol, CppHlslSyntaxClassifier_Function_Parameter));

            uint32 currentItemOffset = 0;
            uint32 advanceCount = 0;
            do
            {
                FS_RETURN_FALSE_IF_NOT(parseFunctionParameters_Item(isDeclaration, symbolPosition + currentItemOffset, parameterListNode, advanceCount) == true);
                currentItemOffset += advanceCount;
            } while (0 < advanceCount);

            return true;
        }

        const bool CppHlslParser::parseFunctionParameters_Item(const bool isDeclaration, const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint32& outAdvanceCount)
        {
            outAdvanceCount = 0;

            TreeNodeAccessor<SyntaxTreeItem> typeNode;
            uint32 postTypeNodeOffset = 0;
            FS_RETURN_FALSE_IF_NOT(parseTypeNode(CppHlslTypeNodeParsingMethod::FunctionParameter, symbolPosition, ancestorNode, typeNode, postTypeNodeOffset) == true);

            const uint32 postfixOffset = postTypeNodeOffset;
            {
                const SymbolTableItem& symbol = getSymbol(symbolPosition + postfixOffset);
                if (symbol._symbolString == ",")
                {
                    outAdvanceCount = postfixOffset + 1;
                    return true;
                }
                else if (symbol._symbolString == ")")
                {
                    outAdvanceCount = 0;
                    return true;
                }
                else if (symbol._symbolClassifier == SymbolClassifier::Identifier)
                {
                    typeNode.insertChildNode(SyntaxTreeItem(symbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Function_Parameter));

                    const SymbolTableItem& postSymbol = getSymbol(symbolPosition + postfixOffset + 1);
                    if (postSymbol._symbolString == "," || postSymbol._symbolString == ")")
                    {
                        if (postSymbol._symbolString == ")")
                        {
                            outAdvanceCount = 0;
                        }
                        else
                        {
                            outAdvanceCount = postfixOffset + 2;
                        }
                        return true;
                    }
                    else
                    {
                        reportError(postSymbol, ErrorType::WrongSuccessor, "',' 나 ')' 가 와야 합니다.");
                        return false;
                    }
                }
            }

            return false;
        }

        const bool CppHlslParser::parseFunctionInstructions(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint32& outAdvanceCount)
        {
            outAdvanceCount = 0;

            // parseStatement (;)
            // parseExpression...?

            TreeNodeAccessor<SyntaxTreeItem> instructionListNode = ancestorNode.insertChildNode(SyntaxTreeItem(kInstructionListSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Function_Instructions));
            FS_RETURN_FALSE_IF_NOT(parseExpression(symbolPosition, instructionListNode, outAdvanceCount));
            ++outAdvanceCount;

            return true;
        }

        const bool CppHlslParser::parseExpression(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint32& outAdvanceCount)
        {
            outAdvanceCount = 0;

            const SymbolTableItem firstSymbol = getSymbol(symbolPosition);
            if (firstSymbol._symbolString == "(")
            {
                uint32 closeSymbolPosition = 0;
                if (findNextDepthMatchingCloseSymbol(symbolPosition, ")", closeSymbolPosition) == false)
                {
                    reportError(firstSymbol, ErrorType::NoMatchingGrouper, "')' 를 찾을 수 없었습니다.");
                    return false;
                }

                uint32 symbolIter = symbolPosition + 1;
                while (symbolIter < closeSymbolPosition)
                {
                    uint32 advanceCount = 0;
                    FS_RETURN_FALSE_IF_NOT(parseExpression(symbolIter, ancestorNode, advanceCount) == true);
                    symbolIter += advanceCount;
                }
            }
            else
            {
                uint32 postTypeChunkPosition = 0;
                if (isTypeChunk(symbolPosition, postTypeChunkPosition) == true)
                {
                    // Declaration

                    uint32 postTypeNodeOffset = 0;
                    TreeNodeAccessor<SyntaxTreeItem> typeNode;
                    FS_RETURN_FALSE_IF_NOT(parseTypeNode(CppHlslTypeNodeParsingMethod::Expression, symbolPosition, ancestorNode, typeNode, postTypeNodeOffset) == true);

                    // 4) identifier
                    {
                        const SymbolTableItem identifierSymbol = getSymbol(symbolPosition + postTypeNodeOffset);
                        if (identifierSymbol._symbolClassifier != SymbolClassifier::Identifier)
                        {
                            reportError(identifierSymbol, ErrorType::WrongSuccessor, "변수 이름이 와야 합니다!");
                            return false;
                        }
                    }

                    // TODO
                    // , ...
                    // array
                    // initialization
                    // Statement terminator check ';'
                    {
                        const SymbolTableItem symbol = getSymbol(symbolPosition + postTypeNodeOffset);
                        if (symbol._symbolString == ";")
                        {
                            outAdvanceCount = postTypeNodeOffset + 1;
                            return true;
                        }
                    }
                }
                else
                {
                    // Non-declaration

                    const SymbolTableItem symbol = getSymbol(symbolPosition);
                    if (symbol._symbolString == "__noop")
                    {
                        const SymbolTableItem nextSymbol = getSymbol(symbolPosition + 1);
                        if (nextSymbol._symbolString != ";")
                        {
                            reportError(nextSymbol, ErrorType::WrongSuccessor, "';' 가 와야 합니다.");
                            return false;
                        }

                        outAdvanceCount = 2;
                        return true;
                    }
                    else if (symbol._symbolString == "return")
                    {
                        const SymbolTableItem nextSymbol = getSymbol(symbolPosition + 1);
                        if (nextSymbol._symbolString == ";")
                        {
                            // 'return;'

                            ancestorNode.insertChildNode(SyntaxTreeItem(symbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Function_Return));

                            outAdvanceCount = 2;
                            return true;
                        }
                        else
                        {
                            TreeNodeAccessor returnNode = ancestorNode.insertChildNode(SyntaxTreeItem(symbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Function_Return));
                            const CppHlslTypeOf cppTypeOf = getTypeOfSymbol(_currentScopeNamespaceNode, nextSymbol);
                            if (cppTypeOf == CppHlslTypeOf::INVALID)
                            {
                                reportError(nextSymbol, ErrorType::WrongSuccessor, "type 이 와야 합니다.");
                                return false;
                            }

                            TreeNodeAccessor valueNode = returnNode.insertChildNode(SyntaxTreeItem(nextSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Function_Return_Value));

                            const SymbolTableItem postNextSymbol = getSymbol(symbolPosition + 2);
                            if (postNextSymbol._symbolString != ";")
                            {
                                reportError(nextSymbol, ErrorType::WrongSuccessor, "';' 가 와야 합니다.");
                                return false;
                            }

                            outAdvanceCount = 3;
                            return true;
                        }
                    }

                    // TODO
                    // ...
                    DebugBreak();
                    return false;
                }
            }
            
            // (binary) operator 다음에 (unary) identifier (unary)
            // a++, a-- == rvalue
            // ++a, --a == lvalue
            // ++(a++) // INVALID
            // (++a)++ // VALID

            return false;
        }

        const bool CppHlslParser::isTypeChunk(const uint32 symbolPosition, uint32& outPostTypeChunkPosition)
        {
            uint32 offset = 0;
            bool foundTypeSymbol = false;
            while (true)
            {
                const SymbolTableItem& symbol = getSymbol(symbolPosition + offset);
                if (isSymbolType(symbol) == true)
                {
                    foundTypeSymbol = true;
                    ++offset;
                }
                else
                {
                    if (symbol._symbolString == "const" || symbol._symbolString == "constexpr" || symbol._symbolString == "mutable" || 
                        symbol._symbolString == "static" || symbol._symbolString == "thread_local")
                    {
                        ++offset;
                    }
                    else if (symbol._symbolString == "*" || symbol._symbolString == "&" || symbol._symbolString == "&&")
                    {
                        ++offset;
                    }
                    else if (symbol._symbolClassifier == SymbolClassifier::Identifier)
                    {
                        const SymbolTableItem& nextSymbol = getSymbol(symbolPosition + offset + 1);
                        if (nextSymbol._symbolString == "::")
                        {
                            // namespace
                            offset += 2;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }

            outPostTypeChunkPosition = symbolPosition + offset;
            return foundTypeSymbol;
        }

        const bool CppHlslParser::parseTypeNode(const CppHlslTypeNodeParsingMethod parsingMethod, const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, TreeNodeAccessor<SyntaxTreeItem>& outTypeNode, uint32& outAdvanceCount)
        {
            outAdvanceCount = 0;

            /*
            int a = 0;
            int* pa = &a;
            const int const* cpa = &a;
            int** ppa = &pa;
            int*** pppa = &ppa;
            int& ra = a;
            int const& ra0 = a;
            int& const ra1 = a;
            */

            // int signed a = 0;
            // long signed a = 0;
            // signed signed int a = 0;
            // short a = 0;
            // short int a = 0;
            // long a = 0;
            // long int a = 0;
            // long long a = 0;
            // long long int a = 0;
            // float long a = 0;
            // long double a = 0;

            // modifier: static constexpr const mutable thread_local
            //           signed unsigned long
            // 1) Premodifier
            // 2) (namespace::) Type(T) and postmodifier
            // 3) * (const) or & &&
            // 4) identifier

            CppHlslTypeModifierSet typeModifierSet;

            // 1) Premodifier
            uint32 postPremodifierOffset = 0;
            FS_RETURN_FALSE_IF_NOT(parseTypeNode_CheckModifiers(parsingMethod, symbolPosition, typeModifierSet, postPremodifierOffset));
            
            // 2) namespace::
            bool isImplicitIntType = false;
            bool isNamespaceSpecified = false;
            uint32 postNamespaceOffset = postPremodifierOffset;
            std::string typeFullIdentifier = kGlobalNamespaceSymbol._symbolString;
            while (true)
            {
                const SymbolTableItem symbol = getSymbol(symbolPosition + postNamespaceOffset);
                const SymbolTableItem postSymbol = getSymbol(symbolPosition + postNamespaceOffset + 1);
                const bool isSymbolNamespace = (postSymbol._symbolString == "::");
                if (isSymbolNamespace == true)
                {
                    postNamespaceOffset += 2;

                    typeFullIdentifier += "::";
                    typeFullIdentifier += symbol._symbolString;

                    isNamespaceSpecified = true;
                }
                else
                {
                    if (typeModifierSet._isShort == true || 0 != typeModifierSet._longState)
                    {
                        // implicit int type
                        isImplicitIntType = true;
                    }
                    break;
                }
            }

            // Type
            const SymbolTableItem& typeSymbol = (isImplicitIntType == true) ? kImplicitIntTypeSymbol : getSymbol(symbolPosition + postNamespaceOffset);
            if (isNamespaceSpecified == true)
            {
                typeFullIdentifier += "::";
                typeFullIdentifier += typeSymbol._symbolString;
                if (isIdentifierType(typeFullIdentifier) == false)
                {
                    reportError(typeSymbol, ErrorType::WrongSuccessor, "Type 이 와야 합니다.");
                    return false;
                }
            }
            else
            {
                if (isSymbolType(typeSymbol) == false)
                {
                    reportError(typeSymbol, ErrorType::WrongSuccessor, "Type 이 와야 합니다.");
                    return false;
                }
            }

            // Postmodifier
            uint32 postPostmodifierOffset = (isImplicitIntType == true) ? postNamespaceOffset : postNamespaceOffset + 1;
            {
                uint32 advanceCount = 0;
                FS_RETURN_FALSE_IF_NOT(parseTypeNode_CheckModifiers(parsingMethod, symbolPosition + postPostmodifierOffset, typeModifierSet, advanceCount));
                postPostmodifierOffset += advanceCount;
            }
            
            outTypeNode = ancestorNode.insertChildNode(SyntaxTreeItem(typeSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Type));
            outTypeNode.getNodeDataXXX().setMainInfo(static_cast<SyntaxMainInfoType>(typeModifierSet.getTypeModifierFlags()));

            // 3) * (const) or & &&
            uint32 postPointerReferenceOffset = postPostmodifierOffset;
            {
                uint8 ampersandCount = 0; // [0, 2]
                TreeNodeAccessor<SyntaxTreeItem> previousNode;
                while (true)
                {
                    const uint32 currentPosition = symbolPosition + postPointerReferenceOffset;
                    if (hasSymbol(currentPosition) == false)
                    {
                        break;
                    }

                    const SymbolTableItem& currentSymbol = getSymbol(currentPosition);
                    if (currentSymbol._symbolString == "&")
                    {
                        ++ampersandCount;
                        if (ampersandCount == 1)
                        {
                            previousNode = outTypeNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Type_ReferenceType));
                        }
                        else if (ampersandCount == 2)
                        {
                            previousNode = outTypeNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Type_RvalueReferenceType));
                        }
                        else
                        {
                            reportError(currentSymbol, ErrorType::WrongSuccessor, "참조에 대한 참조는 사용할 수 없습니다.");
                            return false;
                        }
                    }
                    else if (currentSymbol._symbolString == "const")
                    {
                        previousNode.getNodeDataXXX().setMainInfo(static_cast<SyntaxMainInfoType>(CppHlslMainInfo_TypeModifierFlags::Const));
                    }
                    else if (currentSymbol._symbolString == "*")
                    {
                        if (0 < ampersandCount)
                        {
                            reportError(currentSymbol, ErrorType::WrongSuccessor, "참조에 대한 포인터는 사용할 수 없습니다.");
                            return false;
                        }

                        previousNode = outTypeNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Type_PointerType));
                    }
                    else
                    {
                        break;
                    }

                    ++postPointerReferenceOffset;
                }
            }

            outAdvanceCount = postPointerReferenceOffset;
            return true;
        }

        const bool CppHlslParser::parseTypeNode_CheckModifiers(const CppHlslTypeNodeParsingMethod parsingMethod, const uint32 symbolPosition, CppHlslTypeModifierSet& outTypeModifierSet, uint32& outAdvanceCount)
        {
            outAdvanceCount = 0;
            
            while (true)
            {
                const SymbolTableItem& symbol = getSymbol(symbolPosition + outAdvanceCount);
                if (symbol._symbolClassifier != SymbolClassifier::Keyword)
                {
                    break;
                }

                if (symbol._symbolString == "static")
                {
                    if (outTypeModifierSet._isStatic == true)
                    {
                        reportError(symbol, ErrorType::RepetitionOfCode, "'static' 이 중복됩니다.");
                        return false;
                    }
                    if (parsingMethod == CppHlslTypeNodeParsingMethod::FunctionParameter)
                    {
                        reportError(symbol, ErrorType::WrongScope, "'static' 을 여기에 사용할 수 없습니다.");
                        return false;
                    }
                    outTypeModifierSet._isStatic = true;
                }
                else if (symbol._symbolString == "constexpr")
                {
                    if (outTypeModifierSet._isConstexpr == true)
                    {
                        reportError(symbol, ErrorType::RepetitionOfCode, "'constexpr' 이 중복됩니다.");
                        return false;
                    }
                    if (parsingMethod == CppHlslTypeNodeParsingMethod::FunctionParameter)
                    {
                        reportError(symbol, ErrorType::WrongScope, "'constexpr' 을 여기에 사용할 수 없습니다.");
                        return false;
                    }
                    outTypeModifierSet._isConstexpr = true;
                }
                else if (symbol._symbolString == "const")
                {
                    outTypeModifierSet._isConst = true; // const 는 중복 가능!!
                }
                else if (symbol._symbolString == "mutable")
                {
                if (outTypeModifierSet._isMutable == true)
                {
                    reportError(symbol, ErrorType::RepetitionOfCode, "'mutable' 이 중복됩니다.");
                    return false;
                }
                if (parsingMethod != CppHlslTypeNodeParsingMethod::ClassStructMember)
                {
                    reportError(symbol, ErrorType::WrongScope, "'mutable' 을 여기에 사용할 수 없습니다.");
                    return false;
                }
                outTypeModifierSet._isMutable = true;
                }
                else if (symbol._symbolString == "thread_local")
                {
                if (outTypeModifierSet._isThreadLocal == true)
                {
                    reportError(symbol, ErrorType::RepetitionOfCode, "'thread_local' 이 중복됩니다.");
                    return false;
                }
                if (parsingMethod != CppHlslTypeNodeParsingMethod::Expression)
                {
                    reportError(symbol, ErrorType::WrongScope, "'thread_local' 을 여기에 사용할 수 없습니다.");
                    return false;
                }
                outTypeModifierSet._isThreadLocal = true;
                }
                else if (symbol._symbolString == "short")
                {
                if (outTypeModifierSet._isShort == true)
                {
                    reportError(symbol, ErrorType::RepetitionOfCode, "'short' 가 중복됩니다.");
                    return false;
                }
                else if (0 < outTypeModifierSet._longState)
                {
                    reportError(symbol, ErrorType::WrongSuccessor, "'long' 에 'short' 를 붙일 수 없습니다.");
                    return false;
                }
                outTypeModifierSet._isShort = true;
                }
                else if (symbol._symbolString == "long")
                {
                if (outTypeModifierSet._isShort == true)
                {
                    reportError(symbol, ErrorType::WrongSuccessor, "'short' 에 'long' 을 붙일 수 없습니다.");
                    return false;
                }
                if (2 <= outTypeModifierSet._longState)
                {
                    reportError(symbol, ErrorType::RepetitionOfCode, "'long' 을 더 붙일 수 없습니다.");
                    return false;
                }
                ++outTypeModifierSet._longState;
                }
                else if (symbol._symbolString == "signed")
                {
                if (outTypeModifierSet._signState == 2)
                {
                    reportError(symbol, ErrorType::WrongSuccessor, "'unsigned' 에 'signed' 를 붙일 수 없습니다.");
                    return false;
                }

                outTypeModifierSet._signState = 1;
                }
                else if (symbol._symbolString == "unsigned")
                {
                if (outTypeModifierSet._signState == 1)
                {
                    reportError(symbol, ErrorType::WrongSuccessor, "'signed' 에 'unsigned' 를 붙일 수 없습니다.");
                    return false;
                }

                outTypeModifierSet._signState = 2;
                }
                else
                {
                break;
                }

                ++outAdvanceCount;
            }

            return true;
        }

        const bool CppHlslParser::parseAlignas(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint32& outAdvanceCount)
        {
            outAdvanceCount = 0;

            const SymbolTableItem& alignasSymbol = getSymbol(symbolPosition);
            if (hasSymbol(symbolPosition + 4) == false)
            {
                reportError(alignasSymbol, ErrorType::LackOfCode);
                return false;
            }

            const SymbolTableItem& openSymbol = getSymbol(symbolPosition + 1);
            if (openSymbol._symbolString != "(")
            {
                reportError(openSymbol, ErrorType::WrongSuccessor, "'(' 가 와야 합니다.");
                return false;
            }

            const SymbolTableItem& closeSymbol = getSymbol(symbolPosition + 3);
            if (closeSymbol._symbolString != ")")
            {
                reportError(closeSymbol, ErrorType::WrongSuccessor, "')' 가 와야 합니다.");
                return false;
            }
            
            TreeNodeAccessor alignasNode = ancestorNode.insertChildNode(SyntaxTreeItem(alignasSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Alignas));
            const SymbolTableItem& alignmentSymbol = getSymbol(symbolPosition + 2);
            alignasNode.insertChildNode(SyntaxTreeItem(alignmentSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Alignas_Alignment));

            outAdvanceCount = 4;
            return true;
        }

        const bool CppHlslParser::parseUsing(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, uint32& outAdvanceCount)
        {
            FS_RETURN_FALSE_IF_NOT(hasSymbol(symbolPosition + 4) == true);

            outAdvanceCount = 0;

            const SymbolTableItem& usingSymbol = getSymbol(symbolPosition);
            if (usingSymbol._symbolString != "using")
            {
                // using 이 아니다!
                return false;
            }

            const SymbolTableItem& aliasSymbol = getSymbol(symbolPosition + 1);
            if (aliasSymbol._symbolClassifier != SymbolClassifier::Identifier)
            {
                reportError(aliasSymbol, ErrorType::WrongSuccessor, "'using' 뒤에는 이름이 와야 합니다.");
                return false;
            }

            const SymbolTableItem& equalSymbol = getSymbol(symbolPosition + 2);
            if (equalSymbol._symbolString != "=")
            {
                reportError(equalSymbol, ErrorType::WrongSuccessor, "'=' 가 와야 합니다.");
                return false;
            }

            TreeNodeAccessor<SyntaxTreeItem> usingNode = namespaceNode.insertChildNode(SyntaxTreeItem(usingSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Type_Alias));
            TreeNodeAccessor<SyntaxTreeItem> aliasNode = usingNode.insertChildNode(SyntaxTreeItem(aliasSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Type_Alias));
            
            TreeNodeAccessor<SyntaxTreeItem> typeNode;
            uint32 postTypeNodeOffset = 0;
            FS_RETURN_FALSE_IF_NOT(parseTypeNode(CppHlslTypeNodeParsingMethod::FunctionParameter, symbolPosition + 3, usingNode, typeNode, postTypeNodeOffset) == true);

            const uint32 typeIndex = registerType(namespaceNode, CppHlslTypeTableItem(typeNode.getNodeData()));
            FS_RETURN_FALSE_IF_NOT(registerTypeAlias(aliasSymbol._symbolString, typeIndex) == true);

            outAdvanceCount = postTypeNodeOffset + 4;
            return true;
        }

        const bool CppHlslParser::parseNamespace(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, uint32& outAdvanceCount)
        {
            // Non-statement

            FS_RETURN_FALSE_IF_NOT(hasSymbol(symbolPosition + 3) == true);

            outAdvanceCount = 0;

            const SymbolTableItem& namespaceSymbol = getSymbol(symbolPosition);
            if (namespaceSymbol._symbolString != "namespace")
            {
                // namespace 가 아니다!
                return false;
            }

            const SymbolTableItem& identifierSymbol = getSymbol(symbolPosition + 1);
            if (identifierSymbol._symbolClassifier != SymbolClassifier::Identifier)
            {
                reportError(identifierSymbol, ErrorType::WrongSuccessor, "namespace 뒤에는 identifier 가 와야 합니다!");
                return false;
            }

            const SymbolTableItem& openSymbol = getSymbol(symbolPosition + 2);
            if (openSymbol._symbolString != "{")
            {
                reportError(openSymbol, ErrorType::WrongSuccessor, "'{' 가 와야 합니다!");
                return false;
            }

            uint32 closeSymbolPosition = 0;
            if (findNextDepthMatchingCloseSymbol(symbolPosition + 2, "}", closeSymbolPosition) == false)
            {
                reportError(openSymbol, ErrorType::NoMatchingGrouper, "'}' 를 찾지 못했습니다!");
                return false;
            }

            TreeNodeAccessor newNamespaceNode = findNamespaceNodeInternal(namespaceNode, identifierSymbol._symbolString);
            if (newNamespaceNode.isValid() == false)
            {
                newNamespaceNode = namespaceNode.insertChildNode(SyntaxTreeItem(identifierSymbol, CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Namespace));
            }

            uint32 symbolIter = symbolPosition + 3;
            while (symbolIter < closeSymbolPosition)
            {
                uint32 advanceCount = 0;
                FS_RETURN_FALSE_IF_NOT(parseCode(symbolIter, newNamespaceNode, advanceCount) == true);
                symbolIter += advanceCount;
            }

            outAdvanceCount = symbolIter - symbolPosition + 1;
            return true;
        }

        TreeNodeAccessor<SyntaxTreeItem> CppHlslParser::findNamespaceNode(const std::string& namespaceFullIdentifier) const noexcept
        {
            fs::Vector<std::string> namespaceStack;
            fs::StringUtil::tokenize(namespaceFullIdentifier, "::", namespaceStack);

            TreeNodeAccessor<SyntaxTreeItem> result = _globalNamespaceNode;
            const uint32 namespaceStackDepth = namespaceStack.size();
            for (uint32 iter = 1; iter < namespaceStackDepth; ++iter)
            {
                result = findNamespaceNodeInternal(result, namespaceStack[iter]);
            }
            return result;
        }

        std::string CppHlslParser::getNamespaceNodeFullIdentifier(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode) const noexcept
        {
            std::string result;

            TreeNodeAccessor<SyntaxTreeItem> currentNamespaceNode = namespaceNode;
            fs::Vector<std::string> namespaceStack;
            while (true)
            {
                if (isNamespaceNode(currentNamespaceNode) == false)
                {
                    break;
                }
                namespaceStack.push_back(currentNamespaceNode.getNodeData()._symbolTableItem._symbolString);
                currentNamespaceNode = currentNamespaceNode.getParentNode();
            }

            const uint32 stackDepth = namespaceStack.size();
            if (0 < stackDepth)
            {
                result = namespaceStack.back();
                namespaceStack.pop_back();
                for (uint32 iter = 0; iter < stackDepth - 1; ++iter)
                {
                    result += "::";
                    result += namespaceStack.back();
                    namespaceStack.pop_back();
                }
            }
            return result;
        }

        std::string CppHlslParser::getNamespaceNodeFullIdentifier(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const std::string& subNamespaceIdentifier) const noexcept
        {
            std::string result = getNamespaceNodeFullIdentifier(namespaceNode);
            result += "::";
            result += subNamespaceIdentifier;
            return result;
        }

        TreeNodeAccessor<SyntaxTreeItem> CppHlslParser::findNamespaceNodeInternal(const TreeNodeAccessor<SyntaxTreeItem>& parentNamespaceNode, const std::string& namespaceIdentifier) const noexcept
        {
            const uint32 childNamespaceNodeCount = parentNamespaceNode.getChildNodeCount();
            for (uint32 childIndex = 0; childIndex < childNamespaceNodeCount; ++childIndex)
            {
                TreeNodeAccessor childNode = parentNamespaceNode.getChildNode(childIndex);
                if (isNamespaceNode(childNode) == true && childNode.getNodeData()._symbolTableItem._symbolString == namespaceIdentifier)
                {
                    return parentNamespaceNode.getChildNode(childIndex);
                }
            }
            return TreeNodeAccessor<SyntaxTreeItem>();
        }

        const bool CppHlslParser::isNamespaceNode(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode) const noexcept
        {
            return (namespaceNode.isValid() == true) && (namespaceNode.getNodeData().getSyntaxClassifier() == CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Namespace);
        }

        void CppHlslParser::registerTypeTemplate(const std::string& typeFullIdentifier, const uint32 typeSize)
        {
            registerTypeTemplateInternal(false, typeFullIdentifier, typeSize);
        }

        void CppHlslParser::registerTypeTemplateInternal(const bool isBuiltIn, const std::string& typeFullIdentifier, const uint32 typeSize)
        {
            fs::Vector<std::string> typeStack;
            fs::StringUtil::tokenize(typeFullIdentifier, "::", typeStack);

            std::string typeFullIdentifier_;
            {
                typeFullIdentifier_ = kGlobalNamespaceSymbol._symbolString;
                typeFullIdentifier_ += "::";
                typeFullIdentifier_ += typeFullIdentifier;
            }

            auto found = _typeTableUmap.find(typeFullIdentifier_);
            if (found.isValid() == false)
            {
                _typeTable.push_back(CppHlslTypeTableItem(SymbolTableItem(SymbolClassifier::Identifier, typeStack.back()), CppHlslUserDefinedTypeInfo::Default));
                _typeTable.back().setTypeSize(typeSize);

                const uint32 typeTableIndex = _typeTable.size() - 1;
                if (isBuiltIn == true)
                {
                    _builtInTypeUmap.insert(typeStack.back(), typeTableIndex);
                }
                _typeTableUmap.insert(typeFullIdentifier_, typeTableIndex);
            }
        }

        const uint32 CppHlslParser::registerType(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const CppHlslTypeTableItem& type)
        {
            FS_ASSERT("김장원", isNamespaceNode(namespaceNode) == true, "namespaceNode 가 잘못됐습니다!");

            const std::string typeFullIdentifier = getTypeFullIdentifier(namespaceNode, type.getTypeName());
            auto found = _typeTableUmap.find(typeFullIdentifier);
            if (found.isValid() == false)
            {
                _typeTable.push_back(type);
                const uint32 typeTableIndex = _typeTable.size() - 1;
                _typeTableUmap.insert(typeFullIdentifier, typeTableIndex);
                return typeTableIndex;
            }
            return *found._value;
        }

        std::string CppHlslParser::getTypeFullIdentifier(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const std::string& typeIdentifier) const noexcept
        {
            std::string result = getNamespaceNodeFullIdentifier(namespaceNode);
            result += "::";
            result += typeIdentifier;
            return result;
        }

        std::string CppHlslParser::getTypeInfoIdentifierXXX(const std::string& typeFullIdentifier) const noexcept
        {
            return typeFullIdentifier.substr(kGlobalNamespaceSymbol._symbolString.length() + 2);
        }

        std::string CppHlslParser::extractPureTypeName(const std::string& typeFullIdentifier) noexcept
        {
            const size_t lastColonPosition = typeFullIdentifier.find_last_of(':');
            if (lastColonPosition != std::string::npos)
            {
                return typeFullIdentifier.substr(lastColonPosition + 1);
            }
            return typeFullIdentifier;
        }

        const bool CppHlslParser::registerTypeAlias(const std::string& typeAlias, const uint32 typeIndex)
        {
            auto found = _typeAliasTableUmap.find(typeAlias);
            if (found.isValid() == true)
            {
                reportError(kInvalidGrammarSymbol, ErrorType::RepetitionOfCode, (typeAlias + "는 이미 alias 가 등록되어 있습니다!").c_str());
                return false;
            }
            _typeAliasTableUmap.insert(typeAlias, typeIndex);
            return true;
        }

        const bool CppHlslParser::isSymbolType(const SymbolTableItem& typeSymbol) const noexcept
        {
            bool foundType = false;
            TreeNodeAccessor namespaceNode = _currentScopeNamespaceNode;
            while (isNamespaceNode(namespaceNode) == true)
            {
                if (isSymbolTypeInternal(namespaceNode, typeSymbol) == false)
                {
                    namespaceNode = namespaceNode.getParentNode();
                }
                else
                {
                    foundType = true;
                    break;
                }
            }
            return foundType;
        }

        const bool CppHlslParser::isSymbolTypeInternal(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const SymbolTableItem& typeSymbol) const noexcept
        {
            const std::string& unaliasedSymbolString = getUnaliasedSymbolStringXXX(typeSymbol);
            if (isBuiltInTypeXXX(unaliasedSymbolString) == true)
            {
                return true;
            }
            const std::string typeFullIdentifier = getTypeFullIdentifier(namespaceNode, unaliasedSymbolString);
            return isIdentifierType(typeFullIdentifier);
        }

        const bool CppHlslParser::isIdentifierType(const std::string& typeFullIdentifier) const noexcept
        {
            if (isUserDefinedTypeXXX(typeFullIdentifier) == true)
            {
                return true;
            }
            return false;
        }

        const bool CppHlslParser::isBuiltInTypeXXX(const std::string& symbolString) const noexcept
        {
            return (_builtInTypeUmap.find(symbolString).isValid() == true);
        }

        const bool CppHlslParser::isUserDefinedTypeXXX(const std::string& typeFullIdentifier) const noexcept
        {
            return (_typeTableUmap.find(typeFullIdentifier).isValid() == true);
        }

        const std::string& CppHlslParser::getUnaliasedSymbolStringXXX(const SymbolTableItem& symbol) const noexcept
        {
            auto found = _typeAliasTableUmap.find(symbol._symbolString);
            if (found.isValid() == true)
            {
                return _typeTable[*found._value].getTypeName();
            }
            return symbol._symbolString;
        }

        const CppHlslTypeOf CppHlslParser::getTypeOfSymbol(const TreeNodeAccessor<SyntaxTreeItem>& namespaceNode, const SymbolTableItem& symbol) const noexcept
        {
            const CppHlslSyntaxClassifier literalSyntaxClassifier = convertLiteralSymbolToSyntax(symbol);
            if (literalSyntaxClassifier != CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_INVALID)
            {
                return CppHlslTypeOf::LiteralType;
            }

            const std::string& unaliasedSymbolString = getUnaliasedSymbolStringXXX(symbol);
            if (isBuiltInTypeXXX(unaliasedSymbolString) == true)
            {
                return CppHlslTypeOf::BuiltInType;
            }

            const std::string typeFullIdentifier = getTypeFullIdentifier(namespaceNode, unaliasedSymbolString);
            if (isUserDefinedTypeXXX(typeFullIdentifier) == true)
            {
                return CppHlslTypeOf::UserDefinedType;
            }

            return CppHlslTypeOf::INVALID;
        }

        const CppHlslTypeTableItem& CppHlslParser::getType(const std::string& typeFullIdentifier) const noexcept
        {
            return _typeTable[_typeTableUmap.at(typeFullIdentifier)];
        }

        const CppHlslTypeInfo& CppHlslParser::getTypeInfo(const uint32 typeIndex) const noexcept
        {
            if (typeIndex < _typeInfoArray.size())
            {
                return _typeInfoArray[typeIndex];
            }
            return CppHlslTypeInfo::kInvalidTypeInfo;
        }

        const CppHlslTypeInfo& CppHlslParser::getTypeInfo(const std::string& typeName) const noexcept
        {
            auto found = _typeInfoUmap.find(typeName);
            if (found.isValid() == false)
            {
                return CppHlslTypeInfo::kInvalidTypeInfo;
            }
            return _typeInfoArray[*found._value];
        }

        const uint32 CppHlslParser::getTypeInfoCount() const noexcept
        {
            return static_cast<uint32>(_typeInfoArray.size());
        }

        std::string CppHlslParser::convertDeclarationNameToHlslSemanticName(const std::string& declarationName)
        {
            std::string semanticName = declarationName.substr(1);
            const uint32 semanticNameLength = static_cast<uint32>(semanticName.length());
            for (uint32 semanticNameIter = 0; semanticNameIter < semanticNameLength; ++semanticNameIter)
            {
                semanticName[semanticNameIter] = ::toupper(semanticName[semanticNameIter]);
            }
            return semanticName;
        }

        const DXGI_FORMAT CppHlslParser::convertCppHlslTypeToDxgiFormat(const CppHlslTypeInfo& typeInfo)
        {
            const std::string& typeName = typeInfo.getTypeName();
            if (typeName == "float" || typeName == "float1")
            {
                return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
            }
            else if (typeName == "float2")
            {
                return DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
            }
            else if (typeName == "float3")
            {
                return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
            }
            else if (typeName == "float4")
            {
                return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
            }
            else if (typeName == "uint" || typeName == "uint1")
            {
                return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
            }
            else if (typeName == "uint2")
            {
                return DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT;
            }
            else if (typeName == "uint3")
            {
                return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT;
            }
            else if (typeName == "uint4")
            {
                return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT;
            }

            return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
        }

        std::string CppHlslParser::serializeCppHlslTypeToHlslStreamDatum(const CppHlslTypeInfo& typeInfo)
        {
            std::string result;

            result.append("struct ");
            std::string typeName = extractPureTypeName(typeInfo.getTypeName());
            result.append(typeName);
            result.append("\n{\n");

            const uint32 memberCount = typeInfo.getMemberCount();
            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                const CppHlslTypeInfo& memberType = typeInfo.getMember(memberIndex);
                result.append("\t");
                result.append(memberType.getTypeName());
                result.append(" ");
                result.append(memberType.getDeclName());
                result.append(" : ");
                if (memberType.getSemanticName().empty() == true)
                {
                    result.append(convertDeclarationNameToHlslSemanticName(memberType.getDeclName()));
                }
                else
                {
                    result.append(memberType.getSemanticName());
                }
                result.append(";\n");
            }
            result.append("};\n\n");
            return result;
        }

        std::string CppHlslParser::serializeCppHlslTypeToHlslConstantBuffer(const CppHlslTypeInfo& typeInfo, const uint32 bufferIndex)
        {
            std::string result;

            result.append("cbuffer ");
            std::string typeName = extractPureTypeName(typeInfo.getTypeName());
            result.append(typeName);
            result.append(" : register(");
            result.append("b" + std::to_string((typeInfo.isRegisterIndexValid() == true) ? typeInfo.getRegisterIndex() : bufferIndex));
            result.append(")\n{\n");

            const uint32 memberCount = typeInfo.getMemberCount();
            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                const CppHlslTypeInfo& memberType = typeInfo.getMember(memberIndex);
                result.append("\t");
                result.append(memberType.getTypeName());
                result.append(" ");
                result.append(memberType.getDeclName());
                result.append(";\n");
            }
            result.append("};\n\n");
            return result;
        }

        std::string CppHlslParser::serializeCppHlslTypeToHlslStructuredBufferDefinition(const CppHlslTypeInfo& typeInfo)
        {
            std::string result;

            std::string typeName = extractPureTypeName(typeInfo.getTypeName());
            result.append("struct ");
            result.append(typeName);
            result.append("\n{\n");
            const uint32 memberCount = typeInfo.getMemberCount();
            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                const CppHlslTypeInfo& memberType = typeInfo.getMember(memberIndex);
                result.append("\t");
                result.append(memberType.getTypeName());
                result.append(" ");
                result.append(memberType.getDeclName());
                result.append(";\n");
            }

            result.append("};\n\n");
            return result;
        }
    }
}
