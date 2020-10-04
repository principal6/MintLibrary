#include <stdafx.h>
#include <Container/UniqueString.h>


namespace fs
{
	const UniqueStringA UniqueStringA::kInvalidUniqueString(nullptr, UniqueStringA::kInvalidIndex);
	UniqueStringA::UniqueStringA(const UniqueStringPoolA* const pool, const uint32 index)
		: _pool{ pool }
		, _index{ index }
	{
#if defined FS_DEBUG
		_str = _pool->getRawString(*this);
#endif
	}

	FS_INLINE const bool UniqueStringA::operator==(const UniqueStringA& rhs) const noexcept
	{
		return (_index == rhs._index);
	}

	FS_INLINE const bool UniqueStringA::operator!=(const UniqueStringA& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	FS_INLINE const char* UniqueStringA::c_str() const noexcept
	{
		return _pool->getRawString(*this);
	}


	UniqueStringPoolA::UniqueStringPoolA()
		: _rawMemory{ nullptr }
		, _rawCapacity{ 0 }
		, _totalLength{ 0 }
		, _count{ 0 }
	{
		reserve(kDefaultRawCapacity);
	}

	UniqueStringPoolA::~UniqueStringPoolA()
	{
		FS_DELETE_ARRAY(_rawMemory);
	}

	const UniqueStringA UniqueStringPoolA::registerString(const char* const rawString) noexcept
	{
		if (nullptr == rawString)
		{
			return UniqueStringA::kInvalidUniqueString;
		}

#if defined FS_DEBUG
		{
			bool isCollided = false;
			for (uint32 i = 0; i < _count; ++i)
			{
				const uint32 offset = _offsetArray[i];
				if (strcmp(&_rawMemory[offset], rawString) == 0)
				{
					isCollided = true;
					break;
				}
			}
			FS_ASSERT("김장원", isCollided == false, "이미 등록된 String 을 또 등록하고 있습니다!");
		}
#endif

		const uint32 lengthNullIncluded = static_cast<uint32>(strlen(rawString) + 1);
		if (_rawCapacity < _totalLength + lengthNullIncluded)
		{
			reserve(_rawCapacity * 2);
		}

		const uint32 newIndex = _count;
		_offsetArray[newIndex] = _totalLength;
		memcpy(&_rawMemory[_offsetArray[newIndex]], rawString, lengthNullIncluded - 1);

		_totalLength += lengthNullIncluded;
		++_count;

		return UniqueStringA(this, newIndex);
	}

	FS_INLINE const UniqueStringA UniqueStringPoolA::getString(const uint32 index) const noexcept
	{
		if (_count <= index)
		{
			return UniqueStringA::kInvalidUniqueString;
		}
		return UniqueStringA(this, index);
	}

	FS_INLINE const char* UniqueStringPoolA::getRawString(const UniqueStringA& uniqueString) const noexcept
	{
		if (uniqueString == UniqueStringA::kInvalidUniqueString)
		{
			return nullptr;
		}
		return &_rawMemory[_offsetArray[uniqueString._index]];;
	}

	void UniqueStringPoolA::reserve(const uint32 rawCapacity)
	{
		if (_rawCapacity < rawCapacity)
		{
			std::scoped_lock<std::mutex> scopedLock(_mutex);

			char* temp = FS_NEW_ARRAY(char, _rawCapacity);
			memcpy(temp, _rawMemory, sizeof(char) * _rawCapacity);
			FS_DELETE_ARRAY(_rawMemory);

			_rawMemory = FS_NEW_ARRAY(char, rawCapacity);
			memcpy(_rawMemory, temp, sizeof(char) * _rawCapacity);
			FS_DELETE_ARRAY(temp);

			_rawCapacity = rawCapacity;
			_offsetArray.resize(_rawCapacity);
		}
	}
}
