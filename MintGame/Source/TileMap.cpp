#include <MintGame/Include/TileMap.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintMath/Include/Float2.h>
#include <MintRendering/Include/ImageRenderer.h>
#include <MintPlatform/Include/XML.h>
#include <MintPhysics/Include/Intersection.hpp>


namespace mint
{
	namespace Game
	{
		TileSet::TileSet()
			: _tileWidth{ 0 }
			, _tileHeight{ 0 }
			, _tileCount{ 0 }
			, _columnCount{ 0 }
			, _imageWidth{ 0 }
			, _imageHeight{ 0 }
		{
			__noop;
		}

		bool TileSet::Load(const StringA& tileSetFileName)
		{
			XML xml;
			if (xml.Parse(tileSetFileName) == false)
			{
				return false;
			}

			_tileSetFileName = tileSetFileName;

			const XML::Node* const tileSetNode = xml.GetRootNode();
			_tileWidth = StringUtil::StringToUint32(StringReferenceA(tileSetNode->FindAttribute("tilewidth")->GetValue()));
			_tileHeight = StringUtil::StringToUint32(StringReferenceA(tileSetNode->FindAttribute("tileheight")->GetValue()));
			_tileCount = StringUtil::StringToUint32(StringReferenceA(tileSetNode->FindAttribute("tilecount")->GetValue()));
			_columnCount = StringUtil::StringToUint32(StringReferenceA(tileSetNode->FindAttribute("columns")->GetValue()));

			XML::Node* const imageNode = tileSetNode->GetFirstChildNode();
			_imageFileName = "Assets/";
			_imageFileName += imageNode->FindAttribute("source")->GetValue();
			_imageWidth = StringUtil::StringToUint32(StringReferenceA(imageNode->FindAttribute("width")->GetValue()));
			_imageHeight = StringUtil::StringToUint32(StringReferenceA(imageNode->FindAttribute("height")->GetValue()));

			MINT_ASSERT(_columnCount * _tileWidth == _imageWidth, "Wrong image width!");
			const uint32 rowCount = _tileCount / _columnCount;
			MINT_ASSERT(rowCount * _tileHeight == _imageHeight, "Wrong image height!");

			const XML::Node* const firstTileNode = imageNode->GetNextSiblingNode();
			if (firstTileNode != nullptr)
			{
				_tileCollisionShapes.Resize(_tileCount);

				const float halfTileWidth = 0.5f * _tileWidth;
				const float halfTileHeight = 0.5f * _tileHeight;
				const Float2 coordsFixer(-halfTileWidth, +halfTileHeight);

				for (const XML::Node* tileNode = firstTileNode; tileNode != nullptr; tileNode = tileNode->GetNextSiblingNode())
				{
					const uint32 id = StringUtil::StringToUint32(StringReferenceA(tileNode->GetFirstAttribute()->GetValue()));;
					const XML::Node* const objectGroupNode = tileNode->GetFirstChildNode();
					const XML::Node* const objectNode = objectGroupNode->GetFirstChildNode();
					const XML::Node* const objectChildNode = objectNode->GetFirstChildNode();
					if (objectChildNode == nullptr)
					{
						// bounding box
						const float x = StringUtil::StringToFloat(StringReferenceA(objectNode->FindAttribute("x")->GetValue()));
						const float y = StringUtil::StringToFloat(StringReferenceA(objectNode->FindAttribute("y")->GetValue()));
						const float width = StringUtil::StringToFloat(StringReferenceA(objectNode->FindAttribute("width")->GetValue()));
						const float height = StringUtil::StringToFloat(StringReferenceA(objectNode->FindAttribute("height")->GetValue()));
						Vector<Float2> points;
						points.Resize(4);
						points[0] = Float2(x, -y) + coordsFixer;
						points[1] = Float2(x, -y - height) + coordsFixer;
						points[2] = Float2(x + width, -y - height) + coordsFixer;
						points[3] = Float2(x + width, -y) + coordsFixer;
						_tileCollisionShapes[id].Assign(MINT_NEW(Physics::ConvexCollisionShape2D, Physics::ConvexCollisionShape2D::MakeFromPoints(Float2::kZero, points)));
					}
					else
					{
						MINT_ASSERT(StringUtil::Equals(objectChildNode->GetName(), "polygon"), "Non-polygon objects are not supported!");
						Vector<StringA> textPoints;
						StringUtil::Tokenize(StringA(objectChildNode->GetFirstAttribute()->GetValue()), ' ', textPoints);
						Vector<Float2> points;
						points.Resize(textPoints.Size());
						{
							Vector<StringA> coords;
							for (uint32 i = 0; i < textPoints.Size(); ++i)
							{
								StringUtil::Tokenize(textPoints[i], ',', coords);
								points[i]._x = StringUtil::StringToFloat(coords[0]) - halfTileWidth;
								points[i]._y = -(StringUtil::StringToFloat(coords[1]) - halfTileHeight);
							}
						}
						_tileCollisionShapes[id].Assign(MINT_NEW(Physics::ConvexCollisionShape2D, Physics::ConvexCollisionShape2D::MakeFromPoints(Float2::kZero, points)));
					}
				}
			}
			return true;
		}

		Int2 TileSet::GetTileCoordinates(uint32 tileIndex) const
		{
			int32 y = tileIndex / _columnCount;
			int32 x = tileIndex % _columnCount;
			return Int2(x, y);
		}

		TileMap::TileMap()
			: _width{ 0 }
			, _height{ 0 }
			, _tileWidth{ 0 }
			, _tileHeight{ 0 }
		{
			__noop;
		}

		bool TileMap::Load(const StringA& tileMapFileName)
		{
			XML xmlDocument;
			if (xmlDocument.Parse(tileMapFileName) == false)
			{
				return false;
			}

			const XML::Node* const mapNode = xmlDocument.GetRootNode();
			_width = StringUtil::StringToUint32(StringReferenceA(mapNode->FindAttribute("width")->GetValue()));
			_height = StringUtil::StringToInt32(StringReferenceA(mapNode->FindAttribute("height")->GetValue()));
			_tileWidth = StringUtil::StringToUint32(StringReferenceA(mapNode->FindAttribute("tilewidth")->GetValue()));
			_tileHeight = StringUtil::StringToUint32(StringReferenceA(mapNode->FindAttribute("tileheight")->GetValue()));

			XML::Node* const tileSetNode = mapNode->GetFirstChildNode();
			StringA tileSetFileName = "Assets/";
			tileSetFileName += tileSetNode->FindAttribute("source")->GetValue();
			if (_tileSet.Load(tileSetFileName) == false)
			{
				return false;
			}

			XML::Node* const layerNode = tileSetNode->GetNextSiblingNode();
			XML::Node* const layerDataNode = layerNode->GetFirstChildNode();
			StringA layerData = layerDataNode->GetText();
			StringUtil::Trim(layerData);
			Vector<StringA> tileStrings;
			StringUtil::Tokenize(layerData, ',', tileStrings);

			_tiles.Resize(tileStrings.Size());
			for (uint32 i = 0; i < _tiles.Size(); i++)
			{
				_tiles[i] = StringUtil::StringToUint32(tileStrings[i]);
			}

			MINT_ASSERT(_width * _height == _tiles.Size(), "Wrong width or tile count!");
			MINT_ASSERT(_tileWidth > 0, "Wrong Tile Width");
			MINT_ASSERT(_tileHeight > 0, "Wrong Tile Height");
			return true;
		}

		void TileMap::Draw(ImageRenderer& imageRenderer) const
		{
			const Vector<uint32>& tiles = GetTiles();
			const uint32 mapWidth = GetWidth();
			const uint32 mapHeight = tiles.Size() / mapWidth;
			const TileSet& tileSet = GetTileSet();
			const float tileWidth = static_cast<float>(tileSet.GetTileWidth());
			const float tileHeight = static_cast<float>(tileSet.GetTileHeight());
			const float tileHalfWidth = tileWidth * 0.5f;
			const float tileHalfHeight = tileHeight * 0.5f;
			const float yOffset = static_cast<float>(mapHeight) * tileHeight;
			for (uint32 x = 0; x < mapWidth; ++x)
			{
				for (uint32 y = 0; y < mapHeight; ++y)
				{
					const uint32 tileNumber = tiles[y * mapWidth + x];
					if (tileNumber == 0)
					{
						continue;
					}

					const uint32 tileID = tileNumber - 1;
					const Int2 tileCoordinates = tileSet.GetTileCoordinates(tileID);
					float tileU0 = static_cast<float>(tileCoordinates._x) * tileSet.GetTileWidth();
					float tileV0 = static_cast<float>(tileCoordinates._y) * tileSet.GetTileHeight();
					float tileU1 = tileU0 + tileSet.GetTileWidth();
					float tileV1 = tileV0 + tileSet.GetTileHeight();
					tileU0 /= tileSet.GetImageWidth();
					tileU1 /= tileSet.GetImageWidth();
					tileV0 /= tileSet.GetImageHeight();
					tileV1 /= tileSet.GetImageHeight();

					imageRenderer.DrawImage(Float2(tileHalfWidth + tileWidth * x, -tileHalfHeight + yOffset + -tileHeight * y), Float2(tileWidth, tileHeight)
						, Float2(tileU0, tileV0), Float2(tileU1, tileV1));
				}
			}
		}

		void TileMap::DrawCollisions(Rendering::ShapeRendererContext& shapeRendererContext) const
		{
			const Vector<uint32>& tiles = GetTiles();
			const uint32 mapWidth = GetWidth();
			const uint32 mapHeight = tiles.Size() / mapWidth;
			const TileSet& tileSet = GetTileSet();
			const float tileWidth = static_cast<float>(tileSet.GetTileWidth());
			const float tileHeight = static_cast<float>(tileSet.GetTileHeight());
			const float tileHalfWidth = tileWidth * 0.5f;
			const float tileHalfHeight = tileHeight * 0.5f;
			const float yOffset = static_cast<float>(mapHeight) * tileHeight;
			for (uint32 x = 0; x < mapWidth; ++x)
			{
				for (uint32 y = 0; y < mapHeight; ++y)
				{
					const uint32 tileNumber = tiles[y * mapWidth + x];
					if (tileNumber == 0)
					{
						continue;
					}

					const uint32 tileID = tileNumber - 1;
					const Float2 position = Float2(tileHalfWidth + tileWidth * x, -tileHalfHeight + yOffset + -tileHeight * y);
					tileSet.GetTileCollisionShapes()[tileID]->DebugDrawShape(shapeRendererContext, ByteColor(255, 0, 0), position);
				}
			}
		}
	}
}
