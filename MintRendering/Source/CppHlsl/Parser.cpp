﻿#include <MintRendering/Include/CppHlsl/Parser.h>

#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/Tree.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintLanguage/Include/ILexer.h>

#include <MintRendering/Include/RenderingBaseCommon.h>

#include <functional>


namespace mint
{
	namespace Language
	{
		namespace CppHlsl
		{
			namespace TypeUtils
			{
				StringA extractPureTypeName(const StringA& typeFullName) noexcept
				{
					const uint32 found = StringUtil::FindLastOf(typeFullName.CString(), "::");
					if (found != kStringNPos)
					{
						return typeFullName.Substring(found + 1);
					}
					return typeFullName;
				}
			}


			Parser::Parser(ILexer& lexer)
				: IParser(lexer)
			{
				RegisterTypeInternal("void", 0, true);
				RegisterTypeInternal("bool", 4, true); // TypeSize = 4 임에 주의!!!
				RegisterTypeInternal("int", 4, true);
				RegisterTypeInternal("int1", 4, true);
				RegisterTypeInternal("int2", 8, true);
				RegisterTypeInternal("int3", 12, true);
				RegisterTypeInternal("int4", 16, true);
				RegisterTypeInternal("uint", 4, true);
				RegisterTypeInternal("uint1", 4, true);
				RegisterTypeInternal("uint2", 8, true);
				RegisterTypeInternal("uint3", 12, true);
				RegisterTypeInternal("uint4", 16, true);
				RegisterTypeInternal("float", 4, true);
				RegisterTypeInternal("float1", 4, true);
				RegisterTypeInternal("float2", 8, true);
				RegisterTypeInternal("float3", 12, true);
				RegisterTypeInternal("float4", 16, true);
				RegisterTypeInternal("float4x4", 64, true);
			}

			Parser::~Parser()
			{
				__noop;
			}

			bool Parser::Execute()
			{
				Reset();

				uint32 advanceCount = 0;
				SyntaxTreeNodeData rootItem;
				rootItem._classifier = SyntaxClassifier::ROOT;
				rootItem._Identifier = "ROOT";
				SyntaxTreeNode syntaxTreeRootNode = _syntaxTree.CreateRootNode(rootItem);
				while (ContinuesParsing() == true)
				{
					if (ParseCode(GetSymbolPosition(), syntaxTreeRootNode, advanceCount) == false)
					{
						break;
					}

					AdvanceSymbolPositionXXX(advanceCount);

					advanceCount = 0;
				}

				if (HasReportedErrors() == true)
				{
					MINT_LOG_ERROR("에러가 있었습니다!!!");

					return false;
				}

				return true;
			}

			bool Parser::ParseCode(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
			{
				if (_symbolTable[symbolPosition]._symbolClassifier == SymbolClassifier::Keyword)
				{
					if (_symbolTable[symbolPosition]._symbolString == "namespace")
					{
						return ParseNamespace(symbolPosition, currentNode, outAdvanceCount);
					}
					else if (_symbolTable[symbolPosition]._symbolString == "struct")
					{
						return ParseStruct(symbolPosition, currentNode, outAdvanceCount);
					}
				}
				else if (_symbolTable[symbolPosition]._symbolClassifier == SymbolClassifier::StatementTerminator)
				{
					outAdvanceCount = 1;
					return true;
				}
				else if (_symbolTable[symbolPosition]._symbolClassifier == SymbolClassifier::Grouper_Close)
				{
					currentNode = currentNode.GetParentNode();
					outAdvanceCount = 1;
					return true;
				}
				return false;
			}

			bool Parser::ParseNamespace(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
			{
				MINT_ASSURE(HasSymbol(symbolPosition + 3));
				MINT_ASSURE(_symbolTable[symbolPosition + 1]._symbolClassifier == SymbolClassifier::Identifier);
				MINT_ASSURE(IParser::FindNextDepthMatchingGrouperCloseSymbol(symbolPosition + 2));

				SyntaxTreeNodeData syntaxTreeItem;
				syntaxTreeItem._classifier = SyntaxClassifier::Namespace;
				syntaxTreeItem._Identifier = _symbolTable[symbolPosition + 1]._symbolString;
				SyntaxTreeNode newNode = currentNode.InsertChildNode(syntaxTreeItem);
				currentNode = newNode;

				outAdvanceCount += 2 + 1;
				return true;
			}

			bool Parser::ParseStruct(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
			{
				MINT_ASSURE(HasSymbol(symbolPosition + 4)); // ; 까지!
				MINT_ASSURE(_symbolTable[symbolPosition + 1]._symbolClassifier == SymbolClassifier::Identifier);

				bool hasCustomSyntax = false;
				if (_symbolTable[symbolPosition + 2]._symbolString == "{")
				{
					MINT_ASSURE(IParser::FindNextDepthMatchingGrouperCloseSymbol(symbolPosition + 2));
				}
				else
				{
					hasCustomSyntax = true;
				}

				SyntaxTreeNodeData syntaxTreeItem;
				syntaxTreeItem._classifier = SyntaxClassifier::Struct;
				syntaxTreeItem._Identifier = _symbolTable[symbolPosition + 1]._symbolString;
				SyntaxTreeNode newNode = currentNode.InsertChildNode(syntaxTreeItem);
				currentNode = newNode;
				outAdvanceCount += 2 + 1;

				if (hasCustomSyntax == true)
				{
					MINT_ASSURE(ParseCustomSyntax(symbolPosition + 2, currentNode, outAdvanceCount) == true);
				}

				// Member
				while (ParseStructMember(symbolPosition + outAdvanceCount, currentNode, outAdvanceCount) == true)
				{
					__noop;
				}

				BuildTypeMetaData(newNode);
				return true;
			}

			bool Parser::ParseStructMember(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
			{
				const uint32 kSemicolonMinOffset = 2;
				MINT_ASSURE_SILENT(_symbolTable[symbolPosition]._symbolClassifier != SymbolClassifier::Grouper_Close); // 최우선 검사
				MINT_ASSURE(HasSymbol(symbolPosition + kSemicolonMinOffset));

				SyntaxTreeNodeData syntaxTreeItem;
				syntaxTreeItem._classifier = SyntaxClassifier::Variable;
				syntaxTreeItem._Identifier = _symbolTable[symbolPosition + 1]._symbolString;
				SyntaxTreeNode newNode = currentNode.InsertChildNode(syntaxTreeItem);
				{
					// DataType 은 Variable Identifier 노드의 자식!
					SyntaxTreeNodeData syntaxTreeItemChild;
					syntaxTreeItemChild._classifier = SyntaxClassifier::DataType;
					syntaxTreeItemChild._Identifier = _symbolTable[symbolPosition]._symbolString;
					newNode.InsertChildNode(syntaxTreeItemChild);

					if (_symbolTable[symbolPosition + kSemicolonMinOffset]._symbolString == "{")
					{
						// 초기화! HLSL 에서 멤버 초기화를 하지 않으므로 패스!
						outAdvanceCount += 3;
					}
					else if (_symbolTable[symbolPosition + kSemicolonMinOffset]._symbolClassifier == SymbolClassifier::Identifier)
					{
						// Custom syntax
						MINT_ASSURE(ParseCustomSyntax(symbolPosition + kSemicolonMinOffset, newNode, outAdvanceCount) == true);
					}
				}
				//currentNode = newNode;

				outAdvanceCount += 2 + 1;
				return true;
			}

			bool Parser::ParseCustomSyntax(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept
			{
				const uint32 kCloseParenthesisMinOffset = 3;
				MINT_ASSURE(HasSymbol(symbolPosition + kCloseParenthesisMinOffset));
				MINT_ASSURE(FindNextDepthMatchingGrouperCloseSymbol(symbolPosition + 1) == true);

				SyntaxTreeNodeData syntaxTreeItem;
				if (_symbolTable[symbolPosition]._symbolString == "CPP_HLSL_SEMANTIC_NAME")
				{
					syntaxTreeItem._classifier = SyntaxClassifier::SemanticName;
					syntaxTreeItem._Identifier = _symbolTable[symbolPosition + 2]._symbolString;
					SyntaxTreeNode newNode = currentNode.InsertChildNode(syntaxTreeItem);
					outAdvanceCount += 3 + 1;
				}
				else if (_symbolTable[symbolPosition]._symbolString == "CPP_HLSL_REGISTER_INDEX")
				{
					syntaxTreeItem._classifier = SyntaxClassifier::RegisterIndex;
					syntaxTreeItem._value = _symbolTable[symbolPosition + 2]._symbolString;
					SyntaxTreeNode newNode = currentNode.InsertChildNode(syntaxTreeItem);
					outAdvanceCount += 3 + 1;
				}
				else if (_symbolTable[symbolPosition]._symbolString == "CPP_HLSL_INSTANCE_DATA")
				{
					syntaxTreeItem._classifier = SyntaxClassifier::InstanceData;
					syntaxTreeItem._value = _symbolTable[symbolPosition + 2]._symbolString;
					SyntaxTreeNode newNode = currentNode.InsertChildNode(syntaxTreeItem);
					outAdvanceCount += 3 + 1;
				}
				else
				{
					MINT_NEVER;
				}

				return true;
			}

			void Parser::BuildTypeMetaData(const SyntaxTreeNode& structNode) noexcept
			{
				Vector<StringA> namespaceStack;
				SyntaxTreeNode parentNode = structNode.GetParentNode();
				while (parentNode.IsValid() == true)
				{
					if (parentNode.GetNodeData()._classifier == SyntaxClassifier::ROOT)
					{
						break;
					}

					namespaceStack.PushBack(parentNode.GetNodeData()._Identifier);
					parentNode = parentNode.GetParentNode();
				}

				StringA fullTypeName;
				while (namespaceStack.IsEmpty() == false)
				{
					fullTypeName += namespaceStack.Back();
					fullTypeName += "::";
					namespaceStack.PopBack();
				}
				const SyntaxTreeNodeData& structNodeSyntaxTreeItem = structNode.GetNodeData();
				fullTypeName += structNodeSyntaxTreeItem._Identifier;

				KeyValuePair found = _typeMetaDataMap.Find(fullTypeName);
				if (found.IsValid() == true)
				{
					return;
				}

				TypeMetaData<TypeCustomData> typeMetaData;
				typeMetaData.SetBaseData(fullTypeName, false);

				uint32 structSize = 0;
				StringA streamDataTypeNameForSlots;
				const int32 inputSlot = GetSlottedStreamDataInputSlot(fullTypeName, streamDataTypeNameForSlots);
				const uint32 childNodeCount = structNode.GetChildNodeCount();
				for (uint32 childNodeIndex = 0; childNodeIndex < childNodeCount; ++childNodeIndex)
				{
					SyntaxTreeNode childNode = structNode.GetChildNode(childNodeIndex);
					const SyntaxTreeNodeData& childNodeData = childNode.GetNodeData();
					if (childNodeData._classifier == SyntaxClassifier::Variable)
					{
						const uint32 attributeCount = childNode.GetChildNodeCount();
						SyntaxTreeNode dataTypeNode = childNode.GetChildNode(0);
						TypeMetaData<TypeCustomData> memberTypeMetaData = GetTypeMetaData(dataTypeNode.GetNodeData()._Identifier);
						memberTypeMetaData.SetByteOffset(structSize);
						structSize += memberTypeMetaData.GetSize();
						memberTypeMetaData.SetDeclName(childNodeData._Identifier);
						memberTypeMetaData._customData.SetInputSlot(inputSlot);

						if (attributeCount >= 2)
						{
							// SemanticName
							SyntaxTreeNode attribute1 = childNode.GetChildNode(1);
							const SyntaxTreeNodeData& attribute1Data = attribute1.GetNodeData();
							if (attribute1Data._classifier == SyntaxClassifier::SemanticName)
							{
								memberTypeMetaData._customData.SetSemanticName(attribute1Data._Identifier);
							}
							else
							{
								MINT_NEVER;
							}
						}

						typeMetaData.PushMember(memberTypeMetaData);
					}
					else if (childNodeData._classifier == SyntaxClassifier::RegisterIndex)
					{
						const int32 registerIndex = StringUtil::StringToInt32(childNodeData._value);
						typeMetaData._customData.SetRegisterIndex(registerIndex);
					}
					else if (childNodeData._classifier == SyntaxClassifier::InstanceData)
					{
						const int32 instanceDataStepRate = StringUtil::StringToInt32(childNodeData._value);
						typeMetaData._customData.SetInstanceDataStepRate(instanceDataStepRate);
					}
					else
					{
						MINT_NEVER;
					}
				}
				typeMetaData.SetSize(structSize);

				PushTypeMetaData(fullTypeName, typeMetaData);

				if (inputSlot > 0)
				{
					TypeMetaData<TypeCustomData>& streamDataForSlots = AccessTypeMetaData(streamDataTypeNameForSlots);
					streamDataForSlots._customData.PushSlottedStreamData(typeMetaData);
				}
			}

			StringA Parser::ConvertDeclarationNameToHlslSemanticName(const StringA& declarationName)
			{
				if (declarationName.IsEmpty() == true)
				{
					MINT_NEVER;
				}

				StringA semanticName = declarationName.Substring(1);
				const uint32 semanticNameLength = semanticName.Length();
				for (uint32 semanticNameIter = 0; semanticNameIter < semanticNameLength; ++semanticNameIter)
				{
					semanticName[semanticNameIter] = ::toupper(semanticName[semanticNameIter]);
				}
				return semanticName;
			}

			int32 Parser::GetSlottedStreamDataInputSlot(const StringA& typeName, StringA& streamDataTypeName) const noexcept
			{
				// 0 은 자기 자신을 의미하므로
				// 실제 Slotted StreamData 라면 InputSlot 은 1부터 시작한다!

				const bool isBackDigit = ::isdigit(typeName.Back());
				if (isBackDigit == false)
				{
					return 0;
				}

				const int32 digitCount = (::isdigit(typeName[typeName.Size() - 2]) != 0) ? 2 : 1;
				const int32 inputSlot = StringUtil::StringToInt32(typeName.Substring(typeName.Size() - digitCount, digitCount));
				streamDataTypeName = typeName.Substring(0, typeName.Size() - digitCount);
				if (ExistsTypeMetaData(streamDataTypeName) == true)
				{
					return inputSlot;
				}
				return 0;
			}

			DXGI_FORMAT Parser::ConvertCppHlslTypeToDxgiFormat(const TypeMetaData<TypeCustomData>& typeMetaData)
			{
				const StringA& typeName = typeMetaData.GetTypeName();
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

			StringA Parser::SerializeCppHlslTypeToHlslStreamDatum(const TypeMetaData<TypeCustomData>& typeMetaData)
			{
				StringA pureTypeName = TypeUtils::extractPureTypeName(typeMetaData.GetTypeName());

				// inputSlot 0 은 나 자신이다!
				Vector<TypeMetaData<TypeCustomData>> slottedDatas;
				for (int32 inputSlot = 1; inputSlot < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT; ++inputSlot)
				{
					StringA typeName = typeMetaData.GetTypeName() + StringUtil::ToStringA(inputSlot);
					if (ExistsTypeMetaData(typeName) == false)
					{
						break;
					}

					slottedDatas.PushBack(GetTypeMetaData(typeName));
				}

				StringA result;
				result.Append("struct ");
				result.Append(pureTypeName);
				result.Append("\n{\n");
				result.Append(SerializeCppHlslTypeToHlslStreamDatumMembers(typeMetaData));

				const uint32 slottedDataCount = slottedDatas.Size();
				for (uint32 slottedDataIndex = 0; slottedDataIndex < slottedDataCount; ++slottedDataIndex)
				{
					result += SerializeCppHlslTypeToHlslStreamDatumMembers(slottedDatas[slottedDataIndex]);
				}

				result.Append("};\n\n");
				return result;
			}

			StringA Parser::SerializeCppHlslTypeToHlslStreamDatumMembers(const TypeMetaData<TypeCustomData>& typeMetaData)
			{
				StringA result;
				const uint32 memberCount = typeMetaData.GetMemberCount();
				for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
				{
					const TypeMetaData<TypeCustomData>& memberType = typeMetaData.GetMember(memberIndex);
					result.Append("\t");
					result.Append(memberType.GetTypeName());
					result.Append(" ");
					result.Append(memberType.GetDeclName());
					result.Append(" : ");
					if (memberType._customData.GetSemanticName().IsEmpty() == true)
					{
						result.Append(ConvertDeclarationNameToHlslSemanticName(memberType.GetDeclName()));
					}
					else
					{
						result.Append(memberType._customData.GetSemanticName());
					}
					result.Append(";\n");
				}
				return result;
			}

			StringA Parser::SerializeCppHlslTypeToHlslConstantBuffer(const TypeMetaData<TypeCustomData>& typeMetaData, const uint32 bufferIndex)
			{
				StringA result;

				result.Append("cbuffer ");
				StringA pureTypeName = TypeUtils::extractPureTypeName(typeMetaData.GetTypeName());
				result.Append(pureTypeName);
				result.Append(" : register(");
				result.Append("b" + StringUtil::ToStringA((typeMetaData._customData.IsRegisterIndexValid() == true) ? typeMetaData._customData.GetRegisterIndex() : bufferIndex));
				result.Append(")\n{\n");

				const uint32 memberCount = typeMetaData.GetMemberCount();
				for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
				{
					const TypeMetaData<TypeCustomData>& memberType = typeMetaData.GetMember(memberIndex);
					result.Append("\t");
					result.Append(memberType.GetTypeName());
					result.Append(" ");
					result.Append(memberType.GetDeclName());
					result.Append(";\n");
				}
				result.Append("};\n\n");
				return result;
			}

			StringA Parser::SerializeCppHlslTypeToHlslStructuredBufferDefinition(const TypeMetaData<TypeCustomData>& typeMetaData)
			{
				StringA result;

				StringA pureTypeName = TypeUtils::extractPureTypeName(typeMetaData.GetTypeName());
				result.Append("struct ");
				result.Append(pureTypeName);
				result.Append("\n{\n");
				const uint32 memberCount = typeMetaData.GetMemberCount();
				for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
				{
					const TypeMetaData<TypeCustomData>& memberType = typeMetaData.GetMember(memberIndex);
					result.Append("\t");
					result.Append(memberType.GetTypeName());
					result.Append(" ");
					result.Append(memberType.GetDeclName());
					result.Append(";\n");
				}

				result.Append("};\n\n");
				return result;
			}
		}
	}
}
