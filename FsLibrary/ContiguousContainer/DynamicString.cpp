#include "stdafx.h"
#include <FsLibrary/ContiguousContainer/DynamicString.h>
#include <FsLibrary/ContiguousContainer/DynamicString.hpp>

#include <FsLibrary/Container/StringUtil.h>

#include <FsLibrary/Memory/Allocator.hpp>


namespace fs
{
	DynamicStringA::DynamicStringA()
		: _memoryAllocator{ fs::Memory::Allocator<char>::getInstance() }
		, _memoryAccessor{ &_memoryAllocator }
		, _length{ 0 }
		, _cachedHash{ 0 }
	{
		__noop;
	}

	DynamicStringA::DynamicStringA(const uint32 capacity)
		: DynamicStringA()
	{
		_memoryAccessor = _memoryAllocator.allocateArray(fs::max(capacity, kMinCapacity));
	}

	DynamicStringA::DynamicStringA(const char* const rawString)
		: DynamicStringA()
	{
		assign(rawString);
	}

	DynamicStringA::DynamicStringA(const char* const rawString, const uint32 rawStringLength)
		: DynamicStringA()
	{
		assign(rawString, rawStringLength);
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
		_memoryAccessor = _memoryAllocator.allocateArray(fs::max(newCapacity, kMinCapacity));

		setMemoryInternal(rhs.c_str());
	}

	DynamicStringA::DynamicStringA(DynamicStringA&& rhs) noexcept
		: DynamicStringA()
	{
		const char* const rhsRawString = rhs.c_str();

		_length = rhs._length;
		_memoryAccessor = std::move(rhs._memoryAccessor);

		setMemoryInternal(rhsRawString);

		rhs._length = 0;
	}

	DynamicStringA::~DynamicStringA()
	{
		__noop;
	}

	void DynamicStringA::clear()
	{
		_length = 0;
		setMemoryInternal(nullptr);
	}

	void DynamicStringA::assign(const char* const rawString, const uint32 rawStringLength)
	{
		if (fs::StringUtil::isNullOrEmpty(rawString) == true)
		{
			return;
		}

		_length = rawStringLength;
		const uint32 newCapacity = _length + 1;

		char* rawStringCopy{ nullptr };
		if (_memoryAllocator.isResident(rawString) == true)
		{
			rawStringCopy = FS_NEW_ARRAY(char, newCapacity);
			memcpy(rawStringCopy, rawString, newCapacity);
		}
		
		if (_memoryAccessor.isValid() == true)
		{
			if (_length < _memoryAccessor.getArraySize())
			{
				setMemoryInternal((rawStringCopy == nullptr) ? rawString : rawStringCopy);
			}
			else
			{
				_memoryAccessor = _memoryAllocator.reallocateArray(_memoryAccessor, newCapacity, false);

				setMemoryInternal((rawStringCopy == nullptr) ? rawString : rawStringCopy);
			}
			return;
		}

		_memoryAccessor = _memoryAllocator.allocateArray(fs::max(newCapacity, kMinCapacity));
		setMemoryInternal((rawStringCopy == nullptr) ? rawString : rawStringCopy);

		if (rawStringCopy != nullptr)
		{
			FS_DELETE_ARRAY(rawStringCopy);
		}
	}

	void DynamicStringA::assign(const DynamicStringA& rhs)
	{
		if (this != &rhs)
		{
			_length = rhs._length;

			const uint32 newCapacity = _length + 1;
			_memoryAccessor = _memoryAllocator.allocateArray(fs::max(newCapacity, kMinCapacity));
			
			setMemoryInternal(rhs.c_str());
		}
	}

	void DynamicStringA::assign(DynamicStringA&& rhs)
	{
		if (this != &rhs)
		{
			const char* const rhsRawString = rhs.c_str();

			_length = rhs._length;
			_memoryAccessor = std::move(rhs._memoryAccessor);

			setMemoryInternal(rhsRawString);
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
		if (_memoryAccessor.getArraySize() <= capacityCandidate)
		{
			_memoryAccessor = _memoryAllocator.reallocateArray(_memoryAccessor, fs::max(_memoryAccessor.getArraySize() * 2, capacityCandidate), true);
		}
		
		const uint32 offset = _length;
		_length += deltaLength;
		setMemoryInternal(rawString, offset);
	}

	void DynamicStringA::append(const DynamicStringA& rhs)
	{
		const uint32 capacityCandidate = _length + rhs._length + 1;
		if (_memoryAccessor.getArraySize() <= capacityCandidate)
		{
			_memoryAccessor = _memoryAllocator.reallocateArray(_memoryAccessor, fs::max(_memoryAccessor.getArraySize() * 2, capacityCandidate), true);
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
		if (_memoryAccessor.getArraySize() < newCapacity)
		{
			_memoryAccessor = _memoryAllocator.reallocateArray(_memoryAccessor, fs::max(newCapacity, kMinCapacity), true);
		}
	}

	void DynamicStringA::resize(const uint32 newSize)
	{
		if (_memoryAccessor.getArraySize() <= newSize)
		{
			_memoryAccessor = _memoryAllocator.reallocateArray(_memoryAccessor, fs::max(newSize + 1, kMinCapacity), true);
		}
		setChar(newSize, 0);
	}

	void DynamicStringA::push_back(const char ch)
	{
		if (_memoryAccessor.getArraySize() == _length + 1)
		{
			reserve(_memoryAccessor.getArraySize() * 2);
		}
		++_length;
		setChar(_length - 1, ch);
	}

	const uint32 DynamicStringA::find(const char* const rawString, const uint32 offset) const noexcept
	{
		const uint32 rawStringLength = fs::StringUtil::strlen(rawString);
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
		const uint32 rawStringLength = fs::StringUtil::strlen(rawString);
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
		const uint32 rawStringLength = fs::StringUtil::strlen(rawString);
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

	const bool DynamicStringA::compare(const char* const rawString, const StringRange& stringRange) const noexcept
	{
		if (stringRange.isLengthSet() == true)
		{
			return compareInternal(rawString, stringRange._offset, stringRange._length);
		}
		return compareInternal(rawString, stringRange._offset);
	}

	const bool DynamicStringA::compareInternal(const char* const rawString, const uint32 offset) const noexcept
	{
		if (_length <= offset)
		{
			return false;
		}

		const uint32 compareLength = _length - offset;
		const uint32 rawStringLength = fs::StringUtil::strlen(rawString);
		const char* const myRawString = c_str();
		for (uint32 iter = 0; iter < compareLength; ++iter)
		{
			if (myRawString[offset + iter] != rawString[iter])
			{
				return false;
			}
		}
		return true;
	}

	const bool DynamicStringA::compareInternal(const char* const rawString, const uint32 offset, const uint32 length) const noexcept
	{
		if (_length < offset + length)
		{
			return false;
		}

		const uint32 rawStringLength = fs::StringUtil::strlen(rawString);
		const char* const myRawString = c_str();
		for (uint32 iter = 0; iter < length; ++iter)
		{
			if (myRawString[offset + iter] != rawString[iter])
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

	const bool DynamicStringA::compare(const DynamicStringA& rhs, const StringRange& stringRange) const noexcept
	{
		if (stringRange.isLengthSet() == true)
		{
			return compareInternal(rhs, stringRange._offset, stringRange._length);
		}
		return compareInternal(rhs, stringRange._offset);
	}

	const bool DynamicStringA::compareInternal(const DynamicStringA& rhs, const uint32 offset) const noexcept
	{
		if (_length <= offset)
		{
			return false;
		}

		const uint32 compareLength = _length - offset;
		const char* const myRaw = c_str();
		const char* const rhsRaw = rhs.c_str();
		for (uint32 myAt = 0; myAt < compareLength; ++myAt)
		{
			if (myRaw[offset + myAt] != rhsRaw[myAt])
			{
				return false;
			}
		}
		return true;
	}

	const bool DynamicStringA::compareInternal(const DynamicStringA& rhs, const uint32 offset, const uint32 length) const noexcept
	{
		if (_length < offset + length)
		{
			return false;
		}

		const char* const myRaw = c_str();
		const char* const rhsRaw = rhs.c_str();
		for (uint32 myAt = 0; myAt < length; ++myAt)
		{
			if (myRaw[offset + myAt] != rhsRaw[myAt])
			{
				return false;
			}
		}
		return true;
	}
}
