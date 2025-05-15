#include <MintRendering/Include/SpriteAnimation.h>
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

		SpriteAnimation::SpriteAnimation(const Float2& textureSize, float timePerFrame, const Float2& offsetInTexture, const Float2& sizeInTexture, uint32 rowIndex, uint32 rowCount, uint32 column)
			: SpriteAnimation(textureSize, timePerFrame)
		{
			AddFrames(offsetInTexture, sizeInTexture, rowIndex, rowCount, column);
		}

		SpriteAnimation::SpriteAnimation(const Float2& textureSize, float timePerFrame, const Float2& offsetInTexture, const Float2& sizeInTexture, uint32 frameCount)
			: SpriteAnimation(textureSize, timePerFrame)
		{
			const uint32 columnCount = static_cast<uint32>(textureSize._x / sizeInTexture._x);
			for (uint32 i = 0; i < frameCount; ++i)
			{
				uint32 row = i / columnCount;
				uint32 column = i % columnCount;
				AddFrame(offsetInTexture, sizeInTexture, row, column);
			}
		}

		void SpriteAnimation::AddFrames(const Float2& offsetInTexture, const Float2& sizeInTexture, uint32 rowIndex, uint32 rowCount, uint32 column)
		{
			for (uint32 row = rowIndex; row < rowIndex + rowCount; ++row)
			{
				AddFrame(offsetInTexture, sizeInTexture, row, column);
			}
		}

		void SpriteAnimation::AddFrame(const Float2& offsetInTexture, const Float2& sizeInTexture, uint32 row, uint32 column)
		{
			AddFrame(offsetInTexture + Float2(sizeInTexture._x * column, sizeInTexture._y * row), sizeInTexture);
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

		void SpriteAnimation::SetCurrentFrameByRatio(float ratio)
		{
			MINT_ASSERT(ratio >= 0.0f && ratio <= 1.0f, "Caller must guarantee this!");

			_elapsedTime = _totalTime * ratio;
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

		void SpriteAnimationSet::AddAnimation(const StringA& animationName, SpriteAnimation&& animation)
		{
			_spriteAnimations.PushBack({ animationName, std::move(animation) });
		}

		void SpriteAnimationSet::AddAnimation(const StringA& animationName, const SpriteAnimation& animation)
		{
			_spriteAnimations.PushBack({ animationName, animation });
		}

		void SpriteAnimationSet::SetAnimation(const StringA& animationName)
		{
			if (GetCurrentAnimationName() == animationName)
			{
				return;
			}

			uint32 animationIndex = 0;
			const uint32 animationCount = _spriteAnimations.Size();
			for (uint32 i = 0; i < animationCount; i++)
			{
				if (_spriteAnimations[i]._name == animationName)
				{
					animationIndex = i;
					break;
				}
			}

			SetAnimationByIndex(animationIndex);
		}

		void SpriteAnimationSet::SetAnimationByIndex(uint32 animationIndex)
		{
			if (_currentAnimationIndex == animationIndex)
			{
				return;
			}

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

		bool SpriteAnimationSet::IsValid() const
		{
			return _spriteAnimations.IsEmpty() == false;
		}

		const SpriteAnimation& SpriteAnimationSet::GetCurrentAnimation() const
		{
			MINT_ASSERT(_spriteAnimations.IsEmpty() == false, "No animation was added!");
			return _spriteAnimations[_currentAnimationIndex]._spriteAnimation;
		}

		const StringA& SpriteAnimationSet::GetCurrentAnimationName() const
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
