#include <stdafx.h>
#include <MintRenderingBase/Include/GUI/ControlData.h>

#include <MintContainer/Include/ScopeString.hpp>
#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
    namespace Rendering
    {
        const ControlID ControlData::generateID(const FileLine& fileLine, const ControlType type, const wchar_t* const text)
        {
            ScopeStringA<512> file = fileLine._file;
            ScopeStringW<512> key;
            StringUtil::convertScopeStringAToScopeStringW(file, key);
            key.append(L"_");
            key.append(StringUtil::convertToStringW(fileLine._line).c_str());
            key.append(L"_");
            key.append(text);
            key.append(L"_");
            key.append(StringUtil::convertToStringW(static_cast<uint32>(type)).c_str());
            return ControlID(key.computeHash());
        }
    }
}
