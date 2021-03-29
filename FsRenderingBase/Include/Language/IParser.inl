#pragma once


namespace fs
{
    namespace Language
    {
        FS_INLINE constexpr const char* IParser::convertErrorTypeToTypeString(const ErrorType errorType)
        {
            return kErrorTypeStringArray[static_cast<uint32>(errorType)][0];
        }

        FS_INLINE constexpr const char* IParser::convertErrorTypeToContentString(const ErrorType errorType)
        {
            return kErrorTypeStringArray[static_cast<uint32>(errorType)][1];
        }
    }
}
