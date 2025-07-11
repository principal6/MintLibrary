#pragma once


#ifndef _MINT_GUI_GUI_COMPONENTS_H_
#define _MINT_GUI_GUI_COMPONENTS_H_


#include <MintRendering/Include/RenderingBaseCommon.h>
#include <MintContainer/Include/ID.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintContainer/Include/ContiguousHashMap.h>
#include <MintMath/Include/Transform.h>
#include <MintReflection/Include/Reflection.h>


namespace mint
{
	namespace Physics2D
	{
		class CollisionShape;
	}

	namespace GUI
	{
		class GUIControl;
		class GUIControlTemplate;
		class GUISystem;
	}
}

namespace mint
{
	template <>
	struct Hasher<GUI::GUIControl> final
	{
		uint64 operator()(const GUI::GUIControl& value) const noexcept;
	};

	template <>
	struct Hasher<GUI::GUIControlTemplate> final
	{
		uint64 operator()(const GUI::GUIControlTemplate& value) const noexcept;
	};

	namespace GUI
	{
		class GUIControl : public ID32 { friend GUISystem; };
		class GUIControlTemplate : public ID16 { friend GUISystem; };

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

		struct GUICollisionShapeComponent
		{
			SharedPtr<Physics2D::CollisionShape> _collisionShape;
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

		// type-erasure for GUIComponentPool
		class IGUIComponentPool abstract
		{
		public:
			IGUIComponentPool() { __noop; }
			virtual ~IGUIComponentPool() { __noop; }

		public:
			virtual bool HasComponent(const GUIControl& control) const = 0;
			virtual void CopyComponent(const GUIControl& sourceControl, const GUIControl& targetControl) = 0;
			virtual void CopyComponentToTemplate(const GUIControl& sourceControl, const GUIControlTemplate& targetControlTemplate) = 0;
			virtual void CopyComponentFromTemplate(const GUIControlTemplate& sourceControlTemplate, const GUIControl& targetControl) = 0;
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
			void AddComponentTo(const GUIControl& control, ComponentType&& component);
			void AddComponentToTemplate(const GUIControlTemplate& controlTemplate, ComponentType&& component);
			virtual bool HasComponent(const GUIControl& control) const override final;
			virtual void CopyComponent(const GUIControl& sourceControl, const GUIControl& targetControl) override final;
			virtual void CopyComponentToTemplate(const GUIControl& sourceControl, const GUIControlTemplate& targetControlTemplate) override final;
			virtual void CopyComponentFromTemplate(const GUIControlTemplate& sourceControlTemplate, const GUIControl& targetControl) override final;
			ComponentType* GetComponent(const GUIControl& control);
			ComponentType* GetTemplateComponent(const GUIControlTemplate& controlTemplate);

		private:
			ContiguousHashMap<GUIControl, ComponentType> _controlComponents;
			ContiguousHashMap<GUIControlTemplate, ComponentType> _controlTemplateComponents;
		};

		class GUIComponentPoolRegistry
		{
			GUIComponentPoolRegistry();
			GUIComponentPoolRegistry(const GUIComponentPoolRegistry& rhs) = delete;
			GUIComponentPoolRegistry(GUIComponentPoolRegistry&& rhs) noexcept = delete;

		public:
			~GUIComponentPoolRegistry();
			static GUIComponentPoolRegistry& GetInstance();

		public:
			void RegisterComponentPool(IGUIComponentPool& componentPool);
			const Vector<IGUIComponentPool*>& GetComponentPools() const;

		private:
			Vector<IGUIComponentPool*> _componentPools;
#if defined(MINT_DEBUG)
		private:
			static GUIComponentPoolRegistry* _sInstance;
#endif // defined(MINT_DEBUG)
		};
	}
}


#endif // !_MINT_GUI_GUI_COMPONENTS_H_
