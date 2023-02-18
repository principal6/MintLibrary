#pragma once


#ifndef _MINT_CONTAINER_STRING_HPP_
#define _MINT_CONTAINER_STRING_HPP_


#include <MintContainer/Include/String.h>

#include <MintContainer/Include/StringUtil.h>
#include <MintContainer/Include/MemoryRaw.hpp>
#include <MintContainer/Include/Hash.hpp>


namespace mint
{
	template <typename T>
	inline String<T>::String()
		: MutableString<T>()
	{
		__noop;
	}

	template<typename T>
	inline String<T>::String(const uint32 size, const T ch)
		: String()
	{
		resize(size, ch);
	}

	template<typename T>
	inline String<T>::String(const T* const rhs)
		: String()
	{
		assignInternalXXX(rhs);
	}

	template<typename T>
	inline String<T>::String(const String& rhs)
		: String()
	{
		assignInternalXXX(rhs.c_str());
	}

	template<typename T>
	inline String<T>::String(String&& rhs) noexcept
		: String()
	{
		_long = rhs._long;

		rhs._long._capacity = 0;
		rhs._long._size = 0;
		rhs._long._rawPointer = nullptr;
	}

	template <typename T>
	inline String<T>::~String()
	{
		release();
	}

	template <typename T>
	MINT_INLINE String<T>& String<T>::operator=(const T* const rhs) noexcept
	{
		return assign(rhs);
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator=(const String& rhs) noexcept
	{
		if (this != &rhs)
		{
			assignInternalXXX(rhs.c_str());
		}
		return *this;
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator=(String&& rhs) noexcept
	{
		if (this != &rhs)
		{
			_long = rhs._long;

			rhs._long._capacity = 0;
			rhs._long._size = 0;
			rhs._long._rawPointer = nullptr;
		}
		return *this;
	}

	template<typename T>
	MINT_INLINE String<T> String<T>::operator+(const T* const rhs) const noexcept
	{
		const uint32 rhsLength = StringUtil::length(rhs);
		String<T> newString;
		newString.reserve(size() + rhsLength);
		newString += *this;
		newString += rhs;
		return newString;
	}

	template<typename T>
	MINT_INLINE String<T> String<T>::operator+(const String& rhs) const noexcept
	{
		String<T> newString;
		newString.reserve(size() + rhs.size());
		newString += *this;
		newString += rhs;
		return newString;
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator+=(const T* const rhs) noexcept
	{
		return append(rhs);
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator+=(const String& rhs) noexcept
	{
		return append(rhs.c_str());
	}

	template<typename T>
	MINT_INLINE bool String<T>::operator==(const T* const rhs) const noexcept
	{
		return compare(rhs);
	}

	template<typename T>
	MINT_INLINE bool String<T>::operator==(const String& rhs) const noexcept
	{
		return compare(rhs);
	}

	template<typename T>
	MINT_INLINE bool String<T>::operator!=(const T* const rhs) const noexcept
	{
		return !compare(rhs);
	}

	template<typename T>
	MINT_INLINE bool String<T>::operator!=(const String& rhs) const noexcept
	{
		return !compare(rhs);
	}

	template<typename T>
	inline MutableString<T>& String<T>::assign(const StringReference<T>& rhs)
	{
		release();

		return assignInternalXXX(rhs.c_str());
	}

	template<typename T>
	inline String<T>& String<T>::assign(const T* const rawString) noexcept
	{
		release();

		return assignInternalXXX(rawString);
	}

	template<typename T>
	inline String<T>& String<T>::assignInternalXXX(const T* const rawString) noexcept
	{
		const uint32 length = StringUtil::length(rawString);
		if (length < Short::kSmallStringCapacity)
		{
			_short._size = length;
			__copyString(_short._smallString, rawString, length);
			return *this;
		}
		return assignInternalLongXXX(rawString);
	}

	template<typename T>
	inline String<T>& String<T>::assignInternalLongXXX(const T* const rawString) noexcept
	{
		const uint32 length = StringUtil::length(rawString);
		_long._size = length;
		_long._capacity = _long._size + 1;

		_long._rawPointer = MemoryRaw::allocateMemory<T>(capacity());
		__copyString(_long._rawPointer, rawString, _long._size);
		return *this;
	}

	template<typename T>
	inline MutableString<T>& String<T>::append(const StringReference<T>& rhs)
	{
		return append(rhs.c_str());
	}

	template<typename T>
	inline String<T>& String<T>::append(const T* const rawString) noexcept
	{
		if (isNotAllocated())
		{
			return assign(rawString);
		}

		if (isSmallString())
		{
			return appendInternalSmallXXX(rawString);
		}
		return appendInternalLongXXX(rawString);
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::append(const T ch) noexcept
	{
		T rawString[2] = { ch, 0 };
		return append(rawString);
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::append(const String& rhs) noexcept
	{
		return append(rhs.c_str());
	}

	template<typename T>
	inline String<T>& String<T>::appendInternalSmallXXX(const T* const rhs) noexcept
	{
		const uint32 rhsLength = StringUtil::length(rhs);
		const uint64 newLength = static_cast<uint64>(_short._size) + rhsLength;
		if (newLength < Short::kSmallStringCapacity)
		{
			__copyString(&_short._smallString[_short._size], rhs, rhsLength);
			_short._size = static_cast<T>(newLength);
			return *this;
		}

		reserve(Max(capacity() * 2, static_cast<uint32>(newLength + 1)));
		return appendInternalLongXXX(rhs);
	}

	template<typename T>
	inline String<T>& String<T>::appendInternalLongXXX(const T* const rhs) noexcept
	{
		const uint32 rhsLength = StringUtil::length(rhs);
		const uint64 newLength = _long._size + rhsLength;
		if (_long._capacity <= newLength)
		{
			reserve(Max(static_cast<uint32>(_long._capacity * 2), static_cast<uint32>(newLength + 1)));
		}

		__copyString(&_long._rawPointer[_long._size], rhs, rhsLength);
		_long._size = newLength;
		return *this;
	}

	template<typename T>
	inline void String<T>::reserve(const uint32 newCapacity) noexcept
	{
		if (newCapacity <= capacity() || newCapacity <= Short::kSmallStringCapacity)
		{
			return;
		}

		//if (Long::kStringMaxCapacity <= newCapacity)
		//{
		//    MINT_NEVER;
		//}

		const uint32 oldSize = size();
		T* temp = MemoryRaw::allocateMemory<T>(oldSize + 1);
		__copyString(temp, (isSmallString()) ? _short._smallString : _long._rawPointer, oldSize);

		release();

		_long._rawPointer = MemoryRaw::allocateMemory<T>(newCapacity);
		__copyString(_long._rawPointer, temp, oldSize);
		_long._capacity = newCapacity;
		_long._size = oldSize;

		MemoryRaw::deallocateMemory(temp);
	}

	template<typename T>
	inline void String<T>::resize(const uint32 newSize, const T fillCharacter) noexcept
	{
		if (capacity() <= newSize)
		{
			reserve(newSize + 1);
		}

		T* const dataPointer = data();
		const bool isGrowing = (size() < newSize);
		if (isGrowing == true)
		{
			const uint32 oldSize = size();
			for (uint32 iter = oldSize; iter < newSize; ++iter)
			{
				dataPointer[iter] = fillCharacter;
			}
		}
		dataPointer[newSize] = 0;

		_setSize(newSize);
	}

	template<typename T>
	inline void String<T>::_setSize(const uint32 newSize) noexcept
	{
		if (isSmallString() == true)
		{
			_short._size = newSize;
		}
		else
		{
			_long._size = newSize;
		}
	}

	template<typename T>
	MINT_INLINE void String<T>::clear()
	{
		_setSize(0);

		if (isSmallString())
		{
			_short._smallString[0] = 0;
		}
		else
		{
			::memset(_long._rawPointer, 0, kTypeSize);
		}
	}

	template<typename T>
	MINT_INLINE const T* String<T>::c_str() const
	{
		return (isSmallString() == true) ? _short._smallString : _long._rawPointer;
	}

	template<typename T>
	MINT_INLINE T String<T>::back() const
	{
		MINT_ASSERT(this->empty() == false, "!!!");
		return c_str()[size() - 1];
	}

	template<typename T>
	MINT_INLINE T* String<T>::data()
	{
		return (isSmallString() == true) ? _short._smallString : _long._rawPointer;
	}

	template<typename T>
	MINT_INLINE void String<T>::__copyString(T* const destination, const T* const source, const uint64 byteCount) noexcept
	{
		if (byteCount == 0)
		{
			return;
		}

		::memmove(destination, source, byteCount * kTypeSize);
		destination[byteCount] = 0;
	}

	template<typename T>
	inline uint32 String<T>::find(const T* const target, const uint32 offset) const noexcept
	{
		const uint32 sourceLength = size();
		const uint32 targetLength = StringUtil::length(target);
		if (sourceLength < offset + targetLength)
		{
			return kStringNPos;
		}

		const T* const dataPointer = c_str();
		uint32 result = kStringNPos;
		uint32 targetIter = 0;
		for (uint32 sourceIter = offset; sourceIter < sourceLength; ++sourceIter)
		{
			if (dataPointer[sourceIter] == target[targetIter])
			{
				if (targetIter == 0)
				{
					result = sourceIter;
				}

				++targetIter;
				if (targetIter == targetLength)
				{
					break;
				}
			}
			else
			{
				targetIter = 0;
				result = kStringNPos;
			}
		}
		return result;
	}

	template<typename T>
	inline uint32 String<T>::find(const T target, const uint32 offset) const noexcept
	{
		T targets[2] = { target, 0 };
		return find(targets, offset);
	}

	template<typename T>
	inline String<T> String<T>::substr(const uint32 offset, const uint32 count) const noexcept
	{
		String result;
		const T* const dataPointer = c_str();
		const uint32 stringSize = size();
		if (offset < stringSize && count > 0)
		{
			const uint32 substringSize = Min(count, stringSize - offset);
			result.resize(substringSize);
			for (uint32 iter = 0; iter < substringSize; ++iter)
			{
				result[iter] = dataPointer[offset + iter];
			}
		}
		return result;
	}

	template<typename T>
	inline void String<T>::insert(const uint32 at, const T ch) noexcept
	{
		T str[2] = { ch, 0 };
		insert(at, str);
	}

	template<typename T>
	inline void String<T>::insert(const uint32 at, const T* const rhs) noexcept
	{
		const uint32 rhsLength = StringUtil::length(rhs);
		const uint32 oldLength = size();
		const uint32 newLength = oldLength + rhsLength;
		if (capacity() <= newLength)
		{
			reserve(newLength + 1);
		}

		if (oldLength <= at)
		{
			append(rhs);
		}
		else
		{
			const uint32 movedSize = oldLength - at;
			__copyString(data() + at + rhsLength, data() + at, movedSize);
			for (uint32 iter = 0; iter < rhsLength; ++iter)
			{
				data()[at + iter] = rhs[iter];
			}

			_setSize(newLength);
		}
	}

	template<typename T>
	inline void String<T>::insert(const uint32 at, const String& rhs) noexcept
	{
		insert(at, rhs.c_str());
	}

	template<typename T>
	inline void String<T>::erase(const uint32 at) noexcept
	{
		erase(at, 1);
	}

	template<typename T>
	inline void String<T>::erase(const uint32 at, const uint32 byteCount) noexcept
	{
		if (this->empty() == true)
		{
			return;
		}

		const uint32 lhsSize = size();
		if (lhsSize - 1 <= at)
		{
			data()[lhsSize - 1] = 0;
			_setSize(lhsSize - 1);
		}
		else
		{
			uint32 newSize = 0;
			if (at + byteCount < lhsSize)
			{
				newSize = lhsSize - byteCount;
				__copyString(data() + at, data() + at + byteCount, lhsSize - (at + byteCount));
			}
			else
			{
				newSize = at;
			}
			data()[newSize] = 0;
			_setSize(newSize);
		}
	}

	template<typename T>
	inline void String<T>::pop_back()
	{
		if (this->empty())
		{
			return;
		}

		const uint32 size_ = size();
		data()[size_ - 1] = 0;
		_setSize(size_ - 1);
	}

	template<typename T>
	inline bool String<T>::compare(const T* const rhs) const noexcept
	{
		const uint32 lhsLength = size();
		const uint32 rhsLength = StringUtil::length(rhs);
		if (lhsLength != rhsLength)
		{
			return false;
		}

		const T* const lhs = c_str();
		for (uint32 iter = 0; iter < lhsLength; ++iter)
		{
			if (lhs[iter] != rhs[iter])
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	MINT_INLINE bool String<T>::compare(const String& rhs) const noexcept
	{
		return compare(rhs.c_str());
	}

	template<typename T>
	MINT_INLINE uint64 String<T>::computeHash() const noexcept
	{
		return mint::computeHash(c_str());
	}

	template<typename T>
	inline void String<T>::release() noexcept
	{
		if (isSmallString() == false)
		{
			MemoryRaw::deallocateMemory<T>(_long._rawPointer);
		}

		_long._size = 0;
		_long._capacity = 0;
	}

	template<typename T>
	inline void String<T>::toLongString() noexcept
	{
		if (isSmallString() == false || this->empty() == true)
		{
			return;
		}

		Short tempShort{ _short };
		assignInternalLongXXX(tempShort._smallString);
	}
}


#endif // !_MINT_CONTAINER_STRING_HPP_
