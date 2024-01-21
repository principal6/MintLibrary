#include <MintGame/Include/GameBase.h>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintPlatform/Include/Window.h>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/ImageLoader.h>
#include <MintRendering/Include/ImageRenderer.h>
#include <MintAudio/Include/AudioSystem.h>
#include <MintGame/Include/DeltaTimer.h>
#include <MintGame/Include/TileMap.h>
#include <MintGame/Include/ObjectPool.hpp>


namespace mint
{
	namespace Game
	{
#pragma region Condition
		bool Action::Condition::Evaluate() const
		{
			if (_value->GetType() == Value::Type::Bool)
			{
				const BoolValue& variableValue = static_cast<const BoolValue&>(*_variable->_value);
				const BoolValue& value = static_cast<const BoolValue&>(*_value);
				if (_operator == Operator::Equal)
				{
					return variableValue._value == value._value;
				}
				else
				{
					return variableValue._value != value._value;
				}
			}
			else if (_value->GetType() == Value::Type::Real)
			{
				const RealValue& variableValue = static_cast<const RealValue&>(*_variable->_value);
				const RealValue& value = static_cast<const RealValue&>(*_value);
				if (_operator == Operator::Equal)
				{
					return variableValue._value == variableValue._value;
				}
				else if (_operator == Operator::Less)
				{
					return variableValue._value < variableValue._value;
				}
				else if (_operator == Operator::LessEqual)
				{
					return variableValue._value <= variableValue._value;
				}
				else if (_operator == Operator::Greater)
				{
					return variableValue._value > variableValue._value;
				}
				else if (_operator == Operator::GreaterEqual)
				{
					return variableValue._value >= variableValue._value;
				}
				else
				{
					return variableValue._value != variableValue._value;
				}
			}
			else
			{
				MINT_NEVER;
			}
			return false;
		}
#pragma endregion

#pragma region ActionChart
		ActionChart::ActionChart()
			: _currentActionIndex{ 0 }
		{
			__noop;
		}

		bool ActionChart::Load(const StringA& fileName)
		{
			XML xml;
			if (xml.Parse(fileName) == false)
			{
				return false;
			}

			const XML::Node* const actionChartNode = xml.GetRootNode();
			for (const XML::Node* childNode = actionChartNode->GetFirstChildNode(); childNode != nullptr; childNode = childNode->GetNextSiblingNode())
			{
				if (StringUtil::Equals(childNode->GetName(), "Variables") == true)
				{
					const XML::Node* const variablesNode = childNode;
					for (const XML::Node* variableNode = variablesNode->GetFirstChildNode(); variableNode != nullptr; variableNode = variableNode->GetNextSiblingNode())
					{
						SharedPtr<Variable> variable = MakeShared<Variable>();
						ParseVariable(variableNode, variable);
						_variables.PushBack(variable);
					}
				}
				else
				{
					Action action;
					ParseAction(childNode, action);
					_actions.PushBack(action);
				}
			}
			return true;
		}

		uint32 ActionChart::GetActionIndex(const StringA& actionName) const
		{
			const uint32 actionCount = _actions.Size();
			for (uint32 i = 0; i < actionCount; ++i)
			{
				if (_actions[i]._name == actionName)
				{
					return i;
				}
			}
			return kInvalidIndexUint32;
		}

		const Action& ActionChart::GetCurrentAction() const
		{
			return _actions[_currentActionIndex];
		}

		SharedPtr<Variable> ActionChart::GetVariable(const StringA& variableName) const
		{
			for (const SharedPtr<Variable>& variable : _variables)
			{
				if (variable->_name == variableName.CString())
				{
					return variable;
				}
			}
			return MakeShared<Variable>();
		}

		bool ActionChart::SetBoolVariable(const StringA& variableName, bool value)
		{
			SharedPtr<Variable> variable = GetVariable(variableName);
			if (variable->_value->GetType() != Value::Type::Bool)
			{
				return false;
			}

			BoolValue& variableValue = static_cast<BoolValue&>(*variable->_value);
			variableValue._value = value;
			return true;
		}

		bool ActionChart::ParseVariable(const XML::Node* varaibleNode, SharedPtr<Variable>& outVariable)
		{
			if (varaibleNode == nullptr)
			{
				return false;
			}

			const char* const variableName = varaibleNode->FindAttribute("Name")->GetValue();
			if (StringUtil::Equals(varaibleNode->GetName(), "BoolVariable") == true)
			{
				outVariable->_name = variableName;
				outVariable->_value.Assign(MINT_NEW(BoolValue));
			}
			else if (StringUtil::Equals(varaibleNode->GetName(), "RealVariable") == true)
			{
				outVariable->_name = variableName;
				outVariable->_value.Assign(MINT_NEW(RealValue));
			}
			return true;
		}

		bool ActionChart::ParseAction(const XML::Node* actionNode, Action& outAction)
		{
			if (actionNode == nullptr)
			{
				return false;
			}

			for (const XML::Attribute* attribute = actionNode->GetFirstAttribute(); attribute != nullptr; attribute = attribute->GetNextSiblingAttribute())
			{
				if (StringUtil::Equals(attribute->GetName(), "Name"))
				{
					outAction._name = attribute->GetValue();
				}
			}

			for (const XML::Node* actionChildNode = actionNode->GetFirstChildNode(); actionChildNode != nullptr; actionChildNode = actionChildNode->GetNextSiblingNode())
			{
				if (StringUtil::Equals(actionChildNode->GetName(), "Animation"))
				{
					const XML::Node* const animationNode = actionChildNode;

					StringA animationName;
					for (const XML::Attribute* attribute = animationNode->GetFirstAttribute(); attribute != nullptr; attribute = attribute->GetNextSiblingAttribute())
					{
						if (StringUtil::Equals(attribute->GetName(), "Name"))
						{
							animationName = attribute->GetValue();
						}
					}
					outAction._animationName = animationName;
				}
				else if (StringUtil::Equals(actionChildNode->GetName(), "Transition"))
				{
					const XML::Node* const transitionNode = actionChildNode;

					Action::Transition transition;
					for (const XML::Attribute* attribute = transitionNode->GetFirstAttribute(); attribute != nullptr; attribute = attribute->GetNextSiblingAttribute())
					{
						if (StringUtil::Equals(attribute->GetName(), "ActionName"))
						{
							transition._actionName = attribute->GetValue();
						}
					}

					for (const XML::Node* conditionNode = transitionNode->GetFirstChildNode(); conditionNode != nullptr; conditionNode = conditionNode->GetNextSiblingNode())
					{
						Action::Condition condition;
						if (ParseCondition(conditionNode, condition) == false)
						{
							return false;
						}
						transition._conditions.PushBack(condition);
					}

					outAction._transitions.PushBack(transition);
				}
			}
			return true;
		}

		bool ActionChart::ParseCondition(const XML::Node* conditionNode, Action::Condition& outCondition)
		{
			if (conditionNode == nullptr)
			{
				return false;
			}

			for (const XML::Attribute* attribute = conditionNode->GetFirstAttribute(); attribute != nullptr; attribute = attribute->GetNextSiblingAttribute())
			{
				if (StringUtil::Equals(attribute->GetName(), "Variable"))
				{
					outCondition._variable = GetVariable(attribute->GetValue());
				}
				else if (StringUtil::Equals(attribute->GetName(), "Operator"))
				{
					outCondition._operator = Action::Operator::Equal;

					if (StringUtil::Equals(attribute->GetValue(), "Equal") == true)
					{
						outCondition._operator = Action::Operator::Equal;
					}
					else if (StringUtil::Equals(attribute->GetValue(), "NotEqual") == true)
					{
						outCondition._operator = Action::Operator::NotEqual;
					}
					else if (StringUtil::Equals(attribute->GetValue(), "Less") == true)
					{
						outCondition._operator = Action::Operator::Less;
					}
					else if (StringUtil::Equals(attribute->GetValue(), "LessEqual") == true)
					{
						outCondition._operator = Action::Operator::LessEqual;
					}
					else if (StringUtil::Equals(attribute->GetValue(), "Greater") == true)
					{
						outCondition._operator = Action::Operator::Greater;
					}
					else if (StringUtil::Equals(attribute->GetValue(), "GreaterEqual") == true)
					{
						outCondition._operator = Action::Operator::GreaterEqual;
					}
				}
				else if (StringUtil::Equals(attribute->GetName(), "Value"))
				{
					if (outCondition._variable->_value->GetType() == Value::Type::Bool)
					{
						outCondition._value = MakeShared<BoolValue>();
						BoolValue& value = static_cast<BoolValue&>(*outCondition._value);
						value._value = (StringUtil::Equals(attribute->GetValue(), "true") ? true : false);
					}
					else if (outCondition._variable->_value->GetType() == Value::Type::Real)
					{
						outCondition._value = MakeShared<RealValue>();
						RealValue& value = static_cast<RealValue&>(*outCondition._value);
						value._value = StringUtil::StringToDouble(StringReferenceA(attribute->GetValue()));
					}
				}
			}
			return true;
		}
#pragma endregion

#pragma region GameBase2D
		GameBase2D::GameBase2D(const StringA& title, const Int2& windowSize)
			: _window{ MINT_NEW(Window) }
			, _imageLoader{ MINT_NEW(Rendering::ImageLoader) }
			, _characterFloorOffsetFromBottom(0.0f)
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

			_objectPool.Assign(MINT_NEW(ObjectPool));

			InitializeMainCharacterObject();
			InitializeMainCameraOject();
		}

		GameBase2D::~GameBase2D()
		{
			__noop;
		}

		bool GameBase2D::IsRunning()
		{
			return _window->IsRunning();
		}

		void GameBase2D::Update(float deltaTime)
		{
			MINT_ASSERT(_characterActionChart._actions.IsEmpty() == false, "Character Action must not be empty!");

			_mainCharacterObject->GetObjectTransform()._scale._x = _character._scale._x;
			_mainCharacterObject->GetObjectTransform()._scale._y = _character._scale._y;
			_mainCharacterObject->GetObjectTransform()._translation._x = _character._position._x;
			_mainCharacterObject->GetObjectTransform()._translation._y = _character._position._y;

			for (const Action::Transition& transition : _characterActionChart.GetCurrentAction()._transitions)
			{
				const uint32 conditionCount = transition._conditions.Size();
				bool meetsAllConditions = true;
				for (uint32 i = 0; i < conditionCount; ++i)
				{
					meetsAllConditions &= transition._conditions[i].Evaluate();
				}

				if (meetsAllConditions)
				{
					const uint32 nextActionIndex = _characterActionChart.GetActionIndex(transition._actionName);
					_characterActionChart._currentActionIndex = nextActionIndex;
				}
			}

			const StringA& currentAnimationName = _characterActionChart.GetCurrentAction()._animationName;
			_characterAnimationSet.SetAnimation(currentAnimationName);
			_characterAnimationSet.Update(deltaTime);

			_physicsWorld.Step(deltaTime);

			_character._position = _physicsWorld.GetBody(_character._bodyID)._transform2D._translation;

			const Float2 windowSize{ _graphicDevice->GetWindowSize() };
			Float3& cameraPosition = _mainCameraObject->GetObjectTransform()._translation;
			cameraPosition = _mainCharacterObject->GetObjectTransform()._translation;
			cameraPosition._x = Max(cameraPosition._x, windowSize._x * 0.5f);
			cameraPosition._y = Max(cameraPosition._y, windowSize._y * 0.5f);
		}

		void GameBase2D::BeginRendering()
		{
			_graphicDevice->BeginRendering();

			_graphicDevice->SetSolidCullNoneRasterizer();

			Render();
		}

		void GameBase2D::DrawTextToScreen(const StringA& text, const Int2& position, const ByteColor& color)
		{
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

		void GameBase2D::InitializeMainCharacterObject()
		{
			_mainCharacterObject = _objectPool->CreateObject();

			{
				Collision2DComponent* collision2DComponent = _objectPool->CreateCollision2DComponent();
				const float radius = 32.0f;
				collision2DComponent->SetCollisionShape2D(MakeShared<Physics::CircleCollisionShape2D>(Physics::CircleCollisionShape2D(Float2(0.0f, radius), 32.0f)));
				_mainCharacterObject->AttachComponent(collision2DComponent);

				Physics::Body2DCreationDesc bodyCreationDesc;
				bodyCreationDesc._collisionShape2D = collision2DComponent->GetCollisionShape2D();
				bodyCreationDesc._bodyMotionType = Physics::BodyMotionType::Dynamic;
				_character._bodyID = _physicsWorld.CreateBody(bodyCreationDesc);
			}
		}

		void GameBase2D::InitializeMainCameraOject()
		{
			_mainCameraObject = _objectPool->CreateCameraObject();
			Float2 windowSize{ _graphicDevice->GetWindowSize() };
			_mainCameraObject->SetOrthographic2DCamera(windowSize);
		}

		void GameBase2D::Render()
		{
			if (_window->IsResized())
			{
				_objectPool->UpdateScreenSize(Float2(_window->GetSize()));
			}

			_graphicDevice->SetViewProjectionMatrix(_mainCameraObject->GetViewMatrix(), _mainCameraObject->GetProjectionMatrix());

			_tileMap.Draw(*_mapRenderer);
			Rendering::ShapeRendererContext& shapeRendererContext = _graphicDevice->GetShapeRendererContext();
			_mapRenderer->Render();

			//_tileMap.DrawCollisions(shapeRendererContext);
			//shapeRendererContext.Render();

			const Rendering::SpriteAnimation& characterCurrentAnimation = _characterAnimationSet.GetCurrentAnimation();

			const Float2 scaledCharacterSize = _characterSize * _character._scale;
			const float scaledFloorOffset = _characterFloorOffsetFromBottom * _character._scale._y;
			const Float2 characterDrawPosition = _mainCharacterObject->GetObjectTransform()._translation.XY() + Float2(0.0f, scaledCharacterSize._y * 0.5f - scaledFloorOffset);
			_characterRenderer->DrawImage(characterDrawPosition, scaledCharacterSize, characterCurrentAnimation.GetCurrentFrameUV0(), characterCurrentAnimation.GetCurrentFrameUV1());
			_characterRenderer->Render();

			//const Collision2DComponent* const mainCharacterCollision2DComponent = static_cast<Collision2DComponent*>(_mainCharacterObject->GetComponent(ObjectComponentType::Collision2DComponent));
			//if (mainCharacterCollision2DComponent != nullptr)
			//{
			//	mainCharacterCollision2DComponent->GetCollisionShape2D()->DebugDrawShape(shapeRendererContext, ByteColor(0, 0, 255), _mainCharacterObject->GetObjectTransform()._translation.XY());
			//	shapeRendererContext.Render();
			//}

			_physicsWorld.RenderDebug(shapeRendererContext);

			_graphicDevice->SetViewProjectionMatrix(Float4x4::kIdentity, _graphicDevice->GetScreenSpace2DProjectionMatrix());
		}

		void GameBase2D::LoadTileMap(const StringA& tileMapeFileName)
		{
			_tileMap.Load(tileMapeFileName);

			const TileSet& tileSet = _tileMap.GetTileSet();
			_tileSetImage = LoadImageFile(tileSet.GetImageFileName());
			SetTileMapImage(_tileSetImage);

			const Vector<uint32>& tiles = _tileMap.GetTiles();
			const uint32 mapWidth = _tileMap.GetWidth();
			const uint32 mapHeight = _tileMap.GetHeight();
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
					Physics::Body2DCreationDesc tileBodyCreationDesc;
					tileBodyCreationDesc._collisionShape2D = tileSet.GetTileCollisionShapes()[tileID];
					const Float2 position = _tileMap.ComputeTilePosition(x, y);
					tileBodyCreationDesc._bodyMotionType = Physics::BodyMotionType::Static;
					tileBodyCreationDesc._transform2D._translation = position;
					Physics::BodyID bodyID = _physicsWorld.CreateBody(tileBodyCreationDesc);
				}
			}
		}

		Image GameBase2D::LoadImageFile(const StringA& imageFileName)
		{
			Rendering::ByteColorImage image;
			_imageLoader->LoadImage_(imageFileName, image);
			Rendering::GraphicResourcePool& resourcePool = _graphicDevice->GetResourcePool();
			return Image(resourcePool.AddTexture2D(image));
		}

		void GameBase2D::SetCharacterImage(const Image& image, const Int2& characterSize, uint32 floorOffsetFromBottom)
		{
			Rendering::GraphicResourcePool& resourcePool = _graphicDevice->GetResourcePool();
			resourcePool.GetResource(image._graphicObjectID).BindToShader(Rendering::GraphicShaderType::PixelShader, _characterTextureSlot);

			_characterSize = Float2(characterSize);
			_characterFloorOffsetFromBottom = static_cast<float>(floorOffsetFromBottom);
		}

		void GameBase2D::SetCharacterAnimationSet(const Rendering::SpriteAnimationSet& spriteAnimationSet)
		{
			_characterAnimationSet = spriteAnimationSet;
		}

		bool GameBase2D::SetCharacterActionChart(const StringA& fileName)
		{
			return _characterActionChart.Load(fileName);
		}

		void GameBase2D::SetCharacterCollisionRadius(float radius)
		{
			Collision2DComponent* const mainCharacterCollision2DComponent = static_cast<Collision2DComponent*>(_mainCharacterObject->GetComponent(ObjectComponentType::Collision2DComponent));
			mainCharacterCollision2DComponent->SetCollisionShape2D(MakeShared<Physics::CircleCollisionShape2D>(Physics::CircleCollisionShape2D(Float2(0.0f, _characterFloorOffsetFromBottom + radius * 0.5f), radius)));
		}

		const Rendering::SpriteAnimationSet& GameBase2D::GetCharacterAnimationSet() const
		{
			return _characterAnimationSet;
		}

		ActionChart& GameBase2D::GetCharacterActionChart()
		{
			return _characterActionChart;
		}

		void GameBase2D::SetCharacterScale(const Float2& scale)
		{
			_character._scale = scale;
		}

		const Float2& GameBase2D::GetCharacterScale() const
		{
			return _character._scale;
		}

		void GameBase2D::SetCharacterVelocity(const Float2& velocity)
		{
			_physicsWorld.AccessBody(_character._bodyID)._linearVelocity = velocity;
		}

		const Float2& GameBase2D::GetCharacterVelocity() const
		{
			return _physicsWorld.GetBody(_character._bodyID)._linearVelocity;
		}

		const Float2& GameBase2D::GetCharacterPosition() const
		{
			return _character._position;
		}

		void GameBase2D::TeleportCharacterTo(const Float2& position)
		{
			_physicsWorld.AccessBody(_character._bodyID)._transform2D._translation = position;
			_character._position = position;
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
#pragma endregion
	}
}
