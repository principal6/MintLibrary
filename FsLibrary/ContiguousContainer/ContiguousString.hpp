#pragma once


#include <stdafx.h>
#include <FsLibrary/ContiguousContainer/ContiguousString.h>

#include <FsLibrary/Container/StringUtil.h>


namespace fs
{
#pragma region Static functions
	template<class T>
	FS_INLINE const ContiguousStringA ContiguousStringA::from_value(const typename std::enable_if<std::is_literal_type<T>::value, T>::type value) noexcept
	{
		return ContiguousStringA(std::to_string(value).c_str());
	}

	template<>
	FS_INLINE const ContiguousStringA ContiguousStringA::from_value<bool>(const bool value) noexcept
	{
		return ContiguousStringA((true == value) ? "true" : "false");
	}

	FS_INLINE const bool ContiguousStringA::to_bool(const ContiguousStringA& dynamicString) noexcept
	{
		return dynamicString == "true";
	}

	FS_INLINE const int32 ContiguousStringA::to_int32(const ContiguousStringA& dynamicString) noexcept
	{
		return atoi(dynamicString.c_str());
	}

	FS_INLINE const uint32 ContiguousStringA::to_uint32(const ContiguousStringA& dynamicString) noexcept
	{
		return static_cast<uint32>(atoll(dynamicString.c_str()));
	}

	FS_INLINE const float ContiguousStringA::to_float(const ContiguousStringA& dynamicString) noexcept
	{
		return static_cast<float>(atof(dynamicString.c_str()));
	}

	FS_INLINE const double ContiguousStringA::to_double(const ContiguousStringA& dynamicString) noexcept
	{
		return atof(dynamicString.c_str());
	}
#pragma endregion


	FS_INLINE ContiguousStringA& ContiguousStringA::operator=(const char* const rawString)
	{
		assign(rawString);

		return *this;
	}

	FS_INLINE ContiguousStringA& ContiguousStringA::operator=(const ContiguousStringA& rhs)
	{
		if (this != &rhs)
		{
			assign(rhs);
		}
		return *this;
	}

	FS_INLINE ContiguousStringA& ContiguousStringA::operator=(ContiguousStringA&& rhs) noexcept
	{
		if (this != &rhs)
		{
			assign(std::forward<ContiguousStringA>(rhs));
		}
		return *this;
	}

	FS_INLINE ContiguousStringA& ContiguousStringA::operator+=(const char* const rawString) noexcept
	{
		append(rawString);
		return *this;
	}

	FS_INLINE ContiguousStringA& ContiguousStringA::operator+=(const ContiguousStringA& rhs) noexcept
	{
		append(rhs);
		return *this;
	}

	FS_INLINE const ContiguousStringA ContiguousStringA::operator+(const char* const rawString) const noexcept
	{
		ContiguousStringA result(*this);
		if (fs::StringUtil::isNullOrEmpty(rawString) == false)
		{
			result.append(rawString);
		}
		return result;
	}

	FS_INLINE const ContiguousStringA ContiguousStringA::operator+(const ContiguousStringA& rhs) const noexcept
	{
		ContiguousStringA result(*this);
		result.append(rhs);
		return result;
	}

	FS_INLINE const bool ContiguousStringA::operator==(const char* const rawString) const noexcept
	{
		return compare(rawString);
	}

	FS_INLINE const bool ContiguousStringA::operator==(const ContiguousStringA& rhs) const noexcept
	{
		return compare(rhs);
	}

	FS_INLINE const bool ContiguousStringA::operator!=(const char* const rawString) const noexcept
	{
		return !compare(rawString);
	}

	FS_INLINE const bool ContiguousStringA::operator!=(const ContiguousStringA& rhs) const noexcept
	{
		return !compare(rhs);
	}

	FS_INLINE void ContiguousStringA::assign(const char* const rawString)
	{
		assign(rawString, fs::StringUtil::strlen(rawString));
	}

	FS_INLINE ContiguousStringA ContiguousStringA::substr(const StringRange& stringRange) const noexcept
	{
		return substr(stringRange._offset, stringRange._length);
	}

	FS_INLINE void ContiguousStringA::pop_back()
	{
		if (0 < _length)
		{
			setChar(_length - 1, 0);
			--_length;
		}
	}

	FS_INLINE const bool ContiguousStringA::empty() const noexcept
	{
		return (_length == 0);
	}

	FS_INLINE const uint32 ContiguousStringA::length() const noexcept
	{
		return _length;
	}

	FS_INLINE const char* const ContiguousStringA::c_str() const noexcept
	{
		return reinterpret_cast<const char*>(_memoryAccessor.getMemory());
	}

	FS_INLINE const char ContiguousStringA::getChar(const uint32 at) const noexcept
	{
		if (_length <= at)
		{
			return 0;
		}
		return c_str()[at];
	}

	FS_INLINE const char ContiguousStringA::front() const noexcept
	{
		return getChar(0);
	}

	FS_INLINE const char ContiguousStringA::back() const noexcept
	{
		return getChar(_length - 1);
	}

	FS_INLINE const uint64 ContiguousStringA::hash() const noexcept
	{
		if (_cachedHash == 0)
		{
			_cachedHash = fs::StringUtil::hashRawString64(c_str());
		}
		return _cachedHash;
	}

	FS_INLINE void ContiguousStringA::setMemoryInternal(const char* const rawString, const uint32 offset)
	{
		static const char nullChar = 0;

		if (rawString == nullptr)
		{
			_memoryAccessor.setMemory(&nullChar);
		}
		else
		{
			const uint32 rawStringLength = fs::StringUtil::strlen(rawString);
			_memoryAccessor.setMemory(rawString, offset, rawStringLength);
			_memoryAccessor.setMemory(&nullChar, _length, 1);
		}

		_cachedHash = 0;
	}
}
