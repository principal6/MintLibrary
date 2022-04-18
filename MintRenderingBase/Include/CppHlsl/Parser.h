#pragma once


#ifndef MINT_CPP_PARSER_H
#define MINT_CPP_PARSER_H


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
                void                                    setSemanticName(const std::string& semanticName);
                void                                    setRegisterIndex(const uint32 registerIndex);
                void                                    setInputSlot(const uint32 inputSlot);
                void                                    setInstanceDataStepRate(const uint32 instanceDataStepRate);
                void                                    pushSlottedStreamData(const TypeMetaData<TypeCustomData>& slottedStreamData);

            public:
                bool                                    isRegisterIndexValid() const noexcept;

            public:
                const std::string&                      getSemanticName() const noexcept;
                uint32                                  getRegisterIndex() const noexcept;
                uint32                                  getInputSlot() const noexcept;
                uint32                                  getInstanceDataStepRate() const noexcept;
                uint32                                  getSlottedStreamDataCount() const noexcept;
                const TypeMetaData<TypeCustomData>&     getSlottedStreamData(const uint32 inputSlot) const noexcept;

            private:
                static constexpr uint32                 kInvalidRegisterIndex = kUint32Max;

            private:
                std::string                             _semanticName;
                uint32                                  _registerIndex;
                uint32                                  _inputSlot;
                uint32                                  _instanceDataStepRate;
                Vector<TypeMetaData<TypeCustomData>>    _slottedStreamDatas;
            };


            class Parser final : public IParser<TypeCustomData, SyntaxClassifier>
            {
            public:
                                        Parser(ILexer& lexer);
                virtual                 ~Parser();

            public:
                virtual bool            execute() override final;

            private:
                bool                    parseCode(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;

            private:
                bool                    parseNamespace(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;
                bool                    parseStruct(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;
                bool                    parseStructMember(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;
                bool                    parseCustomSyntax(const uint32 symbolPosition, SyntaxTreeNode& currentNode, uint32& outAdvanceCount) noexcept;

            private:
                void                    buildTypeMetaData(const SyntaxTreeNode& structNode) noexcept;

            private:
                int32                   getSlottedStreamDataInputSlot(const std::string& typeName, std::string& streamDataTypeName) const noexcept;

            public:
                static std::string      convertDeclarationNameToHlslSemanticName(const std::string& declarationName);
                static DXGI_FORMAT      convertCppHlslTypeToDxgiFormat(const TypeMetaData<TypeCustomData>& typeMetaData);

            public:
                std::string             serializeCppHlslTypeToHlslStreamDatum(const TypeMetaData<TypeCustomData>& typeMetaData);

            private:
                std::string             serializeCppHlslTypeToHlslStreamDatumMembers(const TypeMetaData<TypeCustomData>& typeMetaData);

            public:
                std::string             serializeCppHlslTypeToHlslConstantBuffer(const TypeMetaData<TypeCustomData>& typeMetaData, const uint32 bufferIndex);
                std::string             serializeCppHlslTypeToHlslStructuredBufferDefinition(const TypeMetaData<TypeCustomData>& typeMetaData);
            };
        }
    }
}


#include <MintRenderingBase/Include/CppHlsl/Parser.inl>


#endif // !MINT_CPP_PARSER_H
