#pragma once


#ifndef FS_LIBRARY_ALL_HEADERS_H
#define FS_LIBRARY_ALL_HEADERS_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsCommon/Include/Logger.h>


namespace fs
{
    class FsLibraryVersion
    {
    public:
        static const uint16 getVersionMajor() noexcept;
        static const uint16 getVersionMinor() noexcept;
        static void         printVersion() noexcept;
    };
}


#include <FsLibrary/Include/Algorithm.h>

#include <FsLibrary/Include/Profiler.h>

#include <FsMath/Include/AllHeaders.h>

#include <FsContainer/Include/AllHeaders.h>

#include <FsPlatform/Include/AllHeaders.h>

#include <FsRenderingBase/Include/AllHeaders.h>

#include <FsRendering/Include/AllHeaders.h>


#endif // !FS_LIBRARY_ALL_HEADERS_H
