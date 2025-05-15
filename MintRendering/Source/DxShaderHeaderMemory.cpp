#include <MintRendering/Include/DxShaderHeaderMemory.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StackString.hpp>


namespace mint
{
	namespace Rendering
	{
		void DxShaderHeaderMemory::PushHeader(const char* const name, const char* const content)
		{
			_fileNameArray.PushBack(name);
			_fileContentArray.PushBack(content);
		}

		HRESULT DxShaderHeaderMemory::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
		{
			const uint32 fileCount = static_cast<uint32>(_fileNameArray.Size());
			for (uint32 fileIndex = 0; fileIndex < fileCount; ++fileIndex)
			{
				if (_fileNameArray[fileIndex] == pFileName)
				{
					*ppData = _fileContentArray[fileIndex].CString();
					*pBytes = static_cast<UINT>(_fileContentArray[fileIndex].Length());
					return S_OK;
				}
			}
			StackStringA<kMaxPath> asssertMessage{ "셰이더 파일명을 찾지 못했습니다! 파일명:" };
			asssertMessage.Append(pFileName);
			MINT_ASSERT(false, asssertMessage.CString());
			return E_FAIL;
		}

		HRESULT DxShaderHeaderMemory::Close(LPCVOID pData)
		{
			return S_OK;
		}
	}
}
