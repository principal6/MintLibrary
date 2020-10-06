#include <stdafx.h>
#include <Container/DynamicString.h>

#include <Container/StringUtil.h>
#include <Memory/MemoryAllocator.hpp>


namespace fs
{
	MemoryAllocator DynamicStringA::_memoryAllocator;
	DynamicStringA::DynamicStringA()
		: _length{ 0 }
		, _cachedRawMemoryPtr{ nullptr }
		, _cachedHash{ 0 }
		, _cachedByteCapacity{ 0 }
	{
		__noop;
	}

	DynamicStringA::DynamicStringA(const uint32 capacity)
		: DynamicStringA()
	{
		_memoryAccessor = _memoryAllocator.allocate(fs::max(capacity, kMinCapacity));
		cacheRawMemoryInternal();
	}

	DynamicStringA::DynamicStringA(const char* const rawString)
		: DynamicStringA()
	{
		if (fs::StringUtil::isNullOrEmpty(rawString) == true)
		{
			return;
		}

		_length = fs::StringUtil::strlen(rawString);
		const uint32 newCapacity = _length + 1;
		_memoryAccessor = _memoryAllocator.allocate(fs::max(newCapacity, kMinCapacity));
		setMemoryInternal(rawString, _length, 0);
	}

	DynamicStringA::DynamicStringA(const char* const rawString, const uint32 rawStringLength)
		: DynamicStringA()
	{
		if (fs::StringUtil::isNullOrEmpty(rawString) == true)
		{
			return;
		}

		_length = rawStringLength;
		const uint32 newCapacity = _length + 1;
		_memoryAccessor = _memoryAllocator.allocate(fs::max(newCapacity, kMinCapacity));
		setMemoryInternal(rawString, _length, 0);
	}

	DynamicStringA::DynamicStringA(const DynamicStringA& rhs)
		: DynamicStringA()
	{
		if (0 == rhs._length)
		{
			return;
		}

		_length = rhs._length;

		const uint32 newCapacity = _length + 1;
		_memoryAccessor = _memoryAllocator.allocate(fs::max(newCapacity, kMinCapacity));
		setMemoryInternal(rhs.c_str(), _length, 0);
	}

	DynamicStringA::~DynamicStringA()
	{
		__noop;
	}

	DynamicStringA& DynamicStringA::operator=(const char* const rawString)
	{
		assign(rawString);
		return *this;
	}

	DynamicStringA& DynamicStringA::operator=(const DynamicStringA& rhs)
	{
		assign(rhs);
		return *this;
	}

	DynamicStringA& DynamicStringA::operator+=(const char* const rawString) noexcept
	{
		append(rawString);
		return *this;
	}

	DynamicStringA& DynamicStringA::operator+=(const DynamicStringA& rhs) noexcept
	{
		append(rhs);
		return *this;
	}

	const bool DynamicStringA::operator==(const char* const rawString) const noexcept
	{
		return compare(rawString);
	}

	const bool DynamicStringA::operator==(const DynamicStringA& rhs) const noexcept
	{
		return compare(rhs);
	}

	const bool DynamicStringA::operator!=(const char* const rawString) const noexcept
	{
		return !compare(rawString);
	}

	const bool DynamicStringA::operator!=(const DynamicStringA& rhs) const noexcept
	{
		return !compare(rhs);
	}

	void DynamicStringA::clear()
	{
		_length = 0;
		setMemoryInternal(nullptr, 0, 0);
	}

	void DynamicStringA::assign(const char* const rawString)
	{
		assign(rawString, fs::StringUtil::strlen(rawString));
	}

	void DynamicStringA::assign(const char* const rawString, const uint32 rawStringLength)
	{
		if (fs::StringUtil::isNullOrEmpty(rawString) == true)
		{
			return;
		}

		_length = rawStringLength;
		const uint32 newCapacity = _length + 1;
		if (_memoryAccessor.isValid() == true)
		{
			if (_length < _memoryAccessor.getByteCapacity())
			{
				setMemoryInternal(rawString, _length, 0);
			}
			else
			{
				_memoryAllocator.reallocate(_memoryAccessor, newCapacity, false);
				setMemoryInternal(rawString, _length, 0);
			}
			return;
		}

		_memoryAccessor = _memoryAllocator.allocate(fs::max(newCapacity, kMinCapacity));
		setMemoryInternal(rawString, _length, 0);
	}

	void DynamicStringA::assign(const DynamicStringA& rhs)
	{
		if (this != &rhs)
		{
			_length = rhs._length;

			const uint32 newCapacity = _length + 1;
			_memoryAccessor = _memoryAllocator.allocate(fs::max(newCapacity, kMinCapacity));
			setMemoryInternal(rhs.c_str(), _length, 0);
		}
	}

	void DynamicStringA::append(const char* const rawString)
	{
		if (fs::StringUtil::isNullOrEmpty(rawString) == true)
		{
			return;
		}

		const uint32 capacity = _memoryAccessor.getByteCapacity();
		const uint32 deltaLength =fs::StringUtil::strlen(rawString);
		const uint32 capacityCandidate = _length + deltaLength + 1;
		if (capacity <= capacityCandidate)
		{
			_memoryAllocator.reallocate(_memoryAccessor, fs::max(capacity * 2, capacityCandidate), true);
		}
		
		const uint32 offset = _length;
		_length += deltaLength;
		setMemoryInternal(rawString, _length, offset);
	}

	void DynamicStringA::append(const DynamicStringA& rhs)
	{
		append(rhs.c_str());
	}

	DynamicStringA DynamicStringA::substr(const uint32 offset, const uint32 count) const noexcept
	{
		const char* const myRaw = c_str();
		const uint32 subStringLength = fs::min(count, _length - offset - 1);
		if (_length <= offset)
		{
			return DynamicStringA();
		}
		return DynamicStringA(myRaw + offset, subStringLength);
	}

	void DynamicStringA::setChar(const uint32 at, const char ch)
	{
		char str[2]{ ch, 0 };
		setMemoryInternal(str, 1, at, true);
	}

	const bool DynamicStringA::empty() const noexcept
	{
		return (_length == 0);
	}

	const uint32 DynamicStringA::length() const noexcept
	{
		return _length;
	}

	const char* const DynamicStringA::c_str() const noexcept
	{
		return _cachedRawMemoryPtr;
	}

	const char DynamicStringA::getChar(const uint32 at) const noexcept
	{
		if (_length <= at)
		{
			return 0;
		}
		return c_str()[at];
	}

	const uint32 DynamicStringA::find(const char* const rawString, const uint32 offset) const noexcept
	{
		const uint32 rawStringLength = static_cast<uint32>(strlen(rawString));
		if (_length < rawStringLength)
		{
			return kStringNPos;
		}

		const char* const myRaw = c_str();
		for (uint32 myAt = offset; myAt < _length; ++myAt)
		{
			uint32 rawStringAt = 0;
			while (rawStringAt < rawStringLength)
			{
				if (myRaw[myAt + rawStringAt] != rawString[rawStringAt])
				{
					break;
				}
				++rawStringAt;
			}

			if (rawStringAt == rawStringLength)
			{
				return myAt;
			}
		}
		return kStringNPos;
	}

	const uint32 DynamicStringA::rfind(const char* const rawString, const uint32 offset) const noexcept
	{
		const uint32 rawStringLength = static_cast<uint32>(strlen(rawString));
		if (_length < rawStringLength)
		{
			return kStringNPos;
		}

		const char* const myRaw = c_str();
		const uint32 myAtStart = _length - rawStringLength;
		if (myAtStart < offset)
		{
			return kStringNPos;
		}
		for (uint32 myAt = myAtStart - offset; myAt != kUint32Max; --myAt)
		{
			uint32 rawStringAt = 0;
			while (rawStringAt < rawStringLength)
			{
				if (myRaw[myAt + rawStringAt] != rawString[rawStringAt])
				{
					break;
				}
				++rawStringAt;
			}

			if (rawStringAt == rawStringLength)
			{
				return myAt;
			}
		}
		return kStringNPos;
	}

	const bool DynamicStringA::compare(const char* const rawString) const noexcept
	{
		const uint32 rawStringLength = static_cast<uint32>(strlen(rawString));
		if (_length != rawStringLength)
		{
			return false;
		}

		const char* const myRaw = c_str();
		for (uint32 myAt = 0; myAt < _length; ++myAt)
		{
			if (myRaw[myAt] != rawString[myAt])
			{
				return false;
			}
		}
		return true;
	}

	const bool DynamicStringA::compare(const DynamicStringA& rhs) const noexcept
	{
		if (_length != rhs._length)
		{
			return false;
		}

		const char* const myRaw = c_str();
		const char* const rhsRaw = rhs.c_str();
		for (uint32 myAt = 0; myAt < _length; ++myAt)
		{
			if (myRaw[myAt] != rhsRaw[myAt])
			{
				return false;
			}
		}
		return true;
	}

	const uint64 DynamicStringA::hash() const noexcept
	{
		if (_cachedHash == 0)
		{
			_cachedHash = fs::StringUtil::hashRawString64(c_str());
		}
		return _cachedHash;
	}

	void DynamicStringA::setMemoryInternal(const char* const rawString, const uint32 rawStringLength, const uint32 offset, const bool isOneChar)
	{
		if (rawString == nullptr || rawStringLength == 0)
		{
			_memoryAccessor.setMemory<uint8>(0);
		}
		else
		{
			_memoryAccessor.setMemory(rawString, offset);
			if (isOneChar == false)
			{
				_memoryAccessor.setMemory<uint8>(0, rawStringLength);
			}
		}

		cacheRawMemoryInternal();

		_cachedHash = 0;
	}

	void DynamicStringA::cacheRawMemoryInternal()
	{
		const uint32 byteCapacity = _memoryAccessor.getByteCapacity();
		if (_cachedByteCapacity != byteCapacity)
		{
			_cachedRawMemoryPtr = reinterpret_cast<const char*>(_memoryAccessor.getMemory());
			_cachedByteCapacity = byteCapacity;
		}
	}
}
