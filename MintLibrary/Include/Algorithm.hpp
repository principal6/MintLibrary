#pragma once


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


    template <typename T, typename ValueType, typename Evaluator>
    int32 binarySearchInternal(const Vector<T>& vec, const ValueType& value, Evaluator evaluator, const int32 indexBegin, const int32 indexEnd)
    {
        if (indexEnd <= indexBegin)
        {
            return indexBegin;
        }

        const int32 indexMiddle = indexBegin + (indexEnd - indexBegin) / 2;
        if (evaluator(vec[indexMiddle]) == value)
        {
            return indexMiddle;
        }
        else if (evaluator(vec[indexMiddle]) > value)
        {
            return binarySearchInternal(vec, value, evaluator, indexBegin, indexMiddle - 1); // indexMiddle 이 0 일 수 있다!
        }
        else
        {
            return binarySearchInternal(vec, value, evaluator, indexMiddle + 1, indexEnd);
        }
    }

    template<typename T, typename ValueType, typename Evaluator>
    int32 binarySearch(const Vector<T>& vec, const ValueType& value, Evaluator evaluator)
    {
        if (vec.empty() == true)
        {
            return kInt32Max;
        }
        return binarySearchInternal(vec, value, evaluator, 0, static_cast<int32>(vec.size() - 1));
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
            while (left < pivot && comparator(vector[left], vector[pivot]))
            {
                ++left;
            }

            // 모든 entry 에 대해 { cmp(entry, pivot) == true } 이다.
            // 예) 3, 2, 1, 4, 5
            if (left == pivot)
            {
                break;
            }

            while (right >= front && comparator(vector[pivot], vector[right])) 
            {
                --right;
            }

            // 모든 entry 에 대해 { cmp(pivot, entry) == true } 이다.
            // 예) 7, 9, 8, 6, 4
            if (right < front)
            {
                std::swap(vector[left], vector[pivot]);
                pivot = left;
                break;
            }

            // left 및 right 의 모든 entry 에 대해 cmp 가 완료되었다.
            if (right <= left)
            {
                if (left == right)
                {
                    std::swap(vector[right + 1], vector[pivot]);
                    pivot = right + 1;
                }
                else
                {
                    std::swap(vector[left], vector[pivot]);
                    pivot = left;
                }
                break;
            }

            std::swap(vector[left], vector[right]);
            ++left;
            --right;
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
