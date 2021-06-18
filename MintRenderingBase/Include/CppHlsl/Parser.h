#pragma once


#ifndef MINT_CPP_PARSER_H
#define MINT_CPP_PARSER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/HashMap.h>
#include <MintContainer/Include/BitVector.h>

#include <MintRenderingBase/Include/CppHlsl/IParser.h>
#include <MintRenderingBase/Include/CppHlsl/LanguageCommon.h>


namespace mint
{
    namespace CppHlsl
    {
        class ILexer;

        enum class SyntaxClassifier
        {
            ROOT,
            Namespace,
            Struct,
            Variable,
            DataType,

            SemanticName,
            RegisterIndex,
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


        class TypeMetaData final
        {
        public:
                                            TypeMetaData();
                                            ~TypeMetaData() = default;

        public:
            void                            setBaseData(const std::string& typeName, const bool isBuiltIn);
            void                            setDeclName(const std::string& declName);
            void                            setSize(const uint32 size);
            void                            setByteOffset(const uint32 byteOffset);
            void                            setSemanticName(const std::string& semanticName);
            void                            setRegisterIndex(const uint32 registerIndex);
            void                            setInputSlot(const uint32 inputSlot);
            void                            pushMember(const TypeMetaData& member);
            void                            pushSlottedStreamData(const TypeMetaData& slottedStreamData);

        public:
            const bool                      isBuiltIn() const noexcept;
            const bool                      isRegisterIndexValid() const noexcept;

        public:
            const std::string&              getTypeName() const noexcept;
            const std::string&              getDeclName() const noexcept;
            const uint32                    getSize() const noexcept;
            const uint32                    getByteOffset() const noexcept;
            const std::string&              getSemanticName() const noexcept;
            const uint32                    getRegisterIndex() const noexcept;
            const uint32                    getInputSlot() const noexcept;
            const uint32                    getMemberCount() const noexcept;
            const TypeMetaData&             getMember(const uint32 memberIndex) const noexcept;
            const uint32                    getSlottedStreamDataCount() const noexcept;
            const TypeMetaData&             getSlottedStreamData(const uint32 inputSlot) const noexcept;
        
        public:
            static const TypeMetaData       kInvalidTypeMetaData;

        private:
            static constexpr uint32         kInvalidRegisterIndex = kUint32Max;

        private:
            bool                            _isBuiltIn;
            std::string                     _typeName;      // namespace + name
            std::string                     _declName;
            std::string                     _semanticName;
            uint32                          _registerIndex;
            uint32                          _inputSlot;
            uint32                          _size;          // Byte count
            uint32                          _byteOffset;
            mint::Vector<TypeMetaData>      _memberArray;   // Member variables
            mint::Vector<TypeMetaData>      _slottedStreamDatas;
        };


        class Parser final : public IParser
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
            const TypeMetaData&                         getTypeMetaData(const std::string& typeName) const noexcept;
            const TypeMetaData&                         getTypeMetaData(const int32 typeIndex) const noexcept;
        
        private:
            TypeMetaData&                               getTypeMetaData(const std::string& typeName) noexcept;

        private:
            const int32                                 getSlottedStreamDataInputSlot(const std::string& typeName, std::string& streamDataTypeName) const noexcept;
            const bool                                  existsTypeMetaData(const std::string& typeName) const noexcept;

        public:
            static std::string                          convertDeclarationNameToHlslSemanticName(const std::string& declarationName);
            static const DXGI_FORMAT                    convertCppHlslTypeToDxgiFormat(const TypeMetaData& typeMetaData);

        public:
            std::string                                 serializeCppHlslTypeToHlslStreamDatum(const TypeMetaData& typeMetaData);

        private:
            std::string                                 serializeCppHlslTypeToHlslStreamDatumMembers(const TypeMetaData& typeMetaData);

        public:
            std::string                                 serializeCppHlslTypeToHlslConstantBuffer(const TypeMetaData& typeMetaData, const uint32 bufferIndex);
            std::string                                 serializeCppHlslTypeToHlslStructuredBufferDefinition(const TypeMetaData& typeMetaData);

        private:
            mint::Tree<SyntaxTreeItem>                  _syntaxTree;

        private:
            mint::Vector<TypeMetaData>                  _typeMetaDatas;
            mint::HashMap<std::string, uint32>          _typeMetaDataMap;
        
        private:
            mint::HashMap<std::string, uint32>          _builtInTypeUmap;
        };
    }
}


#include <MintRenderingBase/Include/CppHlsl/Parser.inl>


#endif // !MINT_CPP_PARSER_H
