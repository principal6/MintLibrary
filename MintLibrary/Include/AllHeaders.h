#pragma once


#ifndef _MINT_LIBRARY_ALL_HEADERS_H_
#define _MINT_LIBRARY_ALL_HEADERS_H_


#include <MintCommon/Include/AllHeaders.h>


namespace mint
{
	class Library
	{
	public:
		static void Initialize() noexcept;

		static uint16 GetVersionMajor() noexcept;
		static uint16 GetVersionMinor() noexcept;

		static void PrintVersion() noexcept;
		static void PrintCoordinateSystem() noexcept;
	};
}


#include <MintMath/Include/AllHeaders.h>

#include <MintContainer/Include/AllHeaders.h>

#include <MintPlatform/Include/AllHeaders.h>

#include <MintReflection/Include/AllHeaders.h>

#include <MintRendering/Include/AllHeaders.h>

#include <MintLanguage/Include/AllHeaders.h>

#include <MintRendering/Include/AllHeaders.h>

#include <MintPhysics/Include/AllHeaders.h>

#include <MintGUI/Include/AllHeaders.h>

#include <MintAudio/Include/AllHeaders.h>

#include <MintApp/Include/AllHeaders.h>

#include <MintGame/Include/AllHeaders.h>


#endif // !_MINT_LIBRARY_ALL_HEADERS_H_
