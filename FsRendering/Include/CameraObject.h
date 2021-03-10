#pragma once


#ifndef FS_CAMERA_OBJECT_H
#define FS_CAMERA_OBJECT_H


#include <FsRendering/Include/Object.h>


namespace fs
{
	namespace Rendering
	{
		class ObjectManager;


		class CameraObject final : public Object
		{
			friend ObjectManager;

		public:
			enum class MoveDirection
			{
				Forward,
				Backward,
				Leftward,
				Rightward,
				Upward,
				Downward
			};

		private:
									CameraObject();
									
		public:
			virtual					~CameraObject();

		public:
			void					move(const MoveDirection moveDirection);
		
		public:
			void					rotatePitch(const float angle);
			void					rotateYaw(const float angle);

		public:
			fs::Float4x4			getViewMatrix() const noexcept;
		
		private:
			fs::Float4x4			getRotatedFrameMatrix() const noexcept;

		private:
			fs::Float3				_baseUpDirection;
			fs::Float3				_baseForwardDirection;
			fs::Float3				_focusOffset;

		private:
			mutable fs::Float3		_forwardDirectionFinal;

		private:
			float					_nearZ;
			float					_farZ;
			float					_fov;
			float					_pitch;
			float					_yaw;
			float					_roll;
			float					_movementFactor;
			float					_rotationFactor;
		};
	}
}


#endif // !FS_CAMERA_OBJECT_H
