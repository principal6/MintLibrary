﻿#pragma once


#ifndef FS_LIBRARY_H
#define FS_LIBRARY_H


#include <CommonDefinitions.h>

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

#include <FsLibrary/Algorithm.hpp>
#include <FsLibrary/ContiguousContainer/ContiguousContainer.h>
#include <FsLibrary/Memory/Memory.h>

#include <FsLibrary/Profiler/Profiler.h>

#include <FsMath/Include/Math.h>

#include <FsContainer/Include/Container.h>

#include <FsPlatform/Include/Platform.h>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/Language/Language.h>

#include <FsRendering/Include/Rendering.h>


#endif // !FS_LIBRARY_H
