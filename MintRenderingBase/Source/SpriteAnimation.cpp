#include <MintRenderingBase/Include/SpriteAnimation.h>
#include <MintContainer/Include/Vector.hpp>


namespace mint
{
	namespace Rendering
	{
		SpriteAnimation::Frame::Frame(const Float2& positionInTexrue, const Float2& sizeInTexture)
			: _positionInTexrue{ positionInTexrue }
			, _sizeInTexture{ sizeInTexture }
		{
			__noop;
		}

		SpriteAnimation::SpriteAnimation(const Float2& textureSize, float timePerFrame)
			: _textureSize{ textureSize }
			, _timePerFrame{ timePerFrame }
			, _totalTime{ 0.0f }
			, _elapsedTime{ 0.0f }
			, _loops{ true }
		{
			__noop;
		}

		void SpriteAnimation::AddFrame(const Float2& positionInTexrue, const Float2& sizeInTexture)
		{
			_frames.PushBack(Frame(positionInTexrue, sizeInTexture));

			_totalTime = _timePerFrame * _frames.Size();
		}

		void SpriteAnimation::Update(float deltaTime)
		{
			_elapsedTime += deltaTime;

			if (_elapsedTime >= _totalTime)
			{
				_elapsedTime = (_loops ? 0.0f : _totalTime);
			}
		}
		
		void SpriteAnimation::SetCurrentFrame(const uint32 frameIndex)
		{
			_elapsedTime = _timePerFrame * frameIndex;
		}

		void SpriteAnimation::SetLoops(bool loops)
		{
			_loops = loops;
		}

		Float2 SpriteAnimation::GetCurrentFrameUV0() const
		{
			return GetFrameUV0(GetCurrentFrameIndex());
		}

		Float2 SpriteAnimation::GetCurrentFrameUV1() const
		{
			return GetFrameUV1(GetCurrentFrameIndex());
		}

		uint32 SpriteAnimation::GetCurrentFrameIndex() const
		{
			uint32 currentFrameIndex = static_cast<uint32>(_elapsedTime / _timePerFrame);
			if (currentFrameIndex >= _frames.Size())
			{
				currentFrameIndex = 0;
			}
			return currentFrameIndex;
		}

		Float2 SpriteAnimation::GetFrameUV0(uint32 frameIndex) const
		{
			if (frameIndex >= _frames.Size())
			{
				return Float2::kZero;
			}

			const float u0 = _frames[frameIndex]._positionInTexrue._x / _textureSize._x;
			const float v0 = _frames[frameIndex]._positionInTexrue._y / _textureSize._y;
			return Float2(u0, v0);
		}

		Float2 SpriteAnimation::GetFrameUV1(uint32 frameIndex) const
		{
			if (frameIndex >= _frames.Size())
			{
				return Float2::kZero;
			}

			const float u0 = _frames[frameIndex]._positionInTexrue._x / _textureSize._x;
			const float v0 = _frames[frameIndex]._positionInTexrue._y / _textureSize._y;
			const float u1 = u0 + _frames[frameIndex]._sizeInTexture._x / _textureSize._x;
			const float v1 = v0 + _frames[frameIndex]._sizeInTexture._y / _textureSize._y;
			return Float2(u1, v1);
		}
	}
}
