#pragma once


#ifndef FS_STDAFX_H
#define FS_STDAFX_H


#include <cstdio>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <type_traits>


#pragma region Platform
#include <Windows.h>
#undef max
#undef min
#pragma endregion


#pragma region Thread
#include <mutex>
#include <atomic>
#pragma endregion


#pragma region FS Library
#include <FsCommon/Include/Logger.h>
#pragma endregion


#pragma region DirectX
#include <d3d11.h>
#include <wrl.h>
#pragma endregion


#endif
