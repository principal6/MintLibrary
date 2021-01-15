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


#endif
