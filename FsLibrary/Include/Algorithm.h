#pragma once


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsContainer/Include/Vector.h>


namespace fs
{
    template <typename T>
    struct ComparatorAscending final { const bool operator()(const T& lhs, const T& rhs) const noexcept { return lhs < rhs; } };
    
    template <typename T>
    struct ComparatorDescending final { const bool operator()(const T& lhs, const T& rhs) const noexcept { return lhs > rhs; } };


    template <typename T>
    const int32 binarySearch(const fs::Vector<T>& vec, const T& value);

    template <typename T, typename CompT>
    const int32 binarySearch(const fs::Vector<T>& vec, const CompT& value);

    template <typename T, typename Comparator>
    void quickSort(fs::Vector<T>& vector, Comparator comparator);
}
