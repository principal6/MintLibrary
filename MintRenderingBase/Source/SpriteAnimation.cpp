#include <MintRenderingBase/Include/SpriteAnimation.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/String.hpp>


namespace mint
{
	namespace Rendering
	{
#pragma region SpriteAnimation
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

		void SpriteAnimation::AddFrames(const Float2& offsetInTexture, const Float2& sizeInTexture, uint32 rowIndex, uint32 rowCount, uint32 column, bool flipsHorz)
		{
			for (uint32 row = rowIndex; row < rowIndex + rowCount; ++row)
			{
				AddFrame(offsetInTexture, sizeInTexture, row, column, flipsHorz);
			}
		}

		void SpriteAnimation::AddFrame(const Float2& offsetInTexture, const Float2& sizeInTexture, uint32 row, uint32 column, bool flipsHorz)
		{
			if (flipsHorz)
			{
				AddFrame(offsetInTexture + Float2(sizeInTexture._x * (row + 1), sizeInTexture._y * column), Float2(-sizeInTexture._x, sizeInTexture._y));
			}
			else
			{
				AddFrame(offsetInTexture + Float2(sizeInTexture._x * row, sizeInTexture._y * column), sizeInTexture);
			}
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
#pragma endregion

#pragma region
		SpriteAnimationSet::SpriteAnimationSet()
			: _currentAnimationIndex{ 0 }
		{
			__noop;
		}

		void SpriteAnimationSet::AddAnimation(const StringW& animationName, SpriteAnimation&& animation)
		{
			_spriteAnimations.PushBack({ animationName, std::move(animation) });
		}

		void SpriteAnimationSet::AddAnimation(const StringW& animationName, const SpriteAnimation& animation)
		{
			_spriteAnimations.PushBack({ animationName, animation });
		}

		void SpriteAnimationSet::SetAnimation(const StringW& animationName)
		{
			const uint32 animationCount = _spriteAnimations.Size();
			for (uint32 i = 0; i < animationCount; i++)
			{
				if (_spriteAnimations[i]._name == animationName)
				{
					_currentAnimationIndex = i;
					return;
				}
			}
		}

		void SpriteAnimationSet::SetAnimationByIndex(uint32 animationIndex)
		{
			_currentAnimationIndex = animationIndex;

			if (_currentAnimationIndex >= _spriteAnimations.Size())
			{
				_currentAnimationIndex = 0;
			}
		}

		void SpriteAnimationSet::SetAnimationNextInOrder()
		{
			SetAnimationByIndex(_currentAnimationIndex + 1);
		}

		void SpriteAnimationSet::Update(float deltaTime)
		{
			GetCurrentNamedSpriteAnimation()._spriteAnimation.Update(deltaTime);
		}

		const SpriteAnimation& SpriteAnimationSet::GetCurrentAnimation() const
		{
			MINT_ASSERT(_spriteAnimations.IsEmpty() == false, "No animation was added!");
			return _spriteAnimations[_currentAnimationIndex]._spriteAnimation;
		}

		const StringW& SpriteAnimationSet::GetCurrentAnimationName() const
		{
			MINT_ASSERT(_spriteAnimations.IsEmpty() == false, "No animation was added!");
			return _spriteAnimations[_currentAnimationIndex]._name;
		}

		SpriteAnimationSet::NamedSpriteAnimation& SpriteAnimationSet::GetCurrentNamedSpriteAnimation()
		{
			MINT_ASSERT(_spriteAnimations.IsEmpty() == false, "No animation was added!");
			return _spriteAnimations[_currentAnimationIndex];
		}
#pragma endregion
	}
}
