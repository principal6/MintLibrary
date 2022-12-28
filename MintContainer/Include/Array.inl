#pragma once


namespace mint
{
	template <typename T, uint32 maxSize>
	inline constexpr Array<T, maxSize>::Array(const std::initializer_list<T>& il)
	{
		const size_t count = il.size();
		const T* const first = il.begin();
		for (size_t index = 0; index < count; ++index)
		{
			_data[index] = *(first + index);
		}
	}

	template<typename T, uint32 maxSize>
	inline constexpr Array<T, maxSize>::Array(T value)
	{
		_data[0] = value;
	}

	template<typename T, uint32 maxSize>
	inline constexpr Array<T, maxSize>::Array(const T& s)
	{
		for (uint32 index = 0; index < maxSize; ++index)
		{
			_data[index] = s;
		}
	}

	template<typename T, uint32 maxSize>
	inline constexpr Array<T, maxSize>::Array(const T* const arr, const uint32 arrSize)
	{
		for (uint32 index = 0; index < arrSize; ++index)
		{
			_data[index] = arr[index];
		}
	}

	template<typename T, uint32 maxSize>
	inline constexpr Array<T, maxSize>::Array(const Array& rhs)
	{
		for (uint32 index = 0; index < maxSize; ++index)
		{
			_data[index] = rhs._data[index];
		}
	}

	template<typename T, uint32 maxSize>
	inline constexpr Array<T, maxSize>::Array(Array&& rhs) noexcept
	{
		std::swap(_data, rhs._data);
	}

	template<typename T, uint32 maxSize>
	inline constexpr Array<T, maxSize>& Array<T, maxSize>::operator=(const Array& rhs)
	{
		for (uint32 index = 0; index < maxSize; ++index)
		{
			_data[index] = rhs._data[index];
		}
		return *this;
	}

	template<typename T, uint32 maxSize>
	inline constexpr Array<T, maxSize>& Array<T, maxSize>::operator=(Array&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::swap(_data, rhs._data);
		}
		return *this;
	}

	template<typename T, uint32 maxSize>
	inline constexpr T& Array<T, maxSize>::operator[](const uint32 index) noexcept
	{
		MINT_ASSERT(index < maxSize, "범위를 벗어난 접근입니다.");
		return _data[index];
	}

	template<typename T, uint32 maxSize>
	inline constexpr const T& Array<T, maxSize>::operator[](const uint32 index) const noexcept
	{
		MINT_ASSERT(index < maxSize, "범위를 벗어난 접근입니다.");
		return _data[index];
	}

	template<typename T, uint32 maxSize>
	inline constexpr uint32 Array<T, maxSize>::size() const noexcept
	{
		return maxSize;
	}
}
