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
		Resize(size, ch);
	}

	template<typename T>
	inline String<T>::String(const T* const rhs)
		: String()
	{
		AssignInternalXXX(rhs);
	}

	template<typename T>
	inline String<T>::String(const String& rhs)
		: String()
	{
		AssignInternalXXX(rhs.CString());
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
		Release();
	}

	template <typename T>
	MINT_INLINE String<T>& String<T>::operator=(const T* const rhs) noexcept
	{
		return Assign(rhs);
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator=(const String& rhs) noexcept
	{
		if (this != &rhs)
		{
			AssignInternalXXX(rhs.CString());
		}
		return *this;
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator=(const StringReference<T>& rhs) noexcept
	{
		if (this != &rhs)
		{
			AssignInternalXXX(rhs.CString());
		}
		return *this;
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator=(String&& rhs) noexcept
	{
		if (this != &rhs)
		{
			Release();

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
		const uint32 rhsLength = StringUtil::Length(rhs);
		String<T> newString;
		newString.Reserve(Size() + rhsLength);
		newString += *this;
		newString += rhs;
		return newString;
	}

	template<typename T>
	MINT_INLINE String<T> String<T>::operator+(const String& rhs) const noexcept
	{
		String<T> newString;
		newString.Reserve(Size() + rhs.Size());
		newString += *this;
		newString += rhs;
		return newString;
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator+=(const T rhs) noexcept
	{
		return static_cast<String<T>&>(Append(rhs));
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator+=(const T* const rhs) noexcept
	{
		return Append(rhs);
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::operator+=(const String& rhs) noexcept
	{
		return Append(rhs.CString());
	}

	template<typename T>
	MINT_INLINE bool String<T>::operator==(const T* const rhs) const noexcept
	{
		return Equals(rhs);
	}

	template<typename T>
	MINT_INLINE bool String<T>::operator==(const String& rhs) const noexcept
	{
		return Equals(rhs);
	}

	template<typename T>
	MINT_INLINE bool String<T>::operator!=(const T* const rhs) const noexcept
	{
		return !Equals(rhs);
	}

	template<typename T>
	MINT_INLINE bool String<T>::operator!=(const String& rhs) const noexcept
	{
		return !Equals(rhs);
	}

	template<typename T>
	inline MutableString<T>& String<T>::Assign(const StringReference<T>& rhs)
	{
		return AssignInternalXXX(rhs.CString());
	}

	template<typename T>
	inline String<T>& String<T>::Assign(const T* const rawString) noexcept
	{
		return AssignInternalXXX(rawString);
	}

	template<typename T>
	inline String<T>& String<T>::AssignInternalXXX(const T* const rawString) noexcept
	{
		const uint32 length = StringUtil::Length(rawString);
		if (length <= Short::kShortStringCapacity)
		{
			_short._size = length;
			__CopyString(_short._shortString, rawString, length);
			return *this;
		}
		return AssignInternalLongXXX(rawString, length);
	}

	template<typename T>
	inline String<T>& String<T>::AssignInternalLongXXX(const T* const rawString, const uint32 length) noexcept
	{
		MINT_ASSERT(StringUtil::Length(rawString) == length, "It must be guaranteed by the caller.");

		if (_long._capacity == 0)
		{
			_long._rawPointer = MemoryRaw::AllocateMemory<T>(length + 1);
		}
		else if (length > _long._capacity)
		{
			Release();
			_long._rawPointer = MemoryRaw::AllocateMemory<T>(length + 1);
		}

		_long._capacity = _long._size = length;
		__CopyString(_long._rawPointer, rawString, _long._size);
		return *this;
	}

	template<typename T>
	inline MutableString<T>& String<T>::Append(const StringReference<T>& rhs)
	{
		return Append(rhs.CString());
	}

	template<typename T>
	MINT_INLINE MutableString<T>& String<T>::Append(const T rhs)
	{
		T rawString[2] = { rhs, 0 };
		return Append(rawString);
	}

	template<typename T>
	inline String<T>& String<T>::Append(const T* const rawString) noexcept
	{
		if (IsShortString())
		{
			return AppendInternalSmallXXX(rawString);
		}
		return AppendInternalLongXXX(rawString);
	}

	template<typename T>
	MINT_INLINE String<T>& String<T>::Append(const String& rhs) noexcept
	{
		return Append(rhs.CString());
	}

	template<typename T>
	inline String<T>& String<T>::AppendInternalSmallXXX(const T* const rhs) noexcept
	{
		MINT_ASSERT(IsShortString() == true, "It must be guaranteed by the caller.");

		const uint32 capacity = Capacity();
		const uint32 rhsLength = StringUtil::Length(rhs);
		const uint64 newLength = static_cast<uint64>(_short._size) + rhsLength;
		if (newLength <= Short::kShortStringCapacity)
		{
			__CopyString(&_short._shortString[_short._size], rhs, rhsLength);
			_short._size = static_cast<T>(newLength);
			return *this;
		}

		Reserve(Max((capacity + 1) * 2 - 1, static_cast<uint32>(newLength)));
		return AppendInternalLongXXX(rhs);
	}

	template<typename T>
	inline String<T>& String<T>::AppendInternalLongXXX(const T* const rhs) noexcept
	{
		MINT_ASSERT(IsShortString() == false, "It must be guaranteed by the caller.");

		const uint32 capacity = Capacity();
		const uint32 rhsLength = StringUtil::Length(rhs);
		const uint64 newLength = _long._size + rhsLength;
		if (capacity < newLength)
		{
			Reserve(Max((capacity + 1) * 2 - 1, static_cast<uint32>(newLength)));
		}

		__CopyString(&_long._rawPointer[_long._size], rhs, rhsLength);
		_long._size = newLength;
		return *this;
	}

	template<typename T>
	inline void String<T>::Reserve(const uint32 newCapacity) noexcept
	{
		if (newCapacity <= Capacity())
		{
			return;
		}

		const uint32 oldSize = Size();
		if (oldSize == 0)
		{
			_long._rawPointer = MemoryRaw::AllocateMemory<T>(newCapacity + 1);
			_long._rawPointer[0] = 0;
			_long._capacity = newCapacity;
			_long._size = 0;
			return;
		}

		T* temp = MemoryRaw::AllocateMemory<T>(oldSize + 1);
		__CopyString(temp, (IsShortString()) ? _short._shortString : _long._rawPointer, oldSize);

		Release();

		_long._rawPointer = MemoryRaw::AllocateMemory<T>(newCapacity + 1);
		__CopyString(_long._rawPointer, temp, oldSize);
		_long._capacity = newCapacity;
		_long._size = oldSize;

		MemoryRaw::DeallocateMemory(temp);
	}

	template<typename T>
	inline void String<T>::Resize(const uint32 newSize, const T fillCharacter) noexcept
	{
		if (Capacity() < newSize)
		{
			Reserve(newSize);
		}

		T* const dataPointer = Data();
		const bool isGrowing = (Size() < newSize);
		if (isGrowing == true)
		{
			const uint32 oldSize = Size();
			for (uint32 iter = oldSize; iter < newSize; ++iter)
			{
				dataPointer[iter] = fillCharacter;
			}
		}
		dataPointer[newSize] = 0;

		_SetSize(newSize);
	}

	template<typename T>
	inline void String<T>::_SetSize(const uint32 newSize) noexcept
	{
		if (IsShortString() == true)
		{
			_short._size = newSize;
		}
		else
		{
			_long._size = newSize;
		}
	}

	template<typename T>
	MINT_INLINE void String<T>::Clear()
	{
		_SetSize(0);

		if (IsShortString())
		{
			_short._shortString[0] = 0;
		}
		else
		{
			::memset(_long._rawPointer, 0, kTypeSize);
		}
	}

	template<typename T>
	MINT_INLINE const T* String<T>::CString() const
	{
		return (IsShortString() == true) ? _short._shortString : _long._rawPointer;
	}

	template<typename T>
	MINT_INLINE T String<T>::Back() const
	{
		MINT_ASSERT(this->IsEmpty() == false, "!!!");
		return CString()[Size() - 1];
	}

	template<typename T>
	MINT_INLINE T* String<T>::Data()
	{
		return (IsShortString() == true) ? _short._shortString : _long._rawPointer;
	}

	template<typename T>
	MINT_INLINE void String<T>::__CopyString(T* const destination, const T* const source, const uint64 byteCount) noexcept
	{
		if (byteCount == 0)
		{
			return;
		}

		::memmove(destination, source, byteCount * kTypeSize);
		destination[byteCount] = 0;
	}

	template<typename T>
	inline uint32 String<T>::Find(const T* const target, const uint32 offset) const noexcept
	{
		const uint32 sourceLength = Size();
		const uint32 tarGetLength = StringUtil::Length(target);
		if (sourceLength < offset + tarGetLength)
		{
			return kStringNPos;
		}

		const T* const dataPointer = CString();
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
				if (targetIter == tarGetLength)
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
	inline uint32 String<T>::Find(const T target, const uint32 offset) const noexcept
	{
		T targets[2] = { target, 0 };
		return Find(targets, offset);
	}

	template<typename T>
	inline String<T> String<T>::Substring(const uint32 offset, const uint32 count) const noexcept
	{
		String result;
		const T* const dataPointer = CString();
		const uint32 stringSize = Size();
		if (offset < stringSize && count > 0)
		{
			const uint32 substringSize = Min(count, stringSize - offset);
			result.Resize(substringSize);
			for (uint32 iter = 0; iter < substringSize; ++iter)
			{
				result[iter] = dataPointer[offset + iter];
			}
		}
		return result;
	}

	template<typename T>
	inline void String<T>::Insert(const uint32 at, const T ch) noexcept
	{
		T str[2] = { ch, 0 };
		insert(at, str);
	}

	template<typename T>
	inline void String<T>::Insert(const uint32 at, const T* const rhs) noexcept
	{
		const uint32 rhsLength = StringUtil::Length(rhs);
		const uint32 oldLength = Size();
		const uint32 newLength = oldLength + rhsLength;
		if (Capacity() <= newLength)
		{
			Reserve(newLength + 1);
		}

		if (oldLength <= at)
		{
			Append(rhs);
		}
		else
		{
			const uint32 movedSize = oldLength - at;
			__CopyString(Data() + at + rhsLength, Data() + at, movedSize);
			for (uint32 iter = 0; iter < rhsLength; ++iter)
			{
				Data()[at + iter] = rhs[iter];
			}

			_SetSize(newLength);
		}
	}

	template<typename T>
	inline void String<T>::Insert(const uint32 at, const String& rhs) noexcept
	{
		insert(at, rhs.CString());
	}

	template<typename T>
	inline void String<T>::Erase(const uint32 at) noexcept
	{
		Erase(at, 1);
	}

	template<typename T>
	inline void String<T>::Erase(const uint32 at, const uint32 byteCount) noexcept
	{
		if (this->IsEmpty() == true)
		{
			return;
		}

		const uint32 lhsSize = Size();
		if (lhsSize - 1 <= at)
		{
			Data()[lhsSize - 1] = 0;
			_SetSize(lhsSize - 1);
		}
		else
		{
			uint32 newSize = 0;
			if (at + byteCount < lhsSize)
			{
				newSize = lhsSize - byteCount;
				__CopyString(Data() + at, Data() + at + byteCount, lhsSize - (at + byteCount));
			}
			else
			{
				newSize = at;
			}
			Data()[newSize] = 0;
			_SetSize(newSize);
		}
	}

	template<typename T>
	inline void String<T>::PopBack()
	{
		if (this->IsEmpty())
		{
			return;
		}

		const uint32 size_ = Size();
		Data()[size_ - 1] = 0;
		_SetSize(size_ - 1);
	}

	template<typename T>
	inline bool String<T>::Equals(const T* const rhs) const noexcept
	{
		const uint32 lhsLength = Size();
		const uint32 rhsLength = StringUtil::Length(rhs);
		if (lhsLength != rhsLength)
		{
			return false;
		}

		const T* const lhs = CString();
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
	MINT_INLINE bool String<T>::Equals(const String& rhs) const noexcept
	{
		return Equals(rhs.CString());
	}

	template<typename T>
	MINT_INLINE uint64 String<T>::ComputeHash() const noexcept
	{
		return mint::ComputeHash(CString());
	}

	template<typename T>
	inline void String<T>::Release() noexcept
	{
		if (IsShortString() == false)
		{
			MemoryRaw::DeallocateMemory<T>(_long._rawPointer);
		}

		_long._size = 0;
		_long._capacity = 0;
	}

	template<typename T>
	MINT_INLINE void String<T>::ToLongString() noexcept
	{
		if (IsShortString() == false || this->IsEmpty() == true)
		{
			return;
		}

		Short tempShort{ _short };
		AssignInternalLongXXX(tempShort._shortString, StringUtil::Length(tempShort._shortString));
	}


	template <typename T>
	MINT_INLINE String<T> operator+(const T* lhs, String<T>&& rhs)
	{
		String<T> third;
		third.Reserve(StringUtil::Length(lhs) + rhs.Length());
		third = lhs;
		third += rhs;
		return third;
	}

	template <typename T>
	MINT_INLINE String<T> operator+(const T* lhs, String<T>& rhs)
	{
		String<T> third;
		third.Reserve(StringUtil::Length(lhs) + rhs.Length());
		third = lhs;
		third += rhs;
		return third;
	}
}


#endif // !_MINT_CONTAINER_STRING_HPP_
