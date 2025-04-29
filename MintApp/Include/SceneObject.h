#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_H_
#define _MINT_APP_SCENE_OBJECT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/ID.h>
#include <MintContainer/Include/Hash.h>
#include <MintContainer/Include/ContiguousHashMap.h>
#include <MintContainer/Include/SerialAndIndex.h>

#include <MintMath/Include/Transform.h>


namespace mint
{
	class InputContext;
}

namespace mint
{
	class SceneObject : public SerialAndIndex<uint64, kInvalidIndexUint64, 40>
	{
	};

	template <>
	struct Hasher<SceneObject> final
	{
		uint64 operator()(const SceneObject& value) const noexcept;
	};

	// type-erasure for SceneObjectComponentPool
	class ISceneObjectComponentPool abstract
	{
	public:
		ISceneObjectComponentPool() { __noop; }
		virtual ~ISceneObjectComponentPool() { __noop; }

	public:
		virtual void RemoveComponentFrom(const SceneObject& entity) = 0;
		virtual bool HasComponent(const SceneObject& entity) const = 0;
		virtual void CopyComponent(const SceneObject& sourceEntity, const SceneObject& targetEntity) = 0;
	};

	template<typename ComponentType>
	class SceneObjectComponentPool final : public ISceneObjectComponentPool
	{
	public:
		SceneObjectComponentPool();
		virtual ~SceneObjectComponentPool();

	public:
		static SceneObjectComponentPool& GetInstance();

	public:
		void AddComponentTo(const SceneObject& sceneObject, const ComponentType& component);
		void AddComponentTo(const SceneObject& sceneObject, ComponentType&& component);
		virtual void RemoveComponentFrom(const SceneObject& sceneObject) override final;
		virtual bool HasComponent(const SceneObject& sceneObject) const override final;
		virtual void CopyComponent(const SceneObject& sourceEntity, const SceneObject& targetEntity) override final;
		ComponentType* GetComponent(const SceneObject& sceneObject);
		ContiguousHashMap<SceneObject, ComponentType>& GetComponentMap();

	private:
		ContiguousHashMap<SceneObject, ComponentType> _componentMap;
	};

	class SceneObjectComponentPoolRegistry
	{
		SceneObjectComponentPoolRegistry();
		SceneObjectComponentPoolRegistry(const SceneObjectComponentPoolRegistry& rhs) = delete;
		SceneObjectComponentPoolRegistry(SceneObjectComponentPoolRegistry&& rhs) noexcept = delete;

	public:
		~SceneObjectComponentPoolRegistry();
		static SceneObjectComponentPoolRegistry& GetInstance();

	public:
		void RegisterComponentPool(ISceneObjectComponentPool& componentPool);
		const Vector<ISceneObjectComponentPool*>& GetComponentPools() const;

	private:
		Vector<ISceneObjectComponentPool*> _componentPools;
#if defined(MINT_DEBUG)
	private:
		static SceneObjectComponentPoolRegistry* _sInstance;
#endif // defined(MINT_DEBUG)
	};

	struct TransformComponent
	{
		Transform _transform;
	};

	enum class CameraMoveDirection : uint8
	{
		Forward,
		Backward,
		Leftward,
		Rightward,
		Upward,
		Downward
	};
	enum class CameraMoveSpeed : uint8
	{
		x0_125,
		x0_25,
		x0_5,
		x1_0,
		x2_0,
		x4_0,
		x8_0,
		x16_0,
		COUNT
	};
	struct CameraComponent
	{
		Float4x4 _projectionMatrix;
		mutable Float3 _forwardDirection;
		bool _usePerspectiveProjection = true;
		bool _isRightHanded = true;
		float _fov = Math::ToRadian(60.0f);
		union
		{
			struct
			{
				float _nearZ;
				float _farZ;
			};
			Float2 _screenSize{ 1.0f, 100.0f };
		};
		float _screenRatio = 1.0f;
		float _pitch = 0.0f;
		float _yaw = 0.0f;
		float _roll = 0.0f;
		CameraMoveSpeed _moveSpeed = CameraMoveSpeed::x8_0;
		bool _isBoostMode = false;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_H_
