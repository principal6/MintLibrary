#pragma once


#ifndef FS_STDAFX_H
#define FS_STDAFX_H


#include <cstdint>
#include <cstdio>
#include <Windows.h>
#include <vector>
#include <queue>
#include <unordered_map>

// === FsLibrary ===
#include <CommonDefinitions.h>
#include <Assert.h>
// ===


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