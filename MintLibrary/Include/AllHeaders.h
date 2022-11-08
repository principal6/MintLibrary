#pragma once


#ifndef MINT_LIBRARY_ALL_HEADERS_H
#define MINT_LIBRARY_ALL_HEADERS_H


#include <MintCommon/Include/AllHeaders.h>


namespace mint
{
    class Library
    {
    public:
        static void     initialize() noexcept;
        
        static uint16   getVersionMajor() noexcept;
        static uint16   getVersionMinor() noexcept;
        
        static void     printVersion() noexcept;
        static void     printCoordinateSystem() noexcept;
    };
}


#include <MintLibrary/Include/Algorithm.h>

#include <MintMath/Include/AllHeaders.h>

#include <MintContainer/Include/AllHeaders.h>

#include <MintPlatform/Include/AllHeaders.h>

#include <MintRenderingBase/Include/AllHeaders.h>

#include <MintLanguage/Include/AllHeaders.h>

#include <MintRendering/Include/AllHeaders.h>

#include <MintGame/Include/AllHeaders.h>


#endif // !MINT_LIBRARY_ALL_HEADERS_H
