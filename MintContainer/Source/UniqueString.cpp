#include <MintContainer/Include/UniqueString.h>
#include <MintContainer/Include/UniqueString.hpp>


namespace mint
{
#pragma region Static member variables
    const UniqueStringID<char> UniqueString<char>::kInvalidID;
    const UniqueStringID<wchar_t> UniqueString<wchar_t>::kInvalidID;
    
    UniqueStringPool<char> UniqueString<char>::_pool;
    UniqueStringPool<wchar_t> UniqueString<wchar_t>::_pool;
#pragma endregion
}
