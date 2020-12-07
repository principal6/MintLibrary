#include <stdafx.h>
#include <Language/CppParser.h>

#include <Language/Lexer.h>

#include <Container/StringUtil.hpp>
#include <Container/Tree.hpp>

#include <functional>


namespace fs
{
	namespace Language
	{
		CppTypeTableItem::CppTypeTableItem(const std::string& typeName)
			: _typeName{ typeName }
		{
			__noop;
		}

		const SymbolTableItem CppParser::kClassStructAccessModifierSymbolArray[3]{
			   SymbolTableItem(SymbolClassifier::Keyword, convertCppClassStructAccessModifierToString(CppClassStructAccessModifier::Public)),
			   SymbolTableItem(SymbolClassifier::Keyword, convertCppClassStructAccessModifierToString(CppClassStructAccessModifier::Protected)),
			   SymbolTableItem(SymbolClassifier::Keyword, convertCppClassStructAccessModifierToString(CppClassStructAccessModifier::Private))
		};
		const SymbolTableItem CppParser::kInitializerListSymbol{ SymbolTableItem(SymbolClassifier::POST_GENERATED, "InitializerList") };
		const SymbolTableItem CppParser::kMemberVariablesSymbol{ SymbolTableItem(SymbolClassifier::POST_GENERATED, "MemberVariables") };
		CppParser::CppParser(Lexer& lexer)
			: IParser(lexer)
		{
			_builtInTypeUmap.insert(std::make_pair("void", 1));
			_builtInTypeUmap.insert(std::make_pair("bool", 1));
			_builtInTypeUmap.insert(std::make_pair("char", 1));
			_builtInTypeUmap.insert(std::make_pair("wchar_t", 1));
			_builtInTypeUmap.insert(std::make_pair("short", 1));
			_builtInTypeUmap.insert(std::make_pair("int", 1));
			_builtInTypeUmap.insert(std::make_pair("long", 1));
			_builtInTypeUmap.insert(std::make_pair("float", 1));
			_builtInTypeUmap.insert(std::make_pair("double", 1));
		}

		CppParser::~CppParser()
		{
			__noop;
		}

		const bool CppParser::execute()
		{
			reset();

			uint64 advanceCount = 0;
			TreeNodeAccessor syntaxTreeRootNode = getSyntaxTreeRootNode();
			while (needToContinueParsing() == true)
			{
				const SymbolTableItem& statementBeginningSymbol = getSymbol(getSymbolPosition());
				if (statementBeginningSymbol._symbolString == "#")
				{
					FS_RETURN_FALSE_IF_NOT(parsePreprocessor(getSymbolPosition(), syntaxTreeRootNode, advanceCount) == true);

					advanceSymbolPosition(advanceCount);
				}
				else if (statementBeginningSymbol._symbolString == "class")
				{
					FS_RETURN_FALSE_IF_NOT(parseClassStruct(false, getSymbolPosition(), syntaxTreeRootNode, advanceCount) == true);
					
					advanceSymbolPosition(advanceCount);
				}
				else if (statementBeginningSymbol._symbolString == "struct")
				{
					FS_RETURN_FALSE_IF_NOT(parseClassStruct(true, getSymbolPosition(), syntaxTreeRootNode, advanceCount) == true);
					
					advanceSymbolPosition(advanceCount);
				}
			}

			if (hasReportedErrors() == true)
			{
				FS_LOG_ERROR("김장원", "에러가 있었습니다!!!");

				return false;
			}

			return true;
		}

		const bool CppParser::parsePreprocessor(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount)
		{
			outAdvanceCount = 0;

			const SymbolTableItem& currentSymbol = getSymbol(symbolPosition);
			if (hasSymbol(symbolPosition + 1) == false)
			{
				reportError(currentSymbol, ErrorType::LackOfCode);
				return false;
			}

			TreeNodeAccessor preprocessorNode = ancestorNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Preprocessor));

			const SymbolTableItem& directive = getSymbol(symbolPosition + 1);
			if (directive._symbolString == "include")
			{
				if (hasSymbol(symbolPosition + 3) == false)
				{
					reportError(directive, ErrorType::LackOfCode);
					return false;
				}

				TreeNodeAccessor directiveNode = preprocessorNode.insertChildNode(SyntaxTreeItem(directive, CppSyntaxClassifier::CppSyntaxClassifier_Preprocessor_Include));

				const SymbolTableItem& openSymbol = getSymbol(symbolPosition + 2);
				if ((openSymbol._symbolString == "<" || openSymbol._symbolString == "\""))
				{
					// Find a close symbol
					uint64 closeSymbolPosition = 0;
					if (findNextSymbolEither(symbolPosition, ">", "\"", closeSymbolPosition) == true)
					{
						const SymbolTableItem& closeSymbol = getSymbol(closeSymbolPosition);
						if (openSymbol._symbolString == "<" && closeSymbol._symbolString != ">")
						{
							reportError(closeSymbol, ErrorType::GrouperMismatch);
							return false;
						}
						else if (openSymbol._symbolString == "\"" && closeSymbol._symbolString != "\"")
						{
							reportError(closeSymbol, ErrorType::GrouperMismatch);
							return false;
						}

						SymbolTableItem& fileNameSymbol = getSymbol(symbolPosition + 3);
						const uint64 betweenSymbolCount = closeSymbolPosition - (symbolPosition + 3);
						if (1 < betweenSymbolCount)
						{
							for (uint64 betweenSymbolIndex = 1; betweenSymbolIndex < betweenSymbolCount; ++betweenSymbolIndex)
							{
								SymbolTableItem& currentSymbol = getSymbol(symbolPosition + 3 + betweenSymbolIndex);
								fileNameSymbol._symbolString += currentSymbol._symbolString;
								
								currentSymbol.clearData();
							}
						}

						directiveNode.insertChildNode(SyntaxTreeItem(fileNameSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Literal_String));

						outAdvanceCount = closeSymbolPosition + 1 - getSymbolPosition();
						return true;
					}
					else
					{
						reportError(openSymbol, ErrorType::NoMatchingGrouper);
						return false;
					}
				}
				else
				{
					reportError(openSymbol, ErrorType::WrongSuccessor, "#include 뒤에는 '<' 나 '\"' 가 와야 합니다.");
					return false;
				}
			}
			else
			{
				// TODO ...
				reportError(directive, ErrorType::WrongSuccessor, "#include 외에는 아직 지원이 안 됩니다...");
				return false;
			}

			return false;
		}

		const bool CppParser::parseClassStruct(const bool isStruct, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount)
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

			TreeNodeAccessor classStructNode = ancestorNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_Definition));

			uint64 identifierOffset = 1;
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
						uint64 alignasAdvanceCount = 1;
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
				const uint64 identifierSymbolPosition = symbolPosition + identifierOffset;
				const SymbolTableItem& classIdentifierSymbol = getSymbol(identifierSymbolPosition);

				registerUserDefinedType(CppTypeTableItem(classIdentifierSymbol._symbolString));

				// TODO: final, abstract 등의 postfix 키워드...
				const SymbolTableItem& postIdentifierSymbol = getSymbol(identifierSymbolPosition + 1);
				if (postIdentifierSymbol._symbolClassifier == SymbolClassifier::StatementTerminator)
				{
					// Declaration

					classStructNode.insertChildNode(SyntaxTreeItem(classIdentifierSymbol, CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_Declaration));
					outAdvanceCount = identifierOffset + 2;

					registerUserDefinedType(CppTypeTableItem(classIdentifierSymbol._symbolString));

					return true;
				}
				else if (postIdentifierSymbol._symbolClassifier == SymbolClassifier::Grouper_Open)
				{
					// Definition

					if (postIdentifierSymbol._symbolString != "{")
					{
						reportError(postIdentifierSymbol, ErrorType::WrongSuccessor, "'{' 가 와야 합니다.");
						return false;
					}

					uint64 depthMathcingCloseSymbolPosition = 0;
					if (findNextDepthMatchingCloseSymbol(identifierSymbolPosition + 1, "}", depthMathcingCloseSymbolPosition) == false)
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

					uint64 currentPosition = identifierSymbolPosition + 2;
					CppClassStructAccessModifier currentAccessModifier = (true == isStruct) ? CppClassStructAccessModifier::Public : CppClassStructAccessModifier::Private;
					bool continueParsingMember = false;
					while (true)
					{
						uint64 advanceCount = 0;
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

		const bool CppParser::parseClassStructMember(const SymbolTableItem& classIdentifierSymbol, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, CppClassStructAccessModifier& inOutAccessModifier, uint64& outAdvanceCount, bool& outContinueParsing)
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
			uint64 postAccessModifierOffset = 0;
			{
				const SymbolTableItem& symbol = getSymbol(symbolPosition);
				if (convertSymbolToAccessModifierSyntax(symbol) == CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_AccessModifier)
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
				const uint64 currentPosition = symbolPosition + postAccessModifierOffset;
				const SymbolTableItem& symbol = getSymbol(currentPosition);
				const bool isDestructor = (symbol._symbolString == "~");
				if (symbol._symbolString == classIdentifierSymbol._symbolString || isDestructor == true)
				{
					// Constructor or Destructor
					const SymbolTableItem& postSymbol = getSymbol(currentPosition + 1);

					TreeNodeAccessor ctorOrDtorNode = ancestorNode.insertChildNode(SyntaxTreeItem((isDestructor == true) ? postSymbol : symbol, (isDestructor == true) ? CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_Destructor : CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_Constructor));
					{
						TreeNodeAccessor accessModifierNode = ctorOrDtorNode.insertChildNode(SyntaxTreeItem(getClassStructAccessModifierSymbol(inOutAccessModifier), CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_AccessModifier));
					}

					const uint64 openParenthesisPosition = (isDestructor == true) ? currentPosition + 2 : currentPosition + 1;
					const SymbolTableItem& openParenthesisSymbol = getSymbol(openParenthesisPosition);
					if (openParenthesisSymbol._symbolString != "(")
					{
						reportError(openParenthesisSymbol, ErrorType::WrongSuccessor, "'(' 가 와야 합니다.");
						return false;
					}

					uint64 closeParenthesisPosition = 0;
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
					uint64 declarationEndPosition = closeParenthesisPosition + 1;
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
							uint64 initializeListAdvanceCount = 0;
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

							uint64 postInstructionAdvance = 0;
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
					uint64 postTypeChunkPosition = 0;
					if (isTypeChunk(currentPosition, postTypeChunkPosition) == true)
					{
						const uint64 openParenthesisPosition = postTypeChunkPosition + 1;
						const SymbolTableItem& postPostTpeChunkSymbol = getSymbol(openParenthesisPosition);
						if (postPostTpeChunkSymbol._symbolString == "(")
						{
							isFunction = true;

							uint64 closeParenthesisPosition = 0;
							if (findNextDepthMatchingCloseSymbol(openParenthesisPosition, ")", closeParenthesisPosition) == false)
							{
								reportError(symbol, ErrorType::NoMatchingGrouper, "')' 를 찾지 못했습니다.");
								return false;
							}

							const SymbolTableItem& identifierSymbol = getSymbol(postTypeChunkPosition);
							TreeNodeAccessor functionNameNode = ancestorNode.insertChildNode(SyntaxTreeItem(identifierSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Function_Name));

							// function attribute
							bool isConst	= false;
							bool isNoexcept	= false;
							bool isOverride	= false;
							bool isFinal	= false;
							bool isAbstract	= false;
							bool isDefault	= false;
							bool isDelete	= false;
							uint64 postAttributePosition	= closeParenthesisPosition + 1;
							bool hasEqualSign				= false;
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

							const CppAdditionalInfo_FunctionAttributeFlags attributeFlags = static_cast<CppAdditionalInfo_FunctionAttributeFlags>(
								static_cast<int>(isConst)		* CppAdditionalInfo_FunctionAttributeFlags::CppAdditionalInfo_FunctionAttributeFlags_Const		|
								static_cast<int>(isNoexcept)	* CppAdditionalInfo_FunctionAttributeFlags::CppAdditionalInfo_FunctionAttributeFlags_Noexcept	|
								static_cast<int>(isOverride)	* CppAdditionalInfo_FunctionAttributeFlags::CppAdditionalInfo_FunctionAttributeFlags_Override	|
								static_cast<int>(isFinal)		* CppAdditionalInfo_FunctionAttributeFlags::CppAdditionalInfo_FunctionAttributeFlags_Final		|
								static_cast<int>(isAbstract)	* CppAdditionalInfo_FunctionAttributeFlags::CppAdditionalInfo_FunctionAttributeFlags_Abstract	|
								static_cast<int>(isDefault)		* CppAdditionalInfo_FunctionAttributeFlags::CppAdditionalInfo_FunctionAttributeFlags_Default	|
								static_cast<int>(isDelete)		* CppAdditionalInfo_FunctionAttributeFlags::CppAdditionalInfo_FunctionAttributeFlags_Delete
								);
							functionNameNode.getNodeDataXXX().setAdditionalInfo(attributeFlags);

							const SymbolTableItem& postAttributeSymbol = getSymbol(postAttributePosition);
							if (postAttributeSymbol._symbolString == ";")
							{
								outAdvanceCount = closeParenthesisPosition - currentPosition + 2;
								return true;
							}
							else if (postAttributeSymbol._symbolString == "{")
							{
								FS_RETURN_FALSE_IF_NOT(parseFunctionParameters(false, openParenthesisPosition + 1, functionNameNode) == true);

								uint64 postInstructionAdvance = 0;
								FS_RETURN_FALSE_IF_NOT(parseFunctionInstructions(postAttributePosition + 1, functionNameNode, postInstructionAdvance) == true);

								outAdvanceCount = postAttributePosition + postInstructionAdvance - currentPosition + 1;
								return true;
							}
							else
							{
								reportError(postAttributeSymbol, ErrorType::WrongSuccessor, "';' 나 '{' 가 와야 합니다.");
								return false;
							}
						}

						// Variable
						{
							// TODO
							// getTypeNode()
							TreeNodeAccessor memberVariablesNode = ancestorNode.insertChildNode(SyntaxTreeItem(kMemberVariablesSymbol, CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_MemberVariables));

							uint64 postTypeNodeOffset = 0;
							TreeNodeAccessor<SyntaxTreeItem> typeNode;
							FS_RETURN_FALSE_IF_NOT(parseTypeNode(currentPosition, memberVariablesNode, typeNode, postTypeNodeOffset) == true);

							const SymbolTableItem& identifierSymbol = getSymbol(postTypeChunkPosition);
							TreeNodeAccessor identifierNode = typeNode.insertChildNode(SyntaxTreeItem(identifierSymbol, CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_MemberVariableIdentifier));

							const SymbolTableItem& postIdentifierSymbol = getSymbol(postTypeChunkPosition + 1);
							if (postIdentifierSymbol._symbolString == ";")
							{
								// No initialization

								outAdvanceCount = postTypeChunkPosition - symbolPosition + 2;
								return true;
							}
							else if (postIdentifierSymbol._symbolString == "=" || postIdentifierSymbol._symbolString == "{")
							{
								// TODO
								// Initialization

								// parseRvalue();
								return true;
							}
							else
							{
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

		const bool CppParser::parseClassStructInitializerList(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount)
		{
			outAdvanceCount = 0;

			if (hasSymbol(symbolPosition + 3) == false)
			{
				const SymbolTableItem& firstMemberSymbol = getSymbol(symbolPosition);
				reportError(firstMemberSymbol, ErrorType::LackOfCode);
				return false;
			}

			TreeNodeAccessor initializerListNode = ancestorNode.insertChildNode(SyntaxTreeItem(kInitializerListSymbol, CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_Constructor_InitializerList));
			uint64 currentOffset = 0;
			bool continueParsing = true;
			do
			{
				uint64 advanceCount = 0;
				FS_RETURN_FALSE_IF_NOT(parseClassStructInitializerList_Item(symbolPosition + currentOffset, initializerListNode, advanceCount, continueParsing) == true);

				currentOffset += advanceCount;
			} while (continueParsing == true);

			outAdvanceCount = currentOffset;
			return true;
		}

		const bool CppParser::parseClassStructInitializerList_Item(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount, bool& outContinueParsing)
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
					static constexpr uint32 kAccessModifierNodeCount = 1;
					const uint32 childNodeCount = classStructCtorNode.getChildNodeCount();
					for (uint32 childNodeIndex = kAccessModifierNodeCount; childNodeIndex < childNodeCount; ++childNodeIndex)
					{
						TreeNodeAccessor parameterNode = classStructCtorNode.getChildNode(childNodeIndex);
						const uint32 parameterNodeChildCount = parameterNode.getChildNodeCount();
						TreeNodeAccessor parameterIdentifierNode = parameterNode.getChildNode(parameterNodeChildCount - 1);
						if (parameterIdentifierNode.getNodeData()._symbolTableItem->_symbolString == valueSymbol._symbolString)
						{
							parameterIndex = childNodeIndex - kAccessModifierNodeCount;
							break;
						}
					}

					if (parameterIndex == kUint32Max)
					{
						reportError(valueSymbol, ErrorType::SymbolNotFound);
						return false;
					}

					TreeNodeAccessor memberNode = ancestorNode.insertChildNode(SyntaxTreeItem(memberSymbol, CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_MemberVariables));
					memberNode.insertChildNode(SyntaxTreeItem(valueSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Function_Parameter));

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

					TreeNodeAccessor memberNode = ancestorNode.insertChildNode(SyntaxTreeItem(memberSymbol, CppSyntaxClassifier::CppSyntaxClassifier_ClassStruct_MemberVariables));
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

		const bool CppParser::parseFunctionParameters(const bool isDeclaration, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode)
		{
			const SymbolTableItem& currentSymbol = getSymbol(symbolPosition);
			if (currentSymbol._symbolString == ")")
			{
				// No parameters.
				return true;
			}

			uint64 closeParenthesisPosition = 0;
			if (findNextSymbol(symbolPosition, ")", closeParenthesisPosition) == false)
			{
				reportError(currentSymbol, ErrorType::LackOfCode);
				return false;
			}

			uint64 currentItemOffset = 0;
			uint64 advanceCount = 0;
			do
			{
				FS_RETURN_FALSE_IF_NOT(parseFunctionArguments_Item(isDeclaration, symbolPosition + currentItemOffset, ancestorNode, advanceCount) == true);
				currentItemOffset += advanceCount;
			} while (0 < advanceCount);

			return true;
		}

		const bool CppParser::parseFunctionArguments_Item(const bool isDeclaration, const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount)
		{
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


			outAdvanceCount = 0;

			// Prefix const
			uint64 postPrefixConstSymbolOffset = 0;
			bool isConst = false;
			const SymbolTableItem& firstSymbol = getSymbol(symbolPosition);
			if (isSymbolType(firstSymbol) == false)
			{
				if (firstSymbol._symbolString == "const")
				{
					isConst = true;
					postPrefixConstSymbolOffset = 1;
				}
				else
				{
					reportError(firstSymbol, ErrorType::WrongPredecessor, "const 가 와야합니다!");
					return false;
				}
			}

			// (namespace::)Type
			uint64 typeSymbolOffset = postPrefixConstSymbolOffset;
			{
				std::string namespaceString;

				while (true)
				{
					const SymbolTableItem& symbol = getSymbol(symbolPosition + typeSymbolOffset);
					if (isSymbolType(symbol) == false)
					{
						const SymbolTableItem& nextSymbol = getSymbol(symbolPosition + typeSymbolOffset + 1);
						if (nextSymbol._symbolString == "::")
						{
							namespaceString += symbol._symbolString;
							typeSymbolOffset += 2;
						}
						else
						{
							reportError(symbol, ErrorType::WrongSuccessor, "type 이 와야합니다!");
							return false;
						}
					}
					else
					{
						break;
					}
				}
			}
			
			// Postfix const
			uint64 postTypeOffset = typeSymbolOffset + 1;
			{
				const SymbolTableItem& postTypeSymbol = getSymbol(symbolPosition + typeSymbolOffset + 1);
				if (postTypeSymbol._symbolString == "const")
				{
					isConst = true;
					++postTypeOffset;
				}
			}
			
			const SymbolTableItem& typeSymbol = getSymbol(symbolPosition + typeSymbolOffset);
			TreeNodeAccessor typeNode = ancestorNode.insertChildNode(SyntaxTreeItem(typeSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Type,
				(isConst == true) ? CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Const : CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_NONE));
			
			// Postfix part
			// *, **, ...
			// &, && (뒤에 * 는 못 옴)
			// const
			uint64 postfixOffset = postTypeOffset;
			{
				uint8 ampersandCount = 0; // [0, 2]
				TreeNodeAccessor<SyntaxTreeItem> previousNode;
				while (true)
				{
					const uint64 currentPosition = symbolPosition + postfixOffset;
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
							previousNode = typeNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Type_ReferenceType));
						}
						else if (ampersandCount == 2)
						{
							previousNode = typeNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Type_RvalueReferenceType));
						}
						else
						{
							reportError(currentSymbol, ErrorType::WrongSuccessor, "참조에 대한 참조는 사용할 수 없습니다.");
							return false;
						}
					}
					else if (currentSymbol._symbolString == "const")
					{
						previousNode.getNodeDataXXX().setAdditionalInfo(CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Const);
					}
					else if (currentSymbol._symbolString == "*")
					{
						// Asterisk

						if (0 < ampersandCount)
						{
							reportError(currentSymbol, ErrorType::WrongSuccessor, "참조에 대한 포인터는 사용할 수 없습니다.");
							return false;
						}

						previousNode = typeNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Type_PointerType));
					}
					else
					{
						break;
					}

					++postfixOffset;
				}
			}

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
					typeNode.insertChildNode(SyntaxTreeItem(symbol, CppSyntaxClassifier::CppSyntaxClassifier_Function_Parameter));

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

		const bool CppParser::parseFunctionInstructions(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount)
		{
			outAdvanceCount = 0;

			// parseStatement (;)
			// parseExpression...?
			FS_RETURN_FALSE_IF_NOT(parseExpression(symbolPosition, ancestorNode, outAdvanceCount));
			++outAdvanceCount;

			return true;
		}

		const bool CppParser::parseExpression(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount)
		{
			outAdvanceCount = 0;

			// TODO: parenthesis

			uint64 postTypeChunkPosition = 0;
			if (isTypeChunk(symbolPosition, postTypeChunkPosition) == true)
			{
				uint64 postTypeNodeOffset = 0;
				TreeNodeAccessor<SyntaxTreeItem> typeNode;
				FS_RETURN_FALSE_IF_NOT(parseTypeNode(symbolPosition, ancestorNode, typeNode, postTypeNodeOffset) == true);

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

			// TODO
			// Non-declaration ??
			{
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

						ancestorNode.insertChildNode(SyntaxTreeItem(symbol, CppSyntaxClassifier::CppSyntaxClassifier_Function_Return));

						outAdvanceCount = 2;
						return true;
					}
					else
					{
						TreeNodeAccessor returnNode = ancestorNode.insertChildNode(SyntaxTreeItem(symbol, CppSyntaxClassifier::CppSyntaxClassifier_Function_Return));
						const CppTypeOf cppTypeOf = getTypeOf(nextSymbol);
						if (cppTypeOf == CppTypeOf::INVALID)
						{
							reportError(nextSymbol, ErrorType::WrongSuccessor, "type 이 와야 합니다.");
							return false;
						}

						TreeNodeAccessor valueNode = returnNode.insertChildNode(SyntaxTreeItem(nextSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Function_Return_Value));
						
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

			
			
			// (binary) operator 다음에 (unary) identifier (unary)
			// a++, a-- == rvalue
			// ++a, --a == lvalue
			// ++(a++) // INVALID
			// (++a)++ // VALID

			return false;
		}

		const bool CppParser::isTypeChunk(const uint64 symbolPosition, uint64& outPostTypeChunkPosition)
		{
			uint64 offset = 0;
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

		const bool CppParser::parseTypeNode(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, TreeNodeAccessor<SyntaxTreeItem>& outTypeNode, uint64& outAdvanceCount)
		{
			outAdvanceCount = 0;

			// modifier: static constexpr const mutable thread_local
			// 1) Premodifier
			// 2) (namespace::) Type(T) and postmodifier
			// 3) * (const) or & &&
			// 4) identifier

			bool isConst = false;	// const 는 중복 가능!!
			bool isConstexpr = false;
			bool isMutable = false;	// For ClassStruct
			bool isStatic = false;
			bool isThreadLocal = false;	// For non-ClassStruct

			static std::function<const uint64(const uint64, bool&)> modifierChecker =
				[&](const uint64 symbolPosition, bool& errorOccured)->const uint64
			{
				uint64 premodifierOffset = 0;
				while (true)
				{
					const SymbolTableItem& symbol = getSymbol(symbolPosition + premodifierOffset);
					if (symbol._symbolClassifier != SymbolClassifier::Keyword)
					{
						break;
					}

					if (symbol._symbolString == "static")
					{
						if (isStatic == true)
						{
							reportError(symbol, ErrorType::RepetitionOfCode, "'static' 이 중복됩니다.");
							break;
						}
						isStatic = true;
					}
					else if (symbol._symbolString == "constexpr")
					{
						if (isConstexpr == true)
						{
							reportError(symbol, ErrorType::RepetitionOfCode, "'constexpr' 이 중복됩니다.");
							break;
						}
						isConstexpr = true;
					}
					else if (symbol._symbolString == "const")
					{
						isConst = true; // const 는 중복 가능!!
					}
					else if (symbol._symbolString == "mutable")
					{
						if (isMutable == true)
						{
							reportError(symbol, ErrorType::RepetitionOfCode, "'mutable' 이 중복됩니다.");
							break;
						}
						isMutable = true;
					}
					else if (symbol._symbolString == "thread_local")
					{
						if (isThreadLocal == true)
						{
							reportError(symbol, ErrorType::RepetitionOfCode, "'thread_local' 이 중복됩니다.");
							break;
						}
						isThreadLocal = true;
					}
					else
					{
						break;
					}

					++premodifierOffset;
				}
				return premodifierOffset;
			};

			// 1) Premodifier
			bool checkerGotError = false;
			const uint64 postPremodifierOffset = modifierChecker(symbolPosition, checkerGotError);
			if (checkerGotError == true)
			{
				return false;
			}

			// 2) (namespace::) Type and postmodifier
			// TODO: namespace data
			uint64 postPostmodifierOffset = postPremodifierOffset;
			while (true)
			{
				const SymbolTableItem symbol = getSymbol(symbolPosition + postPostmodifierOffset);
				if (isSymbolType(symbol) == false)
				{
					const SymbolTableItem postSymbol = getSymbol(symbolPosition + postPostmodifierOffset + 1);
					if (postSymbol._symbolString == "::")
					{
						postPostmodifierOffset += 2;
					}
					else
					{
						reportError(symbol, ErrorType::WrongSuccessor, "namespace 뒤에는 '::' 가 와야 합니다.");
						return false;
					}
				}
				else
				{
					break;
				}
			}

			const SymbolTableItem typeSymbol = getSymbol(symbolPosition + postPostmodifierOffset);
			if (0 < postPostmodifierOffset)
			{
				if (isSymbolType(typeSymbol) == false)
				{
					reportError(typeSymbol, ErrorType::WrongSuccessor, "Type 이 와야 합니다.");
					return false;
				}

				postPostmodifierOffset += modifierChecker(symbolPosition + postPostmodifierOffset + 1, checkerGotError);
				if (checkerGotError == true)
				{
					return false;
				}
			}

			const CppAdditionalInfo_TypeFlags typeFlags = static_cast<CppAdditionalInfo_TypeFlags>(
				static_cast<int>(isConst) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Const |
				static_cast<int>(isConstexpr) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Constexpr |
				static_cast<int>(isMutable) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Mutable |
				static_cast<int>(isStatic) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Static |
				static_cast<int>(isThreadLocal) * CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_ThreadLocal
				);
			outTypeNode = ancestorNode.insertChildNode(SyntaxTreeItem(typeSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Type, typeFlags));

			// 3) * (const) or & &&
			uint64 postPointerReferenceOffset = postPostmodifierOffset;
			{
				uint8 ampersandCount = 0; // [0, 2]
				TreeNodeAccessor<SyntaxTreeItem> previousNode;
				while (true)
				{
					const uint64 currentPosition = symbolPosition + postPointerReferenceOffset;
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
							previousNode = outTypeNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Type_ReferenceType));
						}
						else if (ampersandCount == 2)
						{
							previousNode = outTypeNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Type_RvalueReferenceType));
						}
						else
						{
							reportError(currentSymbol, ErrorType::WrongSuccessor, "참조에 대한 참조는 사용할 수 없습니다.");
							return false;
						}
					}
					else if (currentSymbol._symbolString == "const")
					{
						previousNode.getNodeDataXXX().setAdditionalInfo(CppAdditionalInfo_TypeFlags::CppAdditionalInfo_TypeFlags_Const);
					}
					else if (currentSymbol._symbolString == "*")
					{
						if (0 < ampersandCount)
						{
							reportError(currentSymbol, ErrorType::WrongSuccessor, "참조에 대한 포인터는 사용할 수 없습니다.");
							return false;
						}

						previousNode = outTypeNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Type_PointerType));
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

		const bool CppParser::parseAlignas(const uint64 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& ancestorNode, uint64& outAdvanceCount)
		{
			outAdvanceCount = 0;

			const SymbolTableItem& currentSymbol = getSymbol(symbolPosition);
			if (hasSymbol(symbolPosition + 4) == false)
			{
				reportError(currentSymbol, ErrorType::LackOfCode);
				return false;
			}

			FS_RETURN_FALSE_IF_NOT(getSymbol(symbolPosition + 2)._symbolString == "(");
			
			TreeNodeAccessor alignasNode = ancestorNode.insertChildNode(SyntaxTreeItem(currentSymbol, CppSyntaxClassifier::CppSyntaxClassifier_Alignas));

			// TODO
			// implement this...

			return false;
		}

		void CppParser::registerUserDefinedType(const CppTypeTableItem& userDefinedType)
		{
			// TODO
			// namespace

			if (isBuiltInType(userDefinedType.getTypeName()) == true)
			{
				return;
			}

			if (_typeTableUmap.find(userDefinedType.getTypeName()) == _typeTableUmap.end())
			{
				_typeTable.emplace_back(userDefinedType);
				const uint64 typeTableIndex = _typeTable.size() - 1;
				_typeTableUmap.insert(std::make_pair(userDefinedType.getTypeName(), typeTableIndex));
			}
		}

		const bool CppParser::isSymbolType(const SymbolTableItem& symbol) const noexcept
		{
			if (isBuiltInType(symbol._symbolString) == true)
			{
				return true;
			}

			return isUserDefinedType(symbol._symbolString);
		}

		const bool CppParser::isBuiltInType(const std::string& symbolString) const noexcept
		{
			return (_builtInTypeUmap.find(symbolString) != _builtInTypeUmap.end());
		}

		const bool CppParser::isUserDefinedType(const std::string& symbolString) const noexcept
		{
			return _typeTableUmap.find(symbolString) != _typeTableUmap.end();
		}

		const CppTypeOf CppParser::getTypeOf(const SymbolTableItem& symbol) const noexcept
		{
			const CppSyntaxClassifier literalSyntaxClassifier = convertLiteralSymbolToSyntax(symbol);
			if (literalSyntaxClassifier != CppSyntaxClassifier::CppSyntaxClassifier_INVALID)
			{
				return CppTypeOf::LiteralType;
			}

			if (isBuiltInType(symbol._symbolString) == true)
			{
				return CppTypeOf::BuiltInType;
			}

			if (isUserDefinedType(symbol._symbolString) == true)
			{
				return CppTypeOf::UserDefinedType;
			}

			return CppTypeOf::INVALID;
		}

	}
}
