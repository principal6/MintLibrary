#include <stdafx.h>
#include <Container/UniqueString.h>
#include <Container/UniqueString.hpp>

#include <Container/StringUtil.h>


namespace fs
{
	const UniqueStringAId UniqueStringA::kInvalidId;
	UniqueStringPoolA UniqueStringA::_pool;
	UniqueStringA::UniqueStringA()
		: _id{ UniqueStringA::kInvalidId }
#if defined FS_DEBUG
		, _str{}
#endif
	{
		__noop;
	}

	UniqueStringA::UniqueStringA(const char* const rawString)
		: _id{ _pool.registerString(rawString) }
#if defined FS_DEBUG
		, _str{ _pool.getRawString(_id) }
#endif
	{
		__noop;
	}

#if defined FS_UNIQUE_STRING_EXPOSE_ID
	UniqueStringA::UniqueStringA(const UniqueStringAId id)
		: _id{ (true == _pool.isValid(id)) ? id : UniqueStringA::kInvalidId }
#if defined FS_DEBUG
		, _str{ _pool.getRawString(_id) }
#endif
	{
		__noop;
	}
#endif


	UniqueStringPoolA::UniqueStringPoolA()
		: _rawMemory{ nullptr }
		, _rawCapacity{ 0 }
		, _totalLength{ 0 }
		, _uniqueStringCount{ 0 }
	{
		reserve(kDefaultRawCapacity);
	}

	UniqueStringPoolA::~UniqueStringPoolA()
	{
		FS_DELETE_ARRAY(_rawMemory);
	}

	const UniqueStringAId UniqueStringPoolA::registerString(const char* const rawString) noexcept
	{
		if (nullptr == rawString)
		{
			return UniqueStringA::kInvalidId;
		}

		const uint64 hash = StringUtil::hashRawString64(rawString);
		{
			auto found = _registrationMap.find(hash);
			if (found != _registrationMap.end())
			{
				return found->second;
			}
		}

		const uint32 lengthNullIncluded = static_cast<uint32>(strlen(rawString) + 1);
		if (_rawCapacity < _totalLength + lengthNullIncluded)
		{
			reserve(_rawCapacity * 2);
		}

		UniqueStringAId newId(_uniqueStringCount);
		_offsetArray[newId._rawId] = _totalLength;
		memcpy(&_rawMemory[_offsetArray[newId._rawId]], rawString, lengthNullIncluded - 1);

		_totalLength += lengthNullIncluded;
		++_uniqueStringCount;
		_registrationMap.insert(std::make_pair(hash, newId));

		return newId;
	}

	void UniqueStringPoolA::reserve(const uint32 rawCapacity)
	{
		std::scoped_lock<std::mutex> scopedLock(_mutex);

		if (_rawCapacity < rawCapacity)
		{
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
