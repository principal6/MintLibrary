#pragma once


#ifndef _MINT_RENDERING_BASE_CPP_HLSL_PARSER_H_
#define _MINT_RENDERING_BASE_CPP_HLSL_PARSER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/HashMap.h>
#include <MintContainer/Include/BitVector.h>

#include <MintLanguage/Include/LanguageCommon.h>
#include <MintLanguage/Include/IParser.h>


namespace mint
{
	namespace Language
	{
		class ILexer;


		namespace CppHlsl
		{
			enum class SyntaxClassifier
			{
				ROOT,
				Namespace,
				Struct,
				Variable,
				DataType,

				SemanticName,
				RegisterIndex,
				InstanceData,
			};


			class TypeCustomData
			{
			public:
				TypeCustomData();
				~TypeCustomData() = default;

			public:
				void SetSemanticName(const StringA& semanticName);
				void SetRegisterIndex(const uint32 registerIndex);
				void SetInputSlot(const uint32 inputSlot);
				void SetInstanceDataStepRate(const uint32 instanceDataStepRate);
				void PushSlottedStreamData(const TypeMetaData<TypeCustomData>& slottedStreamData);

			public:
				bool IsRegisterIndexValid() const noexcept;

			public:
				const StringA& GetSemanticName() const noexcept;
				uint32 GetRegisterIndex() const noexcept;
				uint32 GetInputSlot() const noexcept;
				uint32 GetInstanceDataStepRate() const noexcept;
				uint32 GetSlottedStreamDataCount() const noexcept;
				const TypeMetaData<TypeCustomData>& GetSlottedStreamData(const uint32 inputSlot) const noexcept;

			private:
				static constexpr uint32 kInvalidRegisterIndex = kUint32Max;

			private:
				StringA _semanticName;
				uint32 _registerIndex;
				uint32 _inputSlot;
				uint32 _instanceDataStepRate;
				Vector<TypeMetaData<TypeCustomData>> _slottedStreamDatas;
			};


			class Parser final : public IParser<TypeCustomData, SyntaxClassifier>
			{
			public:
				Parser(ILexer& lexer);
				virtual ~Parser();

			public:
				virtual bool Execute() override final;

			private:
				bool ParseCode(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;

			private:
				bool ParseNamespace(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;
				bool ParseStruct(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;
				bool ParseStructMember(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;
				bool ParseCustomSyntax(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;

			private:
				void BuildTypeMetaData(const SyntaxTreeNode& structNode) noexcept;

			private:
				int32 GetSlottedStreamDataInputSlot(const StringA& typeName, StringA& streamDataTypeName) const noexcept;

			public:
				static StringA ConvertDeclarationNameToHlslSemanticName(const StringA& declarationName);
				static DXGI_FORMAT ConvertCppHlslTypeToDxgiFormat(const TypeMetaData<TypeCustomData>& typeMetaData);

			public:
				StringA SerializeCppHlslTypeToHlslStreamDatum(const TypeMetaData<TypeCustomData>& typeMetaData);

			private:
				StringA SerializeCppHlslTypeToHlslStreamDatumMembers(const TypeMetaData<TypeCustomData>& typeMetaData);

			public:
				StringA SerializeCppHlslTypeToHlslConstantBuffer(const TypeMetaData<TypeCustomData>& typeMetaData, const uint32 bufferIndex);
				StringA SerializeCppHlslTypeToHlslStructuredBufferDefinition(const TypeMetaData<TypeCustomData>& typeMetaData);
			};
		}
	}
}


#include <MintRenderingBase/Include/CppHlsl/Parser.inl>


#endif // !_MINT_RENDERING_BASE_CPP_HLSL_PARSER_H_
