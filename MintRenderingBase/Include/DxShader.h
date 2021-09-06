#pragma once


#ifndef MINT_DX_SHADER_H
#define MINT_DX_SHADER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/IDxObject.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
    namespace Language
    {
        template <typename CustomDataType>
        class TypeMetaData;


        namespace CppHlsl
        {
            class TypeCustomData;
        }
    }


    namespace Rendering
    {
        class DxShaderHeaderMemory;
        class GraphicDevice;
        class DxShaderPool;


        using Microsoft::WRL::ComPtr;


        struct DxInputElementSet
        {
            mint::Vector<std::string>                 _semanticNameArray;
            mint::Vector<D3D11_INPUT_ELEMENT_DESC>    _inputElementDescriptorArray;
        };

        enum class DxShaderVersion
        {
            v_4_0,
            v_5_0,
        };


        class DxShader final : public IDxObject
        {
            friend DxShaderPool;

        private:
                                        DxShader(GraphicDevice* const graphicDevice, const DxShaderType shaderType);

        public:
            virtual                     ~DxShader() = default;

        public:
            void                        bind() const noexcept;
            void                        unbind() const noexcept;

        private:
            DxInputElementSet           _inputElementSet;
            ComPtr<ID3D11InputLayout>   _inputLayout;
            ComPtr<ID3D10Blob>          _shaderBlob;
            ComPtr<ID3D11DeviceChild>   _shader;
            DxShaderType                _shaderType;
            std::string                 _hlslFileName;
            std::string                 _hlslBinaryFileName;
            std::string                 _entryPoint;

        public:
            static const DxShader       kNullInstance;
        };

        
        struct DxShaderCompileParam
        {
            const char*         _inputFileName = nullptr;
            const char*         _outputFileName = nullptr;
            const char*         _shaderIdentifier = nullptr;
            const char*         _shaderTextContent = nullptr;
        };

        class DxShaderPool final : public IDxObject
        {
            template <typename CustomDataType>
            using TypeMetaData = Language::TypeMetaData<CustomDataType>;
            using TypeCustomData = Language::CppHlsl::TypeCustomData;


            static constexpr const char* const  kCompiledShaderFileExtension = ".hlslbin";

        public:
                                        DxShaderPool(GraphicDevice* const graphicDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const DxShaderVersion shaderVersion);
                                        DxShaderPool(const DxShaderPool& rhs) = delete;
            virtual                     ~DxShaderPool() = default;

        public:
            const DxObjectId&           pushVertexShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const TypeMetaData<TypeCustomData>* const inputElementTypeMetaData);
            const DxObjectId&           pushNonVertexShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const DxShaderType shaderType);

        public:
            const DxObjectId&           pushVertexShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const TypeMetaData<TypeCustomData>* const inputElementTypeMetaData, const char* const outputDirectory = nullptr);
            const DxObjectId&           pushNonVertexShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const DxShaderType shaderType, const char* const outputDirectory = nullptr);

        private:
            const DxObjectId&           pushVertexShaderInternal(DxShader& shader, const TypeMetaData<TypeCustomData>* const inputElementTypeMetaData);
            const DxObjectId&           pushNonVertexShaderInternal(DxShader& shader, const DxShaderType shaderType);
        
        private:
            const bool                  createVertexShaderInternal(DxShader& shader, const TypeMetaData<TypeCustomData>* const inputElementTypeMetaData);
            void                        pushInputElement(DxInputElementSet& inputElementSet, const TypeMetaData<TypeCustomData>& outerDataTypeMetaData, const TypeMetaData<TypeCustomData>& memberTypeMetaData);
            const bool                  createNonVertexShaderInternal(DxShader& shader, const DxShaderType shaderType);

        private:
            const bool                  compileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const DxShaderType shaderType, const bool forceCompilation, DxShader& inoutShader);
            const bool                  compileShaderFromFile(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const DxShaderType shaderType, const bool forceCompilation, DxShader& inoutShader);
            const bool                  compileShaderInternalXXX(const DxShaderType shaderType, const DxShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob);

        public:
            void                        recompileAllShaders();

        private:
            void                        reportCompileError();

        public:
            void                        bindShaderIfNot(const DxShaderType shaderType, const DxObjectId& objectId);
            void                        unbindShader(const DxShaderType shaderType);
            
        private:
            const DxShader&             getShader(const DxShaderType shaderType, const DxObjectId& objectId);

        private:
            ComPtr<ID3DBlob>            _errorMessageBlob;
        
        private:
            DxShaderHeaderMemory*       _shaderHeaderMemory;

        private:
            DxShaderVersion             _shaderVersion;

        private:
            mint::Vector<DxShader>        _vertexShaderArray;
            mint::Vector<DxShader>        _geometryShaderArray;
            mint::Vector<DxShader>        _pixelShaderArray;

        private:
            DxObjectId                  _boundShaderIdArray[static_cast<uint32>(DxShaderType::COUNT)];
        };
    }
}


#endif // !MINT_DX_SHADER_H
