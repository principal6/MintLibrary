#include <MintGame/Include/GameBase.h>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintPlatform/Include/Window.h>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/ImageLoader.h>
#include <MintRendering/Include/ImageRenderer.h>
#include <MintAudio/Include/AudioSystem.h>
#include <MintGame/Include/DeltaTimer.h>
#include <MintGame/Include/TileMap.h>


namespace mint
{
	namespace Game
	{
		GameBase2D::GameBase2D(const StringA& title, const Int2& windowSize)
			: _window{ MINT_NEW(Window) }
			, _imageLoader{ MINT_NEW(Rendering::ImageLoader) }
		{
			WindowCreationDesc windowCreationDesc;
			windowCreationDesc._size = windowSize;
			StringW titleW;
			StringUtil::ConvertStringAToStringW(title, titleW);
			windowCreationDesc._title = titleW.CString();
			if (_window->Create(windowCreationDesc) == false)
			{
				MINT_NEVER;
			}

			_graphicDevice.Assign(MINT_NEW(Rendering::GraphicDevice, *_window, true));
			if (_graphicDevice->Initialize() == false)
			{
				MINT_NEVER;
			}

			_characterRenderer.Assign(MINT_NEW(Rendering::ImageRenderer, *_graphicDevice, 1));
			_mapRenderer.Assign(MINT_NEW(Rendering::ImageRenderer, *_graphicDevice, 2));

			_audioSystem.Assign(MINT_NEW(AudioSystem));
		}

		GameBase2D::~GameBase2D()
		{
			__noop;
		}

		bool GameBase2D::IsRunning()
		{
			return _window->IsRunning();
		}

		void GameBase2D::BeginRendering()
		{
			_graphicDevice->BeginRendering();

			Render();
		}

		void GameBase2D::DrawTextToScreen(const StringA& text, const Int2& position, const ByteColor& color)
		{
			_graphicDevice->SetSolidCullFrontRasterizer();

			Rendering::ShapeRendererContext& shapeRendererContext = _graphicDevice->GetShapeRendererContext();
			shapeRendererContext.SetTextColor(color);
			StringW textW;
			StringUtil::ConvertStringAToStringW(text, textW);
			shapeRendererContext.DrawDynamicText(textW.CString(), Float4(static_cast<float>(position._x), static_cast<float>(position._y), 0, 1), Rendering::FontRenderingOption());
			shapeRendererContext.Render();
		}

		void GameBase2D::EndRendering()
		{
			_graphicDevice->EndRendering();
		}

		void GameBase2D::Render()
		{
			_graphicDevice->SetViewProjectionMatrix(Float4x4::kIdentity, _graphicDevice->GetScreenSpace2DProjectionMatrix());

			_graphicDevice->SetSolidCullBackRasterizer();

			_tileMap.Draw(*_mapRenderer);
			_mapRenderer->Render();

			Rendering::ShapeRendererContext& shapeRendererContext = _graphicDevice->GetShapeRendererContext();
			_characterAnimationSet.Update(DeltaTimer::GetInstance().GetDeltaTimeSec());
			const Rendering::SpriteAnimation& characterCurrentAnimation = _characterAnimationSet.GetCurrentAnimation();
			_characterRenderer->DrawImageScreenSpace(Float2(64, 64), Float2(128, 128), characterCurrentAnimation.GetCurrentFrameUV0(), characterCurrentAnimation.GetCurrentFrameUV1());
			_characterRenderer->Render();

			_graphicDevice->SetSolidCullFrontRasterizer();
			shapeRendererContext.SetTextColor(Color::kBlack);
			shapeRendererContext.DrawDynamicText(L"WELCOME!", Float4(10, 10, 0, 1), Rendering::FontRenderingOption());
			shapeRendererContext.Render();
		}

		void GameBase2D::LoadTileMap(const StringA& tileMapeFileName)
		{
			_tileMap.Load(tileMapeFileName);
			_tileSetImage = LoadImageFile(_tileMap.getTileSet().getImageFileName());
			SetTileMapImage(_tileSetImage);
		}

		Image GameBase2D::LoadImageFile(const StringA& imageFileName)
		{
			Rendering::ByteColorImage image;
			_imageLoader->LoadImage_(imageFileName, image);
			Rendering::GraphicResourcePool& resourcePool = _graphicDevice->GetResourcePool();
			return Image(resourcePool.AddTexture2D(image));
		}

		void GameBase2D::SetCharacterImage(const Image& image)
		{
			Rendering::GraphicResourcePool& resourcePool = _graphicDevice->GetResourcePool();
			resourcePool.GetResource(image._graphicObjectID).BindToShader(Rendering::GraphicShaderType::PixelShader, _characterTextureSlot);
		}

		void GameBase2D::SetCharacterAnimationSet(const Rendering::SpriteAnimationSet& spriteAnimationSet)
		{
			_characterAnimationSet = spriteAnimationSet;
		}

		void GameBase2D::SetTileMapImage(const Image& image)
		{
			Rendering::GraphicResourcePool& resourcePool = _graphicDevice->GetResourcePool();
			resourcePool.GetResource(image._graphicObjectID).BindToShader(Rendering::GraphicShaderType::PixelShader, _tileMapTextureSlot);
		}

		void GameBase2D::SetBackgroundMusic(const StringReferenceA& audioFileName)
		{
			_backgroundMusic.Assign(MINT_NEW(AudioObject));
			_audioSystem->LoadAudioMP3(audioFileName.CString(), *_backgroundMusic);
			_backgroundMusic->Play();
		}
	}
}
