#pragma once


#ifndef FS_LIBRARY_H
#define FS_LIBRARY_H


#include <FsCommon/Include/CommonDefinitions.h>

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

#include <FsLibrary/Include/Algorithm.hpp>
#include <FsLibrary/Include/ContiguousContainer.h>
#include <FsLibrary/Include/Memory.h>

#include <FsLibrary/Include/Profiler.h>

#include <FsMath/Include/Math.h>

#include <FsContainer/Include/Container.h>

#include <FsPlatform/Include/Platform.h>

#include <FsRenderingBase/Include/RenderingBase.h>

#include <FsRendering/Include/Rendering.h>


#endif // !FS_LIBRARY_H
