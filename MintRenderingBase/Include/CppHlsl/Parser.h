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

            struct SyntaxTreeItem
            {
            public:
                                        SyntaxTreeItem();
                                        ~SyntaxTreeItem();

            //private:
                SyntaxClassifier        _classifier;
                std::string             _identifier;
                std::string             _value;
            };


            class TypeCustomData
            {
            public:
                                                            TypeCustomData();
                                                            ~TypeCustomData() = default;

            public:
                void                                        setSemanticName(const std::string& semanticName);
                void                                        setRegisterIndex(const uint32 registerIndex);
                void                                        setInputSlot(const uint32 inputSlot);
                void                                        setInstanceDataStepRate(const uint32 instanceDataStepRate);
                void                                        pushSlottedStreamData(const TypeMetaData<TypeCustomData>& slottedStreamData);

            public:
                const bool                                  isRegisterIndexValid() const noexcept;

            public:
                const std::string&                          getSemanticName() const noexcept;
                const uint32                                getRegisterIndex() const noexcept;
                const uint32                                getInputSlot() const noexcept;
                const uint32                                getInstanceDataStepRate() const noexcept;
                const uint32                                getSlottedStreamDataCount() const noexcept;
                const TypeMetaData<TypeCustomData>&         getSlottedStreamData(const uint32 inputSlot) const noexcept;

            private:
                static constexpr uint32                     kInvalidRegisterIndex = kUint32Max;

            private:
                std::string                                 _semanticName;
                uint32                                      _registerIndex;
                uint32                                      _inputSlot;
                uint32                                      _instanceDataStepRate;
                mint::Vector<TypeMetaData<TypeCustomData>>  _slottedStreamDatas;
            };


            class Parser final : public IParser<TypeCustomData>
            {
            public:
                                                            Parser(ILexer& lexer);
                virtual                                     ~Parser();

            public:
                virtual const bool                          execute() override final;

            private:
                void                                        registerTypeInternal(const std::string& typeFullName, const uint32 typeSize, const bool isBuiltIn = false) noexcept;

            private:
                const bool                                  parseCode(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& currentNode, uint32& outAdvanceCount) noexcept;

            private:
                const bool                                  parseNamespace(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& currentNode, uint32& outAdvanceCount) noexcept;
                const bool                                  parseStruct(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& currentNode, uint32& outAdvanceCount) noexcept;
                const bool                                  parseStructMember(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& currentNode, uint32& outAdvanceCount) noexcept;
                const bool                                  parseCustomSyntax(const uint32 symbolPosition, TreeNodeAccessor<SyntaxTreeItem>& currentNode, uint32& outAdvanceCount) noexcept;
        
            private:
                void                                        buildTypeMetaData(const TreeNodeAccessor<SyntaxTreeItem>& structNode) noexcept;

            public:
                const uint32                                getTypeMetaDataCount() const noexcept;
                const TypeMetaData<TypeCustomData>&         getTypeMetaData(const std::string& typeName) const noexcept;
                const TypeMetaData<TypeCustomData>&         getTypeMetaData(const int32 typeIndex) const noexcept;

            private:
                TypeMetaData<TypeCustomData>&               getTypeMetaData(const std::string& typeName) noexcept;

            private:
                const int32                                 getSlottedStreamDataInputSlot(const std::string& typeName, std::string& streamDataTypeName) const noexcept;
                const bool                                  existsTypeMetaData(const std::string& typeName) const noexcept;

            public:
                static std::string                          convertDeclarationNameToHlslSemanticName(const std::string& declarationName);
                static const DXGI_FORMAT                    convertCppHlslTypeToDxgiFormat(const TypeMetaData<TypeCustomData>& typeMetaData);

            public:
                std::string                                 serializeCppHlslTypeToHlslStreamDatum(const TypeMetaData<TypeCustomData>& typeMetaData);

            private:
                std::string                                 serializeCppHlslTypeToHlslStreamDatumMembers(const TypeMetaData<TypeCustomData>& typeMetaData);

            public:
                std::string                                 serializeCppHlslTypeToHlslConstantBuffer(const TypeMetaData<TypeCustomData>& typeMetaData, const uint32 bufferIndex);
                std::string                                 serializeCppHlslTypeToHlslStructuredBufferDefinition(const TypeMetaData<TypeCustomData>& typeMetaData);

            private:
                mint::Tree<SyntaxTreeItem>                  _syntaxTree;

            private:
                mint::Vector<TypeMetaData<TypeCustomData>>  _typeMetaDatas;
                mint::HashMap<std::string, uint32>          _typeMetaDataMap;
        
            private:
                mint::HashMap<std::string, uint32>          _builtInTypeUmap;
            };
        }
    }
}


#include <MintRenderingBase/Include/CppHlsl/Parser.inl>


#endif // !MINT_CPP_PARSER_H
