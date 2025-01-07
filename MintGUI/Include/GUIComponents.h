#pragma once


#ifndef _MINT_GUI_GUI_COMPONENTS_H_
#define _MINT_GUI_GUI_COMPONENTS_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintContainer/Include/ID.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
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
	}
}

namespace mint
{
	namespace GUI
	{
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

		template<typename ComponentType>
		class GUIComponentPool
		{
		public:
			GUIComponentPool();
			~GUIComponentPool();

		public:
			static GUIComponentPool& GetInstance();
			void AddComponentTo(const GUIEntity& entity, ComponentType&& component);
			bool HasComponent(const GUIEntity& entity) const;
			ComponentType* GetComponent(const GUIEntity& entity);

		private:
			Vector<GUIEntity> _entities;
			Vector<ComponentType> _components;
		};
	}
}


#endif // !_MINT_GUI_GUI_COMPONENTS_H_
