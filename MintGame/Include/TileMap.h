#pragma once


#ifndef _MINT_GAME_TILE_MAP_H_
#define _MINT_GAME_TILE_MAP_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StringUtil.h>
#include <MintMath/Include/Int2.h>


namespace mint
{
	namespace Rendering
	{
		class ImageRenderer;
	}
}

namespace mint
{
	namespace Game
	{
		class TileSet
		{
		public:
			TileSet();
			~TileSet() = default;

		public:
			bool Load(const StringA& tileSetFileName);
			const StringA& getImageFileName() const { return _imageFileName; }
			uint32 getTileWidth() const { return _tileWidth; }
			uint32 getTileHeight() const { return _tileHeight; }
			uint32 getImageWidth() const { return _imageWidth; }
			uint32 getImageHeight() const { return _imageHeight; }
			Int2 getTileCoordinates(uint32 tileIndex) const;

		private:
			StringA _tileSetFileName;
			uint32 _tileWidth;
			uint32 _tileHeight;
			uint32 _tileCount;
			uint32 _columnCount;
			StringA _imageFileName;
			uint32 _imageWidth;
			uint32 _imageHeight;
		};

		class TileMap
		{
			template<typename T>
			using Vector = mint::Vector<T>;
			using StringA = mint::StringA;
			using StringReferenceA = mint::StringReferenceA;
			using ImageRenderer = mint::Rendering::ImageRenderer;

		public:
			TileMap();
			~TileMap() = default;

		public:
			bool Load(const StringA& tileMapFileName);
			void Draw(ImageRenderer& imageRenderer) const;

		public:
			const TileSet& GetTileSet() const { return _tileSet; }
			const Vector<uint32>& GetTiles() const { return _tiles; }
			uint32 GetWidth() const { return _width; }
			uint32 GetHeight() const { return _height; }

		private:
			Vector<uint32> _tiles;
			uint32 _width;
			uint32 _height;
			uint32 _tileWidth;
			uint32 _tileHeight;
			TileSet _tileSet;
		};
	}
}


#endif // !_MINT_GAME_TILE_MAP_H_
