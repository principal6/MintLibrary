#include <stdafx.h>
#include <MintRenderingBase/Include/CppHlsl/Parser.h>

#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/Tree.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintLanguage/Include/ILexer.h>

#include <functional>


namespace mint
{
    namespace Language
    {
        namespace CppHlsl
        {
            namespace TypeUtils
            {
                std::string extractPureTypeName(const std::string& typeFullName) noexcept
                {
                    const size_t found = typeFullName.find_last_of("::");
                    if (std::string::npos != found)
                    {
                        return typeFullName.substr(found + 1);
                    }
                    return typeFullName;
                }
            }        


            Parser::Parser(ILexer& lexer)
                : IParser(lexer)
            {
                registerTypeInternal("void"       ,  0, true);
                registerTypeInternal("bool"       ,  4, true); // TypeSize = 4 임에 주의!!!
                registerTypeInternal("int"        ,  4, true);
                registerTypeInternal("int1"       ,  4, true);
                registerTypeInternal("int2"       ,  8, true);
                registerTypeInternal("int3"       , 12, true);
                registerTypeInternal("int4"       , 16, true);
                registerTypeInternal("uint"       ,  4, true);
                registerTypeInternal("uint1"      ,  4, true);
                registerTypeInternal("uint2"      ,  8, true);
                registerTypeInternal("uint3"      , 12, true);
                registerTypeInternal("uint4"      , 16, true);
                registerTypeInternal("float"      ,  4, true);
                registerTypeInternal("float1"     ,  4, true);
                registerTypeInternal("float2"     ,  8, true);
                registerTypeInternal("float3"     , 12, true);
                registerTypeInternal("float4"     , 16, true);
                registerTypeInternal("float4x4"   , 64, true);
            }

            Parser::~Parser()
            {
                __noop;
            }

            const bool Parser::execute()
            {
                reset();

                uint32 advanceCount = 0;
                SyntaxTreeNodeData rootItem;
                rootItem._classifier = SyntaxClassifier::ROOT;
                rootItem._identifier = "ROOT";
                SyntaxTreeNode syntaxTreeRootNode = _syntaxTree.createRootNode(rootItem);
                while (continueParsing() == true)
                {
                    if (parseCode(getSymbolPosition(), syntaxTreeRootNode, advanceCount) == false)
                    {
                        break;
                    }

                    advanceSymbolPositionXXX(advanceCount);

                    advanceCount = 0;
                }

                if (hasReportedErrors() == true)
                {
                    MINT_LOG_ERROR("김장원", "에러가 있었습니다!!!");

                    return false;
                }

                return true;
            }

            const bool Parser::parseCode(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
            {
                if (_symbolTable[symbolPosition]._symbolClassifier == SymbolClassifier::Keyword)
                {
                    if (_symbolTable[symbolPosition]._symbolString == "namespace")
                    {
                        return parseNamespace(symbolPosition, currentNode, outAdvanceCount);
                    }
                    else if (_symbolTable[symbolPosition]._symbolString == "struct")
                    {
                        return parseStruct(symbolPosition, currentNode, outAdvanceCount);
                    }
                }
                else if (_symbolTable[symbolPosition]._symbolClassifier == SymbolClassifier::StatementTerminator)
                {
                    outAdvanceCount = 1;
                    return true;
                }
                else if (_symbolTable[symbolPosition]._symbolClassifier == SymbolClassifier::Grouper_Close)
                {
                    currentNode = currentNode.getParentNode();
                    outAdvanceCount = 1;
                    return true;
                }
                return false;
            }

            const bool Parser::parseNamespace(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
            {
                MINT_ASSURE(hasSymbol(symbolPosition + 3));
                MINT_ASSURE(_symbolTable[symbolPosition + 1]._symbolClassifier == SymbolClassifier::Identifier);
                MINT_ASSURE(IParser::findNextDepthMatchingGrouperCloseSymbol(symbolPosition + 2));

                SyntaxTreeNodeData syntaxTreeItem;
                syntaxTreeItem._classifier = SyntaxClassifier::Namespace;
                syntaxTreeItem._identifier = _symbolTable[symbolPosition + 1]._symbolString;
                SyntaxTreeNode newNode = currentNode.insertChildNode(syntaxTreeItem);
                currentNode = newNode;
            
                outAdvanceCount += 2 + 1;
                return true;
            }

            const bool Parser::parseStruct(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
            {
                MINT_ASSURE(hasSymbol(symbolPosition + 4)); // ; 까지!
                MINT_ASSURE(_symbolTable[symbolPosition + 1]._symbolClassifier == SymbolClassifier::Identifier);

                bool hasCustomSyntax = false;
                if (_symbolTable[symbolPosition + 2]._symbolString == "{")
                {
                    MINT_ASSURE(IParser::findNextDepthMatchingGrouperCloseSymbol(symbolPosition + 2));
                }
                else
                {
                    hasCustomSyntax = true;
                }

                SyntaxTreeNodeData syntaxTreeItem;
                syntaxTreeItem._classifier = SyntaxClassifier::Struct;
                syntaxTreeItem._identifier = _symbolTable[symbolPosition + 1]._symbolString;
                SyntaxTreeNode newNode = currentNode.insertChildNode(syntaxTreeItem);
                currentNode = newNode;
                outAdvanceCount += 2 + 1;

                if (hasCustomSyntax == true)
                {
                    MINT_ASSURE(parseCustomSyntax(symbolPosition + 2, currentNode, outAdvanceCount) == true);
                }

                // Member
                while (parseStructMember(symbolPosition + outAdvanceCount, currentNode, outAdvanceCount) == true)
                {
                    __noop;
                }

                buildTypeMetaData(newNode);
                return true;
            }

            const bool Parser::parseStructMember(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
            {
                const uint32 kSemicolonMinOffset = 2;
                MINT_ASSURE_SILENT(_symbolTable[symbolPosition]._symbolClassifier != SymbolClassifier::Grouper_Close); // 최우선 검사
                MINT_ASSURE(hasSymbol(symbolPosition + kSemicolonMinOffset));

                SyntaxTreeNodeData syntaxTreeItem;
                syntaxTreeItem._classifier = SyntaxClassifier::Variable;
                syntaxTreeItem._identifier = _symbolTable[symbolPosition + 1]._symbolString;
                SyntaxTreeNode newNode = currentNode.insertChildNode(syntaxTreeItem);
                {
                    // DataType 은 Variable Identifier 노드의 자식!
                    SyntaxTreeNodeData syntaxTreeItemChild;
                    syntaxTreeItemChild._classifier = SyntaxClassifier::DataType;
                    syntaxTreeItemChild._identifier = _symbolTable[symbolPosition]._symbolString;
                    newNode.insertChildNode(syntaxTreeItemChild);

                    if (_symbolTable[symbolPosition + kSemicolonMinOffset]._symbolString == "{")
                    {
                        // 초기화! HLSL 에서 멤버 초기화를 하지 않으므로 패스!
                        outAdvanceCount += 3;
                    }
                    else if (_symbolTable[symbolPosition + kSemicolonMinOffset]._symbolClassifier == SymbolClassifier::Identifier)
                    {
                        // Custom syntax
                        MINT_ASSURE(parseCustomSyntax(symbolPosition + kSemicolonMinOffset, newNode, outAdvanceCount) == true);
                    }
                }
                //currentNode = newNode;

                outAdvanceCount += 2 + 1;
                return true;
            }

            const bool Parser::parseCustomSyntax(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
            {
                const uint32 kCloseParenthesisMinOffset = 3;
                MINT_ASSURE(hasSymbol(symbolPosition + kCloseParenthesisMinOffset));
                MINT_ASSURE(findNextDepthMatchingGrouperCloseSymbol(symbolPosition + 1) == true);

                SyntaxTreeNodeData syntaxTreeItem;
                if (_symbolTable[symbolPosition]._symbolString == "CPP_HLSL_SEMANTIC_NAME")
                {
                    syntaxTreeItem._classifier = SyntaxClassifier::SemanticName;
                    syntaxTreeItem._identifier = _symbolTable[symbolPosition + 2]._symbolString;
                    SyntaxTreeNode newNode = currentNode.insertChildNode(syntaxTreeItem);
                    outAdvanceCount += 3 + 1;
                }
                else if (_symbolTable[symbolPosition]._symbolString == "CPP_HLSL_REGISTER_INDEX")
                {
                    syntaxTreeItem._classifier = SyntaxClassifier::RegisterIndex;
                    syntaxTreeItem._value = _symbolTable[symbolPosition + 2]._symbolString;
                    SyntaxTreeNode newNode = currentNode.insertChildNode(syntaxTreeItem);
                    outAdvanceCount += 3 + 1;
                }
                else if (_symbolTable[symbolPosition]._symbolString == "CPP_HLSL_INSTANCE_DATA")
                {
                    syntaxTreeItem._classifier = SyntaxClassifier::InstanceData;
                    syntaxTreeItem._value = _symbolTable[symbolPosition + 2]._symbolString;
                    SyntaxTreeNode newNode = currentNode.insertChildNode(syntaxTreeItem);
                    outAdvanceCount += 3 + 1;
                }
                else
                {
                    MINT_NEVER;
                }

                return true;
            }

            void Parser::buildTypeMetaData(const SyntaxTreeNode& structNode) noexcept
            {
                Vector<std::string> namespaceStack;
                SyntaxTreeNode parentNode = structNode.getParentNode();
                while (parentNode.isValid() == true)
                {
                    if (parentNode.getNodeData()._classifier == SyntaxClassifier::ROOT)
                    {
                        break;
                    }

                    namespaceStack.push_back(parentNode.getNodeData()._identifier);
                    parentNode = parentNode.getParentNode();
                }

                std::string fullTypeName;
                while (namespaceStack.empty() == false)
                {
                    fullTypeName += namespaceStack.back();
                    fullTypeName += "::";
                    namespaceStack.pop_back();
                }
                const SyntaxTreeNodeData& structNodeSyntaxTreeItem = structNode.getNodeData();
                fullTypeName += structNodeSyntaxTreeItem._identifier;

                KeyValuePair found = _typeMetaDataMap.find(fullTypeName);
                if (found.isValid() == true)
                {
                    return;
                }

                TypeMetaData<TypeCustomData> typeMetaData;
                typeMetaData.setBaseData(fullTypeName, false);
            
                uint32 structSize = 0;
                std::string streamDataTypeNameForSlots;
                const int32 inputSlot = getSlottedStreamDataInputSlot(fullTypeName, streamDataTypeNameForSlots);
                const uint32 childNodeCount = structNode.getChildNodeCount();
                for (uint32 childNodeIndex = 0; childNodeIndex < childNodeCount; ++childNodeIndex)
                {
                    SyntaxTreeNode childNode = structNode.getChildNode(childNodeIndex);
                    const SyntaxTreeNodeData& childNodeData = childNode.getNodeData();
                    if (childNodeData._classifier == SyntaxClassifier::Variable)
                    {
                        const uint32 attributeCount = childNode.getChildNodeCount();
                        SyntaxTreeNode dataTypeNode = childNode.getChildNode(0);
                        TypeMetaData<TypeCustomData> memberTypeMetaData = getTypeMetaData(dataTypeNode.getNodeData()._identifier);
                        memberTypeMetaData.setByteOffset(structSize);
                        structSize += memberTypeMetaData.getSize();
                        memberTypeMetaData.setDeclName(childNodeData._identifier);
                        memberTypeMetaData._customData.setInputSlot(inputSlot);

                        if (2 <= attributeCount)
                        {
                            // SemanticName
                            SyntaxTreeNode attribute1 = childNode.getChildNode(1);
                            const SyntaxTreeNodeData& attribute1Data = attribute1.getNodeData();
                            if (attribute1Data._classifier == SyntaxClassifier::SemanticName)
                            {
                                memberTypeMetaData._customData.setSemanticName(attribute1Data._identifier);
                            }
                            else
                            {
                                MINT_NEVER;
                            }
                        }

                        typeMetaData.pushMember(memberTypeMetaData);
                    }
                    else if (childNodeData._classifier == SyntaxClassifier::RegisterIndex)
                    {
                        const int32 registerIndex = std::stoi(childNodeData._value);
                        typeMetaData._customData.setRegisterIndex(registerIndex);
                    }
                    else if (childNodeData._classifier == SyntaxClassifier::InstanceData)
                    {
                        const int32 instanceDataStepRate = std::stoi(childNodeData._value);
                        typeMetaData._customData.setInstanceDataStepRate(instanceDataStepRate);
                    }
                    else
                    {
                        MINT_NEVER;
                    }
                }
                typeMetaData.setSize(structSize);
            
                pushTypeMetaData(fullTypeName, typeMetaData);

                if (0 < inputSlot)
                {
                    TypeMetaData<TypeCustomData>& streamDataForSlots = accessTypeMetaData(streamDataTypeNameForSlots);
                    streamDataForSlots._customData.pushSlottedStreamData(typeMetaData);
                }
            }

            std::string Parser::convertDeclarationNameToHlslSemanticName(const std::string& declarationName)
            {
                if (declarationName.empty() == true)
                {
                    MINT_NEVER;
                }

                std::string semanticName = declarationName.substr(1);
                const uint32 semanticNameLength = static_cast<uint32>(semanticName.length());
                for (uint32 semanticNameIter = 0; semanticNameIter < semanticNameLength; ++semanticNameIter)
                {
                    semanticName[semanticNameIter] = ::toupper(semanticName[semanticNameIter]);
                }
                return semanticName;
            }

            const int32 Parser::getSlottedStreamDataInputSlot(const std::string& typeName, std::string& streamDataTypeName) const noexcept
            {
                // 0 은 자기 자신을 의미하므로
                // 실제 Slotted StreamData 라면 InputSlot 은 1부터 시작한다!

                const bool isBackDigit = ::isdigit(typeName.back());
                if (isBackDigit == false)
                {
                    return 0;
                }

                const int32 digitCount = (::isdigit(typeName[typeName.size() - 2]) != 0) ? 2 : 1;
                const int32 inputSlot = std::stoi(typeName.substr(typeName.size() - digitCount, digitCount));
                streamDataTypeName = typeName.substr(0, typeName.size() - digitCount);
                if (existsTypeMetaData(streamDataTypeName) == true)
                {
                    return inputSlot;
                }
                return 0;
            }

            const DXGI_FORMAT Parser::convertCppHlslTypeToDxgiFormat(const TypeMetaData<TypeCustomData>& typeMetaData)
            {
                const std::string& typeName = typeMetaData.getTypeName();
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

            std::string Parser::serializeCppHlslTypeToHlslStreamDatum(const TypeMetaData<TypeCustomData>& typeMetaData)
            {
                std::string pureTypeName = TypeUtils::extractPureTypeName(typeMetaData.getTypeName());

                // inputSlot 0 은 나 자신이다!
                Vector<TypeMetaData<TypeCustomData>> slottedDatas;
                for (int32 inputSlot = 1; inputSlot < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT; ++inputSlot)
                {
                    std::string typeName = typeMetaData.getTypeName() + std::to_string(inputSlot);
                    if (false == existsTypeMetaData(typeName))
                    {
                        break;
                    }

                    slottedDatas.push_back(getTypeMetaData(typeName));
                }
            
                std::string result;
                result.append("struct ");
                result.append(pureTypeName);
                result.append("\n{\n");
                result.append(serializeCppHlslTypeToHlslStreamDatumMembers(typeMetaData));

                const uint32 slottedDataCount = slottedDatas.size();
                for (uint32 slottedDataIndex = 0; slottedDataIndex < slottedDataCount; ++slottedDataIndex)
                {
                    result += serializeCppHlslTypeToHlslStreamDatumMembers(slottedDatas[slottedDataIndex]);
                }

                result.append("};\n\n");
                return result;
            }

            std::string Parser::serializeCppHlslTypeToHlslStreamDatumMembers(const TypeMetaData<TypeCustomData>& typeMetaData)
            {
                std::string result;
                const uint32 memberCount = typeMetaData.getMemberCount();
                for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
                {
                    const TypeMetaData<TypeCustomData>& memberType = typeMetaData.getMember(memberIndex);
                    result.append("\t");
                    result.append(memberType.getTypeName());
                    result.append(" ");
                    result.append(memberType.getDeclName());
                    result.append(" : ");
                    if (memberType._customData.getSemanticName().empty() == true)
                    {
                        result.append(convertDeclarationNameToHlslSemanticName(memberType.getDeclName()));
                    }
                    else
                    {
                        result.append(memberType._customData.getSemanticName());
                    }
                    result.append(";\n");
                }
                return result;
            }

            std::string Parser::serializeCppHlslTypeToHlslConstantBuffer(const TypeMetaData<TypeCustomData>& typeMetaData, const uint32 bufferIndex)
            {
                std::string result;

                result.append("cbuffer ");
                std::string pureTypeName = TypeUtils::extractPureTypeName(typeMetaData.getTypeName());
                result.append(pureTypeName);
                result.append(" : register(");
                result.append("b" + std::to_string((typeMetaData._customData.isRegisterIndexValid() == true) ? typeMetaData._customData.getRegisterIndex() : bufferIndex));
                result.append(")\n{\n");

                const uint32 memberCount = typeMetaData.getMemberCount();
                for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
                {
                    const TypeMetaData<TypeCustomData>& memberType = typeMetaData.getMember(memberIndex);
                    result.append("\t");
                    result.append(memberType.getTypeName());
                    result.append(" ");
                    result.append(memberType.getDeclName());
                    result.append(";\n");
                }
                result.append("};\n\n");
                return result;
            }

            std::string Parser::serializeCppHlslTypeToHlslStructuredBufferDefinition(const TypeMetaData<TypeCustomData>& typeMetaData)
            {
                std::string result;

                std::string pureTypeName = TypeUtils::extractPureTypeName(typeMetaData.getTypeName());
                result.append("struct ");
                result.append(pureTypeName);
                result.append("\n{\n");
                const uint32 memberCount = typeMetaData.getMemberCount();
                for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
                {
                    const TypeMetaData<TypeCustomData>& memberType = typeMetaData.getMember(memberIndex);
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
}
