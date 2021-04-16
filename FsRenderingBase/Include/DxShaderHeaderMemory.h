#pragma once


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsContainer/Include/Vector.h>

#include <d3dcommon.h>


namespace fs
{
    namespace RenderingBase
    {
        class DxShaderHeaderMemory : public ID3DInclude
        {
        public:
                                                DxShaderHeaderMemory() = default;
            virtual                             ~DxShaderHeaderMemory() = default;
        
        public:
            void                                pushHeader(const char* const name, const char* const content);

        public:
            HRESULT                             Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override final;
            HRESULT                             Close(LPCVOID pData) override final;

        private:
            fs::Vector<std::string>             _fileNameArray;
            fs::Vector<std::string>             _fileContentArray;
        };
    }
}
