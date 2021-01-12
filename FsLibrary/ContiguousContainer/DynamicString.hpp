#pragma once


#include <stdafx.h>
#include <FsLibrary/ContiguousContainer/DynamicString.h>

#include <FsLibrary/Container/StringUtil.h>


namespace fs
{
#pragma region Static functions
	template<class T>
	FS_INLINE const DynamicStringA DynamicStringA::from_value(const typename std::enable_if<std::is_literal_type<T>::value, T>::type value) noexcept
	{
		return DynamicStringA(std::to_string(value).c_str());
	}

	template<>
	FS_INLINE const DynamicStringA DynamicStringA::from_value<bool>(const bool value) noexcept
	{
		return DynamicStringA((true == value) ? "true" : "false");
	}

	FS_INLINE const bool DynamicStringA::to_bool(const DynamicStringA& dynamicString) noexcept
	{
		return dynamicString == "true";
	}

	FS_INLINE const int32 DynamicStringA::to_int32(const DynamicStringA& dynamicString) noexcept
	{
		return atoi(dynamicString.c_str());
	}

	FS_INLINE const uint32 DynamicStringA::to_uint32(const DynamicStringA& dynamicString) noexcept
	{
		return static_cast<uint32>(atoll(dynamicString.c_str()));
	}

	FS_INLINE const float DynamicStringA::to_float(const DynamicStringA& dynamicString) noexcept
	{
		return static_cast<float>(atof(dynamicString.c_str()));
	}

	FS_INLINE const double DynamicStringA::to_double(const DynamicStringA& dynamicString) noexcept
	{
		return atof(dynamicString.c_str());
	}
#pragma endregion


	FS_INLINE DynamicStringA& DynamicStringA::operator=(const char* const rawString)
	{
		assign(rawString);

		return *this;
	}

	FS_INLINE DynamicStringA& DynamicStringA::operator=(const DynamicStringA& rhs)
	{
		if (this != &rhs)
		{
			assign(rhs);
		}
		return *this;
	}

	FS_INLINE DynamicStringA& DynamicStringA::operator=(DynamicStringA&& rhs) noexcept
	{
		if (this != &rhs)
		{
			assign(std::forward<DynamicStringA>(rhs));
		}
		return *this;
	}

	FS_INLINE DynamicStringA& DynamicStringA::operator+=(const char* const rawString) noexcept
	{
		append(rawString);
		return *this;
	}

	FS_INLINE DynamicStringA& DynamicStringA::operator+=(const DynamicStringA& rhs) noexcept
	{
		append(rhs);
		return *this;
	}

	FS_INLINE const DynamicStringA DynamicStringA::operator+(const char* const rawString) const noexcept
	{
		DynamicStringA result(*this);
		if (fs::StringUtil::isNullOrEmpty(rawString) == false)
		{
			result.append(rawString);
		}
		return result;
	}

	FS_INLINE const DynamicStringA DynamicStringA::operator+(const DynamicStringA& rhs) const noexcept
	{
		DynamicStringA result(*this);
		result.append(rhs);
		return result;
	}

	FS_INLINE const bool DynamicStringA::operator==(const char* const rawString) const noexcept
	{
		return compare(rawString);
	}

	FS_INLINE const bool DynamicStringA::operator==(const DynamicStringA& rhs) const noexcept
	{
		return compare(rhs);
	}

	FS_INLINE const bool DynamicStringA::operator!=(const char* const rawString) const noexcept
	{
		return !compare(rawString);
	}

	FS_INLINE const bool DynamicStringA::operator!=(const DynamicStringA& rhs) const noexcept
	{
		return !compare(rhs);
	}

	FS_INLINE void DynamicStringA::assign(const char* const rawString)
	{
		assign(rawString, fs::StringUtil::strlen(rawString));
	}

	FS_INLINE DynamicStringA DynamicStringA::substr(const StringRange& stringRange) const noexcept
	{
		return substr(stringRange._offset, stringRange._length);
	}

	FS_INLINE void DynamicStringA::pop_back()
	{
		if (0 < _length)
		{
			setChar(_length - 1, 0);
			--_length;
		}
	}

	FS_INLINE const bool DynamicStringA::empty() const noexcept
	{
		return (_length == 0);
	}

	FS_INLINE const uint32 DynamicStringA::length() const noexcept
	{
		return _length;
	}

	FS_INLINE const char* const DynamicStringA::c_str() const noexcept
	{
		return reinterpret_cast<const char*>(_memoryAccessor.getMemory());
	}

	FS_INLINE const char DynamicStringA::getChar(const uint32 at) const noexcept
	{
		if (_length <= at)
		{
			return 0;
		}
		return c_str()[at];
	}

	FS_INLINE const char DynamicStringA::front() const noexcept
	{
		return getChar(0);
	}

	FS_INLINE const char DynamicStringA::back() const noexcept
	{
		return getChar(_length - 1);
	}

	FS_INLINE const uint64 DynamicStringA::hash() const noexcept
	{
		if (_cachedHash == 0)
		{
			_cachedHash = fs::StringUtil::hashRawString64(c_str());
		}
		return _cachedHash;
	}

	FS_INLINE void DynamicStringA::setMemoryInternal(const char* const rawString, const uint32 offset)
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
