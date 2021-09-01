#include <stdafx.h>
#include <MintRenderingBase/Include/DxShaderHeaderMemory.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/ScopeString.hpp>


namespace mint
{
    namespace Rendering
    {
        void DxShaderHeaderMemory::pushHeader(const char* const name, const char* const content)
        {
            _fileNameArray.push_back(name);
            _fileContentArray.push_back(content);
        }

        HRESULT DxShaderHeaderMemory::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
        {
            const uint32 fileCount = static_cast<uint32>(_fileNameArray.size());
            for (uint32 fileIndex = 0; fileIndex < fileCount; ++fileIndex)
            {
                if (_fileNameArray[fileIndex] == pFileName)
                {
                    *ppData = _fileContentArray[fileIndex].c_str();
                    *pBytes = static_cast<UINT>(_fileContentArray[fileIndex].length());
                    return S_OK;
                }
            }
            mint::ScopeStringA<kMaxPath> asssertMessage{ "셰이더 파일명을 찾지 못했습니다! 파일명:" };
            asssertMessage.append(pFileName);
            MINT_ASSERT("김장원", false, asssertMessage.c_str());
            return E_FAIL;
        }

        HRESULT DxShaderHeaderMemory::Close(LPCVOID pData)
        {
            return S_OK;
        }
    }
}
