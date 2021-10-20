#pragma once


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
    template <typename T>
    struct ComparatorAscending final { const bool operator()(const T& lhs, const T& rhs) const noexcept { return lhs < rhs; } };
    
    template <typename T>
    struct ComparatorDescending final { const bool operator()(const T& lhs, const T& rhs) const noexcept { return lhs > rhs; } };


    template <typename T>
    const int32 binarySearch(const Vector<T>& vec, const T& value);

    template <typename T, typename CompT>
    const int32 binarySearch(const Vector<T>& vec, const CompT& value);

    template <typename T, typename Comparator>
    void quickSort(Vector<T>& vector, Comparator comparator);
}
