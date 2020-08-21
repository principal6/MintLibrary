#pragma once


#ifndef FS_STATIC_ARRAY_H
#define FS_STATIC_ARRAY_H


#include <CommonDefinitions.h>
#include <initializer_list>
#include <utility>


namespace fs
{
	template <typename T, uint32 maxSize>
	class StaticArray
	{
	public:
		constexpr				StaticArray() = default;
		constexpr				StaticArray(const std::initializer_list<T>& il)
		{
			for (size_t index = 0; index < il.size(); ++index)
			{
				_data[index] = *(il.begin() + index);
			}
		}
		constexpr				StaticArray(T value)
		{
			_data[0] = value;
		}
		constexpr				StaticArray(const T& s)
		{
			for (uint32 index = 0; index < maxSize; ++index)
			{
				_data[index] = s;
			}
		}
		// Compiler checks overflow
		constexpr				StaticArray(const T* const arr, const uint32 arrSize)
		{
			for (uint32 index = 0; index < arrSize; ++index)
			{
				_data[index] = arr[index];
			}
		}
		constexpr				StaticArray(const StaticArray& rhs)
		{
			for (uint32 index = 0; index < maxSize; ++index)
			{
				_data[index] = rhs._data[index];
			}
		}
		constexpr				StaticArray(StaticArray&& rhs) noexcept
		{
			std::swap(_data, rhs._data);
		}
								~StaticArray() = default;

	public:
		constexpr StaticArray&	operator=(const StaticArray& rhs)
		{
			for (uint32 index = 0; index < maxSize; ++index)
			{
				_data[index] = rhs._data[index];
			}
			return *this;
		}
		constexpr StaticArray&	operator=(StaticArray&& rhs) noexcept
		{
			if (this != &rhs)
			{
				std::swap(_data, rhs._data);
			}
			return *this;
		}

	public:
		constexpr T&			operator[](const uint32 index) noexcept
		{
			FS_ASSERT("김장원", index < maxSize, "범위를 벗어난 접근입니다.");
			return _data[index];
		}
		constexpr T				operator[](const uint32 index) const noexcept
		{
			FS_ASSERT("김장원", index < maxSize, "범위를 벗어난 접근입니다.");
			return _data[index];
		}

	public:
		constexpr uint32		size() const noexcept { return maxSize; }

	private:
		T						_data[maxSize]{};
	};
}


#endif // !FS_STATIC_ARRAY_H
