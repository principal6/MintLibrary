#pragma once


#include <CommonDefinitions.h>


#ifndef FS_ASSERT_H
#define FS_ASSERT_H


namespace fs
{
#pragma region Assertion
	#if defined FS_DEBUG
		#define FS_ASSERT(author, expression, content) if (!(expression)) { static char staticBuffer[300]{}; sprintf_s(staticBuffer, "[%s] %s\n%s: %d", author, content, __FILE__, __LINE__); MessageBoxA(nullptr, staticBuffer, "FS ASSERT", MB_OK); DebugBreak(); }
	#else
		#define FS_ASSERT(author, expression, content)
	#endif
#pragma endregion
}


#endif // !FS_ASSERT_H
