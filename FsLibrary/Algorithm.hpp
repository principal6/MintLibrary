#pragma once


#include <stdafx.h>
#include <Algorithm.h>


namespace fs
{
	template <typename T>
	const uint32 binarySearchInternal(const std::vector<T>& vec, const T& value, const uint32 indexBegin, const uint32 indexEnd)
	{
		if (indexBegin == indexEnd)
		{
			return indexBegin;
		}

		const uint32 indexMiddle = indexBegin + (indexEnd - indexBegin) / 2;
		if (value == vec[indexMiddle])
		{
			return indexMiddle;
		}
		else if (value < vec[indexMiddle])
		{
			return binarySearchInternal(vec, value, indexBegin, indexMiddle - 1);
		}
		else
		{
			return binarySearchInternal(vec, value, indexMiddle + 1, indexEnd);
		}
	}

	template <typename T>
	const uint32 binarySearch(const std::vector<T>& vec, const T& value)
	{
		if (vec.empty() == true)
		{
			return kUint32Max;
		}
		return binarySearchInternal(vec, value, 0, static_cast<uint32>(vec.size() - 1));
	}


	template <typename T, typename CompT>
	const uint32 binarySearchInternal(const std::vector<T>& vec, const CompT& value, const uint32 indexBegin, const uint32 indexEnd)
	{
		if (indexBegin == indexEnd)
		{
			return indexBegin;
		}

		const uint32 indexMiddle = indexBegin + (indexEnd - indexBegin) / 2;
		if (vec[indexMiddle] == value)
		{
			return indexMiddle;
		}
		else if (vec[indexMiddle] > value)
		{
			return binarySearchInternal(vec, value, indexBegin, indexMiddle - 1);
		}
		else
		{
			return binarySearchInternal(vec, value, indexMiddle + 1, indexEnd);
		}
	}

	template<typename T, typename CompT>
	const uint32 binarySearch(const std::vector<T>& vec, const CompT& value)
	{
		if (vec.empty() == true)
		{
			return kUint32Max;
		}
		return binarySearchInternal(vec, value, 0, static_cast<uint32>(vec.size() - 1));
	}

	template <typename T>
	void stableDelete(std::vector<T>& vec, const uint32 index)
	{
		const uint32 count = static_cast<uint32>(vec.size());
		if (index < count)
		{
			if (index == count - 1)
			{
				vec.pop_back();
				return;
			}

			for (uint32 iter = index; iter < count - 1; ++iter)
			{
				vec[iter] = std::move(vec[iter + 1]);
			}
			vec.pop_back();
		}
	}
}
