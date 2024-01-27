#pragma once


#ifndef _MINT_GAME_GAME_BASE_H_
#define _MINT_GAME_GAME_BASE_H_


#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintContainer/Include/String.h>
#include <MintRenderingBase/Include/GraphicObject.h>
#include <MintRenderingBase/Include/SpriteAnimation.h>
#include <MintPlatform/Include/XML.h>
#include <MintPhysics/Include/Intersection.h>
#include <MintPhysics/Include/PhysicsWorld.h>


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
		class ObjectPool;
		class Object;
		class CameraObject;
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

		public:
			// TEMP
			void Draw(const Float2& centerPosition, const Float2& size);
			const Rendering::GraphicObjectID& GetGraphicObjectID() const { return _graphicObjectID; }

		private:
			Rendering::GraphicObjectID _graphicObjectID;

			// TEMP
			Rendering::ImageRenderer* _imageRenderer;
		};

		class Value abstract
		{
		public:
			enum class Type
			{
				Bool,
				Real
			};
			Value() = default;
			virtual ~Value() = default;
			virtual Type GetType() const = 0;
		};

		template<typename ValueType, Value::Type kType>
		class TypedValue : public Value
		{
		public:
			virtual Type GetType() const override { return kType; }
			ValueType _value{};
		};
		using BoolValue = TypedValue<bool, Value::Type::Bool>;
		using RealValue = TypedValue<double, Value::Type::Real>;

		class Variable
		{
		public:
			StringA _name;
			OwnPtr<Value> _value;
		};

		class Action
		{
		public:
			enum class Operator
			{
				Equal,
				NotEqual,
				Less,
				LessEqual,
				Greater,
				GreaterEqual,
				COUNT
			};

			class Condition
			{
			public:
				bool Evaluate() const;
				SharedPtr<Variable> _variable;
				Operator _operator;
				SharedPtr<Value> _value;
			};

			class Transition
			{
			public:
				Vector<Condition> _conditions;
				StringA _actionName;
			};

		public:
			StringA _name;
			StringA _animationName;
			Vector<Transition> _transitions;
		};

		class ActionChart
		{
		public:
			ActionChart();
			bool Load(const StringA& fileName);

		public:
			uint32 GetActionIndex(const StringA& actionName) const;
			const Action& GetCurrentAction() const;
			SharedPtr<Variable> GetVariable(const StringA& variableName) const;
			bool SetBoolVariable(const StringA& variableName, bool value);

		private:
			bool ParseVariable(const XML::Node* varaibleNode, SharedPtr<Variable>& outVariable);
			bool ParseAction(const XML::Node* actionNode, Action& outAction);
			bool ParseCondition(const XML::Node* conditionNode, Action::Condition& outCondition);

		public:
			Vector<SharedPtr<Variable>> _variables;
			Vector<Action> _actions;
			uint32 _currentActionIndex;
		};

		class Character2D
		{
		public:
			Float2 _position;
			float _moveSpeed = 1.0f;
			Float2 _velocity;
			Float2 _scale = Float2(1, 1);
			Physics::BodyID _bodyID;
		};

		class Effect2D
		{
		public:
			Float2 _spawnPosition = Float2::kZero;
			Float2 _size = Float2(128, 128);
			float _elapsedTime = 0.0f;
			float _lifeTime = 1.0f;
		};

		enum class CharacterControlMode
		{
			Default,
			Custom
		};
		enum class GameCameraMode
		{
			FixedToWorld,
			FollowCharacter
		};

		class GameBase2D
		{
		public:
			GameBase2D(const StringA& title, const Int2& windowSize);
			virtual ~GameBase2D();

		public:
			bool IsRunning();
			void Update(float deltaTime);

		public:
			void BeginRendering();
			void DrawMap();
			void DrawCharacter();
			void DrawEffects();
			void DrawCircle(const Float2& position, float radius, const ByteColor& color);
			void DrawGrid();
			void DrawTextToScreen(const StringA& text, const Int2& position, const ByteColor& color);
			void EndRendering();

		public:
			void LoadTileMap(const StringA& tileMapeFileName);
			Image LoadImageFile(const StringA& imageFileName);

		public:
			void SetCharacterImage(const Image& image, const Int2& characterSize, int32 floorOffset);
			void SetCharacterAnimationSet(const Rendering::SpriteAnimationSet& spriteAnimationSet);
			bool SetCharacterActionChart(const StringA& fileName);
			void SetCharacterCollision(const Float2& centerOffset, float radius);
			void SetCharacterMoveSpeed(float moveSpeed);
			const Rendering::SpriteAnimationSet& GetCharacterAnimationSet() const;
			ActionChart& GetCharacterActionChart();
			void SetCharacterScale(const Float2& scale);
			const Float2& GetCharacterScale() const;
			void SetCharacterVelocity(const Float2& velocity);
			const Float2& GetCharacterVelocity() const;
			const Float2& GetCharacterPosition() const;
			void TeleportCharacterTo(const Float2& position);
			void TeleportCharacterBy(const Float2& delta);

		public:
			void SetTileMapImage(const Image& image);
			void SetBackgroundMusic(const StringReferenceA& audioFileName);

		public:
			void SpawnEffect(const Float2& position);

		public:
			Rendering::GraphicDevice& GetGraphicDevice();
			const Physics::World& GetPhysicsWorld() const;
			float GetPhysicsStepDeltaTime() const { return kPhysicsStepDeltaTime; }

		public:
			void SetDebugMode(bool isDebugMode);
			bool IsRecordingHistory() const { return _isRecordingHistory; }
			bool BeginHistoryRecording();
			void EndHistoryRecording();
			bool IsPlayingHistory() const { return _isPlayingHistory; }
			void BeginHistoryPlaying();
			void EndHistoryPlaying();

		protected:
			void InitializeMainCharacterObject();
			void InitializeMainCameraOject();

		protected:
			OwnPtr<Window> _window;
			OwnPtr<Rendering::GraphicDevice> _graphicDevice;
			OwnPtr<Rendering::ImageLoader> _imageLoader;

		protected:
			TileMap _tileMap;
			Image _tileSetImage;

		protected:
			Rendering::SpriteAnimationSet _characterAnimationSet;
			ActionChart _characterActionChart;
			Float2 _characterSize;
			float _characterFloorOffset;
			Character2D _character;

		protected:
			static constexpr const uint32 kCharacterTextureSlot = 1;
			static constexpr const uint32 kTileMapTextureSlot = 2;
			static constexpr const uint32 kObjectTextureSlot = 3;
			static constexpr const uint32 kEffectTextureSlot = 4;
			OwnPtr<Rendering::ImageRenderer> _characterRenderer;
			OwnPtr<Rendering::ImageRenderer> _mapRenderer;
			OwnPtr<Rendering::ImageRenderer> _objectRenderer;
			OwnPtr<Rendering::ImageRenderer> _effectRenderer;

		protected:
			OwnPtr<AudioSystem> _audioSystem;
			OwnPtr<AudioObject> _backgroundMusic;

		protected:
			OwnPtr<ObjectPool> _objectPool;

			// TODO: make this object!
			OwnPtr<Rendering::SpriteAnimation> _effectAnimation;
			Image _effectImage;
			Vector<Effect2D> _effects;

		protected:
			CharacterControlMode _characterControlMode;
			SharedPtr<Object> _mainCharacterObject;
		
		protected:
			SharedPtr<CameraObject> _mainCameraObject;
			GameCameraMode _gameCameraMode;

		protected:
			static constexpr const float kPhysicsStepDeltaTime = 1.0f / 64.0f;
			float _deltaTimeRemainder;
			Physics::World _physicsWorld;

		protected:
			bool _isDebugMode;
			bool _isRecordingHistory;
			bool _isPlayingHistory;
		};
	}
}


#endif // !_MINT_GAME_GAME_BASE_H_
