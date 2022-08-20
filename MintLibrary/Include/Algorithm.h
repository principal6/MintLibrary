#pragma once


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
    template <typename T>
    struct ComparatorAscending final { bool operator()(const T& lhs, const T& rhs) const noexcept { return lhs < rhs; } };
    
    template <typename T>
    struct ComparatorDescending final { bool operator()(const T& lhs, const T& rhs) const noexcept { return lhs > rhs; } };


    template <typename T>
    int32 binarySearch(const Vector<T>& vec, const T& value);

    template <typename T, typename ValueType, typename Evaluator>
    int32 binarySearch(const Vector<T>& vec, const ValueType& value, Evaluator evaluator);

    template <typename T, typename Comparator>
    void quickSort(Vector<T>& vector, Comparator comparator);
}
