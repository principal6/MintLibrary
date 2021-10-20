#pragma once


#ifndef MINT_LIBRARY_ALL_HEADERS_H
#define MINT_LIBRARY_ALL_HEADERS_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintCommon/Include/Logger.h>


namespace mint
{
    class LibraryVersion
    {
    public:
        static const uint16 getVersionMajor() noexcept;
        static const uint16 getVersionMinor() noexcept;
        static void         printVersion() noexcept;
    };
}


#include <MintLibrary/Include/Algorithm.h>

#include <MintLibrary/Include/Profiler.h>

#include <MintMath/Include/AllHeaders.h>

#include <MintContainer/Include/AllHeaders.h>

#include <MintPlatform/Include/AllHeaders.h>

#include <MintRenderingBase/Include/AllHeaders.h>

#include <MintLanguage/Include/AllHeaders.h>

#include <MintRendering/Include/AllHeaders.h>

#include <MintGame/Include/AllHeaders.h>


#endif // !MINT_LIBRARY_ALL_HEADERS_H
