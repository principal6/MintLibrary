#pragma once


#ifndef FS_TRANSFORM_COMPONENT_H
#define FS_TRANSFORM_COMPONENT_H


#include <CommonDefinitions.h>

#include <FsRendering/Include/ObjectComponent.h>

#include <FsMath/Include/Float4x4.h>
#include <FsMath/Include/Quaternion.h>


namespace fs
{
	namespace Rendering
	{
		struct Srt
		{
		public:
			fs::Float4x4			toMatrix() const noexcept { return fs::Float4x4::srtMatrix(_scale, _rotation, _translation); }
		
		public:
			fs::Float3				_scale			= fs::Float3::kUnitScale;
			fs::Quaternion			_rotation;
			fs::Float3				_translation;
		};


		class TransformComponent : public ObjectComponent
		{
		public:
									TransformComponent() : ObjectComponent(ObjectComponentType::TransformComponent) { __noop; }
									TransformComponent(const ObjectComponentType derivedType) : ObjectComponent(derivedType) { __noop; }
			virtual					~TransformComponent() { __noop; }

		public:
			Srt						_srt;
		};
	}
}


#endif // !FS_TRANSFORM_COMPONENT_H
