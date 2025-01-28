#pragma once


#ifndef _MINT_GUI_GUI_COMPONENTS_H_
#define _MINT_GUI_GUI_COMPONENTS_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintContainer/Include/ID.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintContainer/Include/HashMap.h>
#include <MintMath/Include/Transform.h>
#include <MintReflection/Include/Reflection.h>


namespace mint
{
	namespace Physics
	{
		class CollisionShape2D;
	}

	namespace GUI
	{
		class GUIEntity;
		class GUIEntityTemplate;
		class GUISystem;
	}
}

namespace mint
{
	template <>
	struct Hasher<GUI::GUIEntity> final
	{
		uint64 operator()(const GUI::GUIEntity& value) const noexcept;
	};
	
	template <>
	struct Hasher<GUI::GUIEntityTemplate> final
	{
		uint64 operator()(const GUI::GUIEntityTemplate& value) const noexcept;
	};

	namespace GUI
	{
		class GUIEntity : public ID32 { friend GUISystem; };
		class GUIEntityTemplate : public ID16 { friend GUISystem; };

		enum class GUIInteractionState : uint8
		{
			None,
			Hovered,
			Pressed,
			Focused,
			COUNT
		};
		
		struct GUIInteractionStateComponent
		{
			GUIInteractionState _interactionState;
		};

		struct GUICollisionShape2DComponent
		{
			SharedPtr<Physics::CollisionShape2D> _collisionShape2D;
		};
		
		struct GUITransform2DComponent
		{
			Transform2D _transform2D;
		};

		struct GUITextComponent
		{
			StringW _text;
			Float2 _offset;
		};

		struct GUIShapeComponent
		{
			Rendering::Shape _shapes[static_cast<uint32>(GUIInteractionState::COUNT)];
		};
		
		struct GUIDraggableComponent
		{
			Float2 _localPressedPosition;
		};

		// Don't forget to RegisterComponentPool to GUISystem for each type of ComponentPool!
		// type-erasure for GUIComponentPool
		class IGUIComponentPool abstract
		{
		public:
			IGUIComponentPool() { __noop; }
			virtual ~IGUIComponentPool() { __noop; }

		public:
			virtual bool HasComponent(const GUIEntity& entity) const = 0;
			virtual void CopyComponent(const GUIEntity& sourceEntity, const GUIEntity& targetEntity) = 0;
			virtual void CopyComponentToTemplate(const GUIEntity& sourceEntity, const GUIEntityTemplate& targetEntityTemplate) = 0;
			virtual void CopyComponentFromTemplate(const GUIEntityTemplate& sourceEntityTemplate, const GUIEntity& targetEntity) = 0;
		};

		template<typename ComponentType>
		class GUIComponentPool final : public IGUIComponentPool
		{
		public:
			GUIComponentPool();
			virtual ~GUIComponentPool();

		public:
			static GUIComponentPool& GetInstance();

		public:
			void AddComponentTo(const GUIEntity& entity, ComponentType&& component);
			void AddComponentToTemplate(const GUIEntityTemplate& targetEntityTemplate, ComponentType&& component);
			virtual bool HasComponent(const GUIEntity& entity) const override final;
			virtual void CopyComponent(const GUIEntity& sourceEntity, const GUIEntity& targetEntity) override final;
			virtual void CopyComponentToTemplate(const GUIEntity& sourceEntity, const GUIEntityTemplate& targetEntityTemplate) override final;
			virtual void CopyComponentFromTemplate(const GUIEntityTemplate& sourceEntityTemplate, const GUIEntity& targetEntity) override final;
			ComponentType* GetComponent(const GUIEntity& entity);
			ComponentType* GetTemplateComponent(const GUIEntityTemplate& entityTemplate);

		private:
			// sparse set
			HashMap<GUIEntity, uint32> _entityToComponentMap;
			Vector<ComponentType> _components;

		private:
			HashMap<GUIEntityTemplate, uint32> _entityTemplateToTemplateComponentMap;
			Vector<ComponentType> _templateComponents;
		};
	}
}


#endif // !_MINT_GUI_GUI_COMPONENTS_H_
