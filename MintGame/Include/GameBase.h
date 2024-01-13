#pragma once


#ifndef _MINT_GAME_GAME_BASE_H_
#define _MINT_GAME_GAME_BASE_H_


#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/String.h>
#include <MintRenderingBase/Include/GraphicObject.h>
#include <MintRenderingBase/Include/SpriteAnimation.h>


namespace mint
{
	class Window;
	class AudioObject;
	class AudioSystem;

	namespace Rendering
	{
		class GraphicDevice;
		class GraphicObjectID;
		class ImageLoader;
		class ImageRenderer;
	}

	namespace Game
	{
		class GameBase2D;
	}
}

namespace mint
{
	namespace Game
	{
		class GameBase2D;

		class Image
		{
			friend GameBase2D;

		public:
			Image() = default;
			Image(Rendering::GraphicObjectID graphicObjectID) : _graphicObjectID{ graphicObjectID } { __noop; }
			~Image() = default;

		private:
			Rendering::GraphicObjectID _graphicObjectID;
		};

		class GameBase2D
		{
		public:
			GameBase2D(const StringA& title, const Int2& windowSize);
			virtual ~GameBase2D();

		public:
			bool IsRunning();
			void BeginRendering();
			void DrawTextToScreen(const StringA& text, const Int2& position, const ByteColor& color);
			void EndRendering();

		public:
			void LoadTileMap(const StringA& tileMapeFileName);
			Image LoadImageFile(const StringA& imageFileName);
			void SetCharacterImage(const Image& image);
			void SetCharacterAnimationSet(const Rendering::SpriteAnimationSet& spriteAnimationSet);
			void SetTileMapImage(const Image& image);
			void SetBackgroundMusic(const StringReferenceA& audioFileName);

		private:
			void Render();

		protected:
			OwnPtr<Window> _window;
			OwnPtr<Rendering::GraphicDevice> _graphicDevice;
			OwnPtr<Rendering::ImageLoader> _imageLoader;

		protected:
			TileMap _tileMap;
			Image _tileSetImage;
			Rendering::SpriteAnimationSet _characterAnimationSet;

		protected:
			static constexpr const uint32 _characterTextureSlot = 1;
			static constexpr const uint32 _tileMapTextureSlot = 2;
			OwnPtr<Rendering::ImageRenderer> _characterRenderer;
			OwnPtr<Rendering::ImageRenderer> _mapRenderer;

		protected:
			OwnPtr<AudioSystem> _audioSystem;
			OwnPtr<AudioObject> _backgroundMusic;
		};
	}
}


#endif // !_MINT_GAME_GAME_BASE_H_
