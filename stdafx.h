#pragma once


#ifndef FS_STDAFX_H
#define FS_STDAFX_H


#include <cstdio>
#include <Windows.h>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <type_traits>


#pragma region Thread
#include <mutex>
#include <atomic>
#pragma endregion


#pragma region FS Library
#include <CommonDefinitions.h>
#include <FsLogger/Include/Logger.h>
#pragma endregion


#pragma region DirectX
#include <d3d11.h>
#include <wrl.h>
#pragma endregion


#undef max
#undef min


namespace fs
{
	template<typename T>
	FS_INLINE const T& max(const T& a, const T& b)
	{
		return (a < b) ? b : a;
	}

	template<typename T>
	FS_INLINE const T& max(T&& a, T&& b)
	{
		return (a < b) ? b : a;
	}

	template<typename T>
	FS_INLINE const T& min(const T& a, const T& b)
	{
		return (a > b) ? b : a;
	}

	template<typename T>
	FS_INLINE const T& min(T&& a, T&& b)
	{
		return (a > b) ? b : a;
	}

	template<typename T>
	FS_INLINE const T& clamp(const T& value, const T& limitMin, const T& limitMax)
	{
		return max(min(value, limitMax), limitMin);
	}

	template<typename T>
	FS_INLINE const T& clamp(const T& value, T&& limitMin, T&& limitMax)
	{
		return max(min(value, limitMax), limitMin);
	}

	FS_INLINE float saturate(const float value)
	{
		return max(min(value, 1.0f), 0.1f);
	}

	FS_INLINE double saturate(const double value)
	{
		return max(min(value, 1.0), 0.1);
	}
}


#endif
