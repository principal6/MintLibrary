#pragma once


#include <Container/UniqueString.h>


namespace fs
{
	template<uint32 Capacity>
	inline UniqueStringAHolder<Capacity>::UniqueStringAHolder()
		: _totalLength{ 0 }
		, _count{ 0 }
	{
		memset(_raw, 0, Capacity);
		memset(_offsetArray, 0, Capacity);
	}

	template<uint32 Capacity>
	inline UniqueStringAHolder<Capacity>::~UniqueStringAHolder()
	{
		__noop;
	}

	template<uint32 Capacity>
	inline const uint32 UniqueStringAHolder<Capacity>::registerString(const char* const rawString) noexcept
	{
		if (nullptr == rawString)
		{
			return kUniqueStringInvalidIndex;
		}

		for (uint32 i = 0; i < _count; ++i)
		{
			const uint32 offset = _offsetArray[i];
			if (0 == strcmp(&_raw[offset], rawString))
			{
				return i;
			}
		}

		const uint32 newIndex = _count;
		_offsetArray[newIndex] = _totalLength;
		const uint32 lengthNullIncluded = strlen(rawString) + 1;
		if (Capacity < _totalLength + lengthNullIncluded)
		{
			FS_ASSERT("김장원", false, "String 등록에 실패했습니다. Holder 의 capacity 를 더 늘려주세요!");
			return kUniqueStringInvalidIndex;
		}

		memcpy(&_raw[_offsetArray[newIndex]], rawString, lengthNullIncluded - 1);

		_totalLength += lengthNullIncluded;
		++_count;
		return newIndex;
	}

	template<uint32 Capacity>
	inline const char* UniqueStringAHolder<Capacity>::getString(const uint32 index) const noexcept
	{
		if (_count <= index)
		{
			return nullptr;
		}
		return &_raw[_offsetArray[index]];
	}

	template<uint32 HolderCapacity>
	inline UniqueStringA<HolderCapacity>::UniqueStringA()
		: _holder{ UniqueStringAHolder<HolderCapacity>::getInstance() }
		, _index{ kUniqueStringInvalidIndex }
	{
		__noop;
	}

	template<uint32 HolderCapacity>
	inline UniqueStringA<HolderCapacity>::UniqueStringA(const char* const rawString)
		: _holder{ UniqueStringAHolder<HolderCapacity>::getInstance() }
		, _index{ _holder->registerString(rawString) }
	{
#if defined FS_DEBUG
		setDebugString();
#endif
	}

	template<uint32 HolderCapacity>
	inline UniqueStringA<HolderCapacity>::UniqueStringA(const UniqueStringA& rhs)
		: _holder{ rhs._holder }
		, _index{ rhs._index }
	{
#if defined FS_DEBUG
		setDebugString();
#endif
	}

	template<uint32 HolderCapacity>
	inline UniqueStringA<HolderCapacity>::UniqueStringA(UniqueStringA&& rhs) noexcept
		: _holder{ rhs._holder }
		, _index{ rhs._index }
	{
#if defined FS_DEBUG
		setDebugString();
#endif

		rhs._index = kUniqueStringInvalidIndex;
	}

	template<uint32 HolderCapacity>
	inline UniqueStringA<HolderCapacity>::~UniqueStringA()
	{
		__noop;
	}

	template<uint32 HolderCapacity>
	inline UniqueStringA<HolderCapacity>& UniqueStringA<HolderCapacity>::operator=(const UniqueStringA& rhs)
	{
		if (this != &rhs)
		{
			_index = rhs._index;

#if defined FS_DEBUG
			setDebugString();
#endif
		}
		return *this;
	}

	template<uint32 HolderCapacity>
	inline UniqueStringA<HolderCapacity>& UniqueStringA<HolderCapacity>::operator=(UniqueStringA&& rhs) noexcept
	{
		if (this != &rhs)
		{
			_index = rhs._index;

#if defined FS_DEBUG
			setDebugString();
#endif

			rhs._index = kUniqueStringInvalidIndex;
		}
		return *this;
	}

	template<uint32 HolderCapacity>
	inline const bool UniqueStringA<HolderCapacity>::operator==(const UniqueStringA& rhs)
	{
		return (_index == rhs._index);
	}

	template<uint32 HolderCapacity>
	inline const bool UniqueStringA<HolderCapacity>::operator!=(const UniqueStringA& rhs)
	{
		return (_index != rhs._index);
	}

	template<uint32 HolderCapacity>
	inline bool UniqueStringA<HolderCapacity>::isValid() const noexcept
	{
		return (_index != kUniqueStringInvalidIndex);
	}

	template<uint32 HolderCapacity>
	inline bool UniqueStringA<HolderCapacity>::assign(const char* const rawString) noexcept
	{
		if (_index != kUniqueStringInvalidIndex)
		{
			// 이미 _index 가 지정된 UniqueString 에 assign() 을 하면
			// 이 문자열을 영영 잃을 수도 있다는 뜻이다...
			return false;
		}

		_index = _holder->registerString(rawString);

#if defined FS_DEBUG
		setDebugString();
#endif

		return isValid();
	}

	template<uint32 HolderCapacity>
	inline const char* UniqueStringA<HolderCapacity>::c_str() const noexcept
	{
		return _holder->getString(_index);
	}

#if defined FS_DEBUG
	template<uint32 HolderCapacity>
	inline void UniqueStringA<HolderCapacity>::setDebugString() noexcept
	{
		_str = _holder->getString(_index);
	}
#endif
}
