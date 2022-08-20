#pragma once


#ifndef MINT_CPP_HLSL_H
#define MINT_CPP_HLSL_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/CppHlsl/Lexer.h>
#include <MintRenderingBase/Include/CppHlsl/Parser.h>


namespace mint
{
    namespace Language
    {
        namespace CppHlsl
        {
            enum class CppHlslFileType
            {
                StreamData,
                ConstantBuffers,
                StructuredBuffers,
            };

            class Interpreter
            {
            public:
                                                        Interpreter();
                                                        ~Interpreter() = default;

            public:
                void                                    parseCppHlslFile(const char* const fileName);
        
            public:
                void                                    generateHlslString(const CppHlslFileType fileType);
        
            public:
                const char*                             getHlslString() const noexcept;

            public:
                uint32                                  getTypeMetaDataCount() const noexcept;
                const TypeMetaData<TypeCustomData>&     getTypeMetaData(const uint32 typeIndex) const noexcept;
                const TypeMetaData<TypeCustomData>&     getTypeMetaData(const std::string& typeName) const noexcept;
                const TypeMetaData<TypeCustomData>&     getTypeMetaData(const std::type_info& stdTypeInfo) const noexcept;
        
            private:
                Lexer                                   _lexer;
                Parser                                  _parser;

            private:
                CppHlslFileType                         _fileType;
                std::string                             _hlslString;
            };
        }
    }
}


#endif // !MINT_CPP_HLSL_H
