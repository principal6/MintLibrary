#pragma once


#ifndef _MINT_RENDERING_BASE_DX_SHADER_HEADER_MEMORY_H_
#define _MINT_RENDERING_BASE_DX_SHADER_HEADER_MEMORY_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/String.h>

#include <d3dcommon.h>


namespace mint
{
	namespace Rendering
	{
		class DxShaderHeaderMemory : public ID3DInclude
		{
		public:
			DxShaderHeaderMemory() = default;
			virtual ~DxShaderHeaderMemory() = default;

		public:
			void pushHeader(const char* const name, const char* const content);

		public:
			HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override final;
			HRESULT Close(LPCVOID pData) override final;

		private:
			Vector<StringA> _fileNameArray;
			Vector<StringA> _fileContentArray;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_DX_SHADER_HEADER_MEMORY_H_
