#pragma once


#ifndef _MINT_RENDERING_BASE_GUI_CONTROL_H_
#define _MINT_RENDERING_BASE_GUI_CONTROL_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintContainer/Include/ID.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintRenderingBase/Include/ShapeGenerator.h>


namespace mint
{
	namespace Physics
	{
		class ConvexShape2D;
	}


	namespace Rendering
	{
		class GraphicDevice;
		class GUISystem;

		class GUIControlID : public ID64
		{
			friend GUISystem;
		public:
			MINT_INLINE uint64 ComputeHash() const { return mint::ComputeHash(Value()); }
		};

		enum class GUIControlInteractionState
		{
			None,
			Hovered,
			Pressed
		};

		class GUIControlComponent
		{
		public:
			GUIControlComponent() = default;
			virtual ~GUIControlComponent() = default;
			virtual void Render(GraphicDevice& graphicDevice, const Float2& controlPosition, const GUIControlInteractionState& controlInteractionState) const { __noop; }
			//protected:
				//GUIControlID _controlID;
		};

		class GUIControlTextComponent : public GUIControlComponent
		{
		public:
			GUIControlTextComponent() : GUIControlComponent() { __noop; }
			virtual void Render(GraphicDevice& graphicDevice, const Float2& controlPosition, const GUIControlInteractionState& controlInteractionState) const override;
		public:
			Float2 _offset;
			StringW _text;
		};

		class GUIControlShapeComponent : public GUIControlComponent
		{
		public:
			GUIControlShapeComponent() : GUIControlComponent() { __noop; }
			virtual void Render(GraphicDevice& graphicDevice, const Float2& controlPosition, const GUIControlInteractionState& controlInteractionState) const override;
		public:
			Float2 _offset;
			Shape _shapes[3];
		};

		class GUIControl
		{
			friend GUISystem;
		public:
			GUIControl() = default;
			virtual ~GUIControl() = default;
			void SetPosition(const Float2& position) { _position = position; }
			void Render(GraphicDevice& graphicDevice, const GUIControlInteractionState& controlInteractionState) const;
			uint32 GetComponentCount() const { return _components.Size(); }
			SharedPtr<GUIControlComponent> GetComponent(const uint32 index) const { return _components.At(index); }
		protected:
			GUIControlID _controlID;
			SharedPtr<Physics::ConvexShape2D> _collisionShape;
			Vector<SharedPtr<GUIControlComponent>> _components;
		protected:
			Float2 _position;
		};

		class GUIControlTemplate : public GUIControl
		{
		public:
			GUIControlTemplate() : GUIControl() { __noop; }
			Vector<SharedPtr<GUIControlComponent>>& AccessComponents();
			void SetCollisionShape(Physics::ConvexShape2D&& collisionShape);
		};
	}
}


#endif // !_MINT_RENDERING_BASE_GUI_CONTROL_H_
