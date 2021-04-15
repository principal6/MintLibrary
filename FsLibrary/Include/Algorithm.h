#pragma once


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsContainer/Include/Vector.h>


namespace fs
{
    template <typename T>
    const int32 binarySearch(const fs::Vector<T>& vec, const T& value);

    template <typename T, typename CompT>
    const int32 binarySearch(const fs::Vector<T>& vec, const CompT& value);
}
