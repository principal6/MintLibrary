#pragma once


#include <stdafx.h>
#include <MintLibrary/Include/Algorithm.h>

#include <MintContainer/Include/Vector.hpp>


namespace mint
{
    template <typename T>
    int32 binarySearchInternal(const Vector<T>& vec, const T& value, const int32 indexBegin, const int32 indexEnd)
    {
        if (indexEnd <= indexBegin)
        {
            return indexBegin;
        }

        const int32 indexMiddle = indexBegin + (indexEnd - indexBegin) / 2;
        if (value == vec[indexMiddle])
        {
            return indexMiddle;
        }
        else if (value < vec[indexMiddle])
        {
            return binarySearchInternal(vec, value, indexBegin, indexMiddle - 1); // indexMiddle 이 0 일 수 있다!
        }
        else
        {
            return binarySearchInternal(vec, value, indexMiddle + 1, indexEnd);
        }
    }

    template <typename T>
    int32 binarySearch(const Vector<T>& vec, const T& value)
    {
        if (vec.empty() == true)
        {
            return kInt32Max;
        }
        return binarySearchInternal(vec, value, 0, static_cast<int32>(vec.size() - 1));
    }


    template <typename T, typename CompT>
    int32 binarySearchInternal(const Vector<T>& vec, const CompT& value, const int32 indexBegin, const int32 indexEnd)
    {
        if (indexEnd <= indexBegin)
        {
            return indexBegin;
        }

        const int32 indexMiddle = indexBegin + (indexEnd - indexBegin) / 2;
        if (vec[indexMiddle] == value)
        {
            return indexMiddle;
        }
        else if (vec[indexMiddle] > value)
        {
            return binarySearchInternal(vec, value, indexBegin, indexMiddle - 1); // indexMiddle 이 0 일 수 있다!
        }
        else
        {
            return binarySearchInternal(vec, value, indexMiddle + 1, indexEnd);
        }
    }

    template<typename T, typename CompT>
    int32 binarySearch(const Vector<T>& vec, const CompT& value)
    {
        if (vec.empty() == true)
        {
            return kInt32Max;
        }
        return binarySearchInternal(vec, value, 0, static_cast<int32>(vec.size() - 1));
    }

    template<typename T, typename Comparator>
    void quickSortInternal(Vector<T>& vector, const int32 front, const int32 back, Comparator comparator)
    {
        if (back <= front)
        {
            return;
        }

        int32 pivot = back;
        int32 left = front;
        int32 right = back - 1;
        while (true)
        {
            while (left <= back && comparator(vector[left], vector[pivot]) == true) { ++left; }
            while (front <= right && comparator(vector[pivot], vector[right]) == true) { --right; }

            // left, right 가 전혀 움직이지 않은 경우.
            if (left == front && right == back - 1)
            {
                break;
            }

            // left - 1 까지 전부 { cmp(left, pivot) == true } 고, right + 1 부터 { cmp(pivot, right) == true } 인 경우
            if (right <= left)
            {
                break;
            }

            std::swap(vector[left], vector[right]);
        }

        if (left != pivot && comparator(vector[pivot], vector[left]) == true)
        {
            std::swap(vector[left], vector[pivot]);
            std::swap(left, pivot);
        }

        quickSortInternal(vector, front, pivot - 1, comparator);
        quickSortInternal(vector, pivot, back, comparator);
    }

    template<typename T, typename Comparator>
    void quickSort(Vector<T>& vector, Comparator comparator)
    {
        const int32 begin = 0;
        const int32 end = static_cast<int32>(vector.size() - 1);
        quickSortInternal(vector, begin, end, comparator);
    }
}
