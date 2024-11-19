#pragma once


#ifndef _MINT_GUI_GUI_OBJECT_H_
#define _MINT_GUI_GUI_OBJECT_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintContainer/Include/ID.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintReflection/Include/Reflection.h>


namespace mint
{
	namespace Rendering
	{
		class ShapeRenderer;
	}

	namespace Physics
	{
		class ConvexCollisionShape2D;
	}

	namespace GUI
	{
		class GUISystem;
	}
}

namespace mint
{
	namespace GUI
	{
		class GUIObjectID : public ID64
		{
			friend GUISystem;
		};

		class GUIObjectTemplateID : public ID16
		{
			friend GUISystem;
		};

		enum class GUIObjectInteractionState : uint8
		{
			None,
			Hovered,
			Pressed,
			Focused,
			COUNT
		};
		
		class GUIComponent
		{
			REFLECTION_CLASS(GUIComponent);
		public:
			GUIComponent() { InitializeReflection(); }
			virtual ~GUIComponent() = default;
			virtual SharedPtr<GUIComponent> Clone() const = 0;
			virtual void Render(Rendering::ShapeRenderer& shapeRenderer, const Float2& objectPosition, const GUIObjectInteractionState& objectInteractionState) const { __noop; }
		//protected:
			//GUIObjectID _objectID;
			REFLECTION_BIND_BEGIN;
			REFLECTION_BIND_END;
		};

		class GUITextComponent : public GUIComponent
		{
			REFLECTION_CLASS(GUITextComponent);

		public:
			GUITextComponent() : GUIComponent() { InitializeReflection(); }
			void SetText(const StringReferenceW& text);
			void SetOffset(const Float2& offset);
			virtual SharedPtr<GUIComponent> Clone() const override final;
			virtual void Render(Rendering::ShapeRenderer& shapeRenderer, const Float2& objectPosition, const GUIObjectInteractionState& objectInteractionState) const override;

		private:
			StringW _text;
			Float2 _offset;
			REFLECTION_BIND_BEGIN;
			REFLECTION_BIND_END;
		};

		class GUIShapeComponent : public GUIComponent
		{
			REFLECTION_CLASS(GUIShapeComponent);
		public:
			GUIShapeComponent() : GUIComponent() { InitializeReflection(); }
			void SetShape(const GUIObjectInteractionState& objectInteractionState, const Rendering::Shape& shape);
			virtual SharedPtr<GUIComponent> Clone() const override final;
			virtual void Render(Rendering::ShapeRenderer& shapeRenderer, const Float2& objectPosition, const GUIObjectInteractionState& objectInteractionState) const override;

		private:
			Rendering::Shape _shapes[static_cast<uint32>(GUIObjectInteractionState::COUNT)];
			REFLECTION_BIND_BEGIN;
			REFLECTION_BIND_END;
		};
		
		class GUIDraggableComponent : public GUIComponent
		{
			REFLECTION_CLASS(GUIDraggableComponent);
		public:
			GUIDraggableComponent() : GUIComponent() { InitializeReflection(); }
			MINT_INLINE void SetLocalPressedPosition(const Float2& localPressedPosition) { _localPressedPosition = localPressedPosition; }
			virtual SharedPtr<GUIComponent> Clone() const override final;
			MINT_INLINE const Float2& GetLocalPressedPosition() const { return _localPressedPosition; }
		
		private:
			Float2 _localPressedPosition;
			REFLECTION_BIND_BEGIN;
			REFLECTION_BIND_END;
		};

		class GUIObject
		{
			friend GUISystem;
		public:
			struct IDEvaluator 
			{
				const GUIObjectID& operator()(const SharedPtr<GUIObject>& rhs) const noexcept { return rhs->_objectID; } 
				const GUIObjectID& operator()(const GUIObject& rhs) const noexcept { return rhs._objectID; } 
			};
		public:
			GUIObject() = default;
			virtual ~GUIObject();
			void SetPosition(const Float2& position) { _position = position; }
			void AddComponent(SharedPtr<GUIComponent> guiComponent);
			void Render(Rendering::ShapeRenderer& shapeRenderer, const GUIObjectInteractionState& objectInteractionState) const;
		public:
			uint32 GetComponentCount() const { return _components.Size(); }
			template<typename T>
			SharedPtr<T> GetComponent() const;
			SharedPtr<GUIComponent> GetComponent(const uint32 index) const { return _components.At(index); }
		protected:
			GUIObjectID _objectID;
			SharedPtr<Physics::ConvexCollisionShape2D> _collisionShape;
			Vector<SharedPtr<GUIComponent>> _components;
		protected:
			Float2 _position;
		};

		class GUIObjectTemplate : public GUIObject
		{
			friend GUISystem;
		public:
			struct IDEvaluator
			{
				const GUIObjectTemplateID& operator()(const GUIObjectTemplate& rhs) const noexcept { return rhs._templateID; }
				const GUIObjectTemplateID& operator()(const SharedPtr<GUIObjectTemplate>& rhs) const noexcept { return rhs->_templateID; }
			};
			struct NameEvaluator
			{
				StringReferenceU8 operator()(const GUIObjectTemplate& rhs) const noexcept { return rhs._templateName; }
				StringReferenceU8 operator()(const SharedPtr<GUIObjectTemplate>& rhs) const noexcept { return rhs->_templateName; }
			};
		public:
			GUIObjectTemplate() : GUIObject() { __noop; }
			void AddComponent(SharedPtr<GUIComponent> guiComponent);
			void SetCollisionShape(Physics::ConvexCollisionShape2D&& collisionShape);
		private:
			StringU8 _templateName;
			GUIObjectTemplateID _templateID;
		};
	}
}


#endif // !_MINT_GUI_GUI_OBJECT_H_
