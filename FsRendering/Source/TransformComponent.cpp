#include <stdafx.h>
#include <FsRendering/Include/TransformComponent.h>


namespace fs
{
	namespace Rendering
	{
		fs::Float4x4 fs::Rendering::Srt::toMatrix() const noexcept
		{
			return fs::Float4x4::srtMatrix(_scale, _rotation, _translation);
		}
	}
}
