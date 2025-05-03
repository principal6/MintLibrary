#pragma once


#ifndef _MINT_LIBRARY_ALGORITHM_H_
#define _MINT_LIBRARY_ALGORITHM_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
	template <typename T>
	struct ComparatorAscending final { bool operator()(const T& lhs, const T& rhs) const noexcept { return lhs < rhs; } };

	template <typename T>
	struct ComparatorDescending final { bool operator()(const T& lhs, const T& rhs) const noexcept { return lhs > rhs; } };


	template <typename T>
	uint32 BinarySearch(const Vector<T>& vec, const T& value);

	template <typename T, typename ValueType, typename Evaluator>
	uint32 BinarySearch(const Vector<T>& vec, const ValueType& value, Evaluator evaluator);

	template <typename T, typename Comparator>
	void QuickSort(Vector<T>& vector, Comparator comparator);
	
	template <typename T, typename Comparator>
	void QuickSort(T* arrayPointer, uint32 arraySize, Comparator comparator);
}


#endif // !_MINT_LIBRARY_ALGORITHM_H_
