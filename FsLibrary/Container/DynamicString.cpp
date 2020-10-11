#include <stdafx.h>
#include <Container/DynamicString.h>

#include <Container/StringUtil.h>
#include <Memory/MemoryAllocator.hpp>


namespace fs
{
	MemoryAllocator DynamicStringA::_memoryAllocator;
	DynamicStringA::DynamicStringA()
		: _length{ 0 }
		, _cachedHash{ 0 }
	{
		__noop;
	}

	DynamicStringA::DynamicStringA(const uint32 capacity)
		: DynamicStringA()
	{
		_memoryAccessor = _memoryAllocator.allocate(fs::max(capacity, kMinCapacity));
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
		setMemoryInternal(rawString);
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
		setMemoryInternal(rawString);
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
		setMemoryInternal(rhs.c_str());
	}

	DynamicStringA::DynamicStringA(DynamicStringA&& rhs) noexcept
		: DynamicStringA()
	{
		_length = rhs._length;
		_memoryAccessor = rhs._memoryAccessor;

		setMemoryInternal(rhs.c_str());

		rhs._length = 0;
		rhs._memoryAccessor = MemoryAccessor();
	}

	DynamicStringA::~DynamicStringA()
	{
		_memoryAllocator.deallocate(_memoryAccessor);
	}

	DynamicStringA& DynamicStringA::operator=(const char* const rawString)
	{
		assign(rawString);
		return *this;
	}

	DynamicStringA& DynamicStringA::operator=(const DynamicStringA& rhs)
	{
		if (this != &rhs)
		{
			assign(rhs);
		}
		return *this;
	}

	DynamicStringA& DynamicStringA::operator=(DynamicStringA&& rhs) noexcept
	{
		if (this != &rhs)
		{
			assign(rhs);

			rhs._length = 0;
			rhs._memoryAccessor = MemoryAccessor();
		}
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

	const DynamicStringA DynamicStringA::operator+(const char* const rawString) const noexcept
	{
		DynamicStringA result(*this);
		if (fs::StringUtil::isNullOrEmpty(rawString) == false)
		{
			result.append(rawString);
		}
		return result;
	}

	const DynamicStringA DynamicStringA::operator+(const DynamicStringA& rhs) const noexcept
	{
		DynamicStringA result(*this);
		result.append(rhs);
		return result;
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
		setMemoryInternal(nullptr);
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
				setMemoryInternal(rawString);
			}
			else
			{
				_memoryAllocator.reallocate(_memoryAccessor, newCapacity, false);

				setMemoryInternal(rawString);
			}
			return;
		}

		_memoryAccessor = _memoryAllocator.allocate(fs::max(newCapacity, kMinCapacity));
		setMemoryInternal(rawString);
	}

	void DynamicStringA::assign(const DynamicStringA& rhs)
	{
		if (this != &rhs)
		{
			_length = rhs._length;

			const uint32 newCapacity = _length + 1;
			_memoryAccessor = _memoryAllocator.allocate(fs::max(newCapacity, kMinCapacity));
			
			setMemoryInternal(rhs.c_str());
		}
	}

	void DynamicStringA::append(const char* const rawString)
	{
		if (fs::StringUtil::isNullOrEmpty(rawString) == true)
		{
			return;
		}

		const uint32 deltaLength = fs::StringUtil::strlen(rawString);
		const uint32 capacityCandidate = _length + deltaLength + 1;
		if (_memoryAccessor.getByteCapacity() <= capacityCandidate)
		{
			_memoryAllocator.reallocate(_memoryAccessor, fs::max(_memoryAccessor.getByteCapacity() * 2, capacityCandidate), true);
		}
		
		const uint32 offset = _length;
		_length += deltaLength;
		setMemoryInternal(rawString, offset);
	}

	void DynamicStringA::append(const DynamicStringA& rhs)
	{
		const uint32 capacityCandidate = _length + rhs._length + 1;
		if (_memoryAccessor.getByteCapacity() <= capacityCandidate)
		{
			_memoryAllocator.reallocate(_memoryAccessor, fs::max(_memoryAccessor.getByteCapacity() * 2, capacityCandidate), true);
		}

		const uint32 offset = _length;
		_length += rhs._length;
		setMemoryInternal(rhs.c_str(), offset);
	}

	DynamicStringA DynamicStringA::substr(const uint32 offset, const uint32 count) const noexcept
	{
		const char* const myRaw = c_str();
		const uint32 subStringLength = fs::min(count, _length - offset);
		if (_length <= offset)
		{
			return DynamicStringA();
		}
		return DynamicStringA(myRaw + offset, subStringLength);
	}

	void DynamicStringA::setChar(const uint32 at, const char ch)
	{
		if (at < _length)
		{
			const char str[2]{ ch, 0 };
			setMemoryInternal(str, at);
		}
	}

	void DynamicStringA::reserve(const uint32 newCapacity)
	{
		if (_memoryAccessor.getByteCapacity() < newCapacity)
		{
			_memoryAllocator.reallocate(_memoryAccessor, fs::max(newCapacity, kMinCapacity), true);
		}
	}

	void DynamicStringA::resize(const uint32 newSize)
	{
		if (_memoryAccessor.getByteCapacity() <= newSize)
		{
			_memoryAllocator.reallocate(_memoryAccessor, fs::max(newSize + 1, kMinCapacity), true);
		}
		setChar(newSize, 0);
	}

	void DynamicStringA::push_back(const char ch)
	{
		if (_memoryAccessor.getByteCapacity() == _length + 1)
		{
			reserve(_memoryAccessor.getByteCapacity() * 2);
		}
		++_length;
		setChar(_length - 1, ch);
	}

	void DynamicStringA::pop_back()
	{
		if (0 < _length)
		{
			setChar(_length - 1, 0);
			--_length;
		}
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
		return reinterpret_cast<const char*>(_memoryAccessor.getMemory());
	}

	const char DynamicStringA::getChar(const uint32 at) const noexcept
	{
		if (_length <= at)
		{
			return 0;
		}
		return c_str()[at];
	}

	const char DynamicStringA::front() const noexcept
	{
		return getChar(0);
	}

	const char DynamicStringA::back() const noexcept
	{
		return getChar(_length - 1);
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

	void DynamicStringA::setMemoryInternal(const char* const rawString, const uint32 offset)
	{
		if (rawString == nullptr)
		{
			_memoryAccessor.setMemory<uint8>(0);
		}
		else
		{
			_memoryAccessor.setMemory(rawString, offset);
			_memoryAccessor.setMemory<uint8>(0, _length);
		}

		_cachedHash = 0;
	}
}
