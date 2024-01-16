#pragma once


#ifndef _MINT_RENDERING_SPRITE_ANIMATION_H_
#define _MINT_RENDERING_SPRITE_ANIMATION_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintContainer/Include/String.h>


namespace mint
{
	namespace Rendering
	{
#pragma region SpriteAnimation
		class SpriteAnimation
		{
		private:
			struct Frame
			{
				Frame(const Float2& positionInTexrue, const Float2& sizeInTexture);
				Float2 _positionInTexrue;
				Float2 _sizeInTexture;
			};

		public:
			SpriteAnimation(const Float2& textureSize, float timePerFrame);
			SpriteAnimation(const Float2& textureSize, float timePerFrame, const Float2& offsetInTexture, const Float2& sizeInTexture, uint32 rowIndex, uint32 rowCount, uint32 column);
			~SpriteAnimation() = default;

		public:
			void AddFrames(const Float2& offsetInTexture, const Float2& sizeInTexture, uint32 rowIndex, uint32 rowCount, uint32 column);
			void AddFrame(const Float2& offsetInTexture, const Float2& sizeInTexture, uint32 row, uint32 column);
			void AddFrame(const Float2& positionInTexrue, const Float2& sizeInTexture);

		public:
			void Update(float deltaTime);
			void SetCurrentFrame(const uint32 frameIndex);
			void SetLoops(bool loops);

		public:
			Float2 GetCurrentFrameUV0() const;
			Float2 GetCurrentFrameUV1() const;
			float GetElapsedTime() const { return _elapsedTime; }
			float GetTotalTime() const { return _totalTime; }
			uint32 GetCurrentFrameIndex() const;

		private:
			Float2 GetFrameUV0(uint32 frameIndex) const;
			Float2 GetFrameUV1(uint32 frameIndex) const;

		private:
			Float2 _textureSize;
			float _timePerFrame;
			Vector<Frame> _frames;
			float _totalTime;
			float _elapsedTime;
			bool _loops;
		};
#pragma endregion

#pragma region SpriteAnimationSet
		class SpriteAnimationSet
		{
			struct NamedSpriteAnimation
			{
				StringA _name;
				SpriteAnimation _spriteAnimation;
			};

		public:
			SpriteAnimationSet();
			~SpriteAnimationSet() = default;

		public:
			void AddAnimation(const StringA& animationName, SpriteAnimation&& animation);
			void AddAnimation(const StringA& animationName, const SpriteAnimation& animation);
			void SetAnimation(const StringA& animationName);
			void SetAnimationByIndex(uint32 animationIndex);
			void SetAnimationNextInOrder();
			void Update(float deltaTime);

		public:
			const SpriteAnimation& GetCurrentAnimation() const;
			const StringA& GetCurrentAnimationName() const;

		private:
			NamedSpriteAnimation& GetCurrentNamedSpriteAnimation();

		private:
			Vector<NamedSpriteAnimation> _spriteAnimations;
			uint32 _currentAnimationIndex;
		};
#pragma endregion
	}
}


#endif // !_MINT_RENDERING_SPRITE_ANIMATION_H_
