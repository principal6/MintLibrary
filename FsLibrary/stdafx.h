#pragma once


#ifndef FS_STDAFX_H
#define FS_STDAFX_H


#include <cstdint>
#include <cstdio>
#include <Windows.h>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <type_traits>

#pragma region FsLibrary
#include <CommonDefinitions.h>
#include <Assert.h>
#include <Logger/Logger.h>
#include <Logger/Logger.hpp>
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
	const T& max(const T& a, const T& b)
	{
		return (a < b) ? b : a;
	}

	template<typename T>
	const T& max(T&& a, T&& b)
	{
		return (a < b) ? b : a;
	}

	template<typename T>
	const T& min(const T& a, const T& b)
	{
		return (a > b) ? b : a;
	}

	template<typename T>
	const T& min(T&& a, T&& b)
	{
		return (a > b) ? b : a;
	}
}


#endif