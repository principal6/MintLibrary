#pragma once


#include <CommonDefinitions.h>


namespace fs
{
    template <typename T>
    const int32 binarySearch(const std::vector<T>& vec, const T& value);

    template <typename T, typename CompT>
    const int32 binarySearch(const std::vector<T>& vec, const CompT& value);

    template <typename T>
    void stableDelete(std::vector<T>& vec, const uint32 index);
}
