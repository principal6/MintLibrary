#pragma once


#ifndef _MINT_RENDERING_SPRITE_ANIMATION_H_
#define _MINT_RENDERING_SPRITE_ANIMATION_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
	namespace Rendering
	{
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
			~SpriteAnimation() = default;

		public:
			void AddFrame(const Float2& positionInTexrue, const Float2& sizeInTexture);
			void Update(float deltaTime);
			void SetCurrentFrame(const uint32 frameIndex);
			void SetLoops(bool loops);

		public:
			Float2 GetCurrentFrameUV0() const;
			Float2 GetCurrentFrameUV1() const;

		private:
			uint32 GetCurrentFrameIndex() const;
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
	}
}


#endif // !_MINT_RENDERING_SPRITE_ANIMATION_H_
