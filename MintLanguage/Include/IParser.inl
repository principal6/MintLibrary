#pragma once


namespace mint
{
    namespace Language
    {
        MINT_INLINE constexpr const char* IParser::convertErrorTypeToTypeString(const ErrorType errorType)
        {
            return kErrorTypeStringArray[static_cast<uint32>(errorType)][0];
        }

        MINT_INLINE constexpr const char* IParser::convertErrorTypeToContentString(const ErrorType errorType)
        {
            return kErrorTypeStringArray[static_cast<uint32>(errorType)][1];
        }
    }
}
