#pragma once


#include <stdafx.h>
#include <Container\DynamicString.h>


namespace fs
{
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
}
