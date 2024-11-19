#include <MintGUI/Include/GUISystem.h>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/StringReference.hpp>
#include <MintContainer/Include/Algorithm.hpp>
#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/ShapeRenderer.h>
#include <MintPlatform/Include/InputContext.h>
#include <MintPhysics/Include/CollisionShape.h>
#include <MintPhysics/Include/Intersection.hpp>
#include <MintGUI/Include/GUIObject.hpp>


namespace mint
{
	namespace GUI
	{
#pragma region GUISystem::GUIObjectManager
		GUISystem::GUIObjectManager::GUIObjectManager()
			: _nextObjectRawID{ 0 }
		{
			__noop;
		}

		GUIObjectTemplateID GUISystem::GUIObjectManager::RegisterTemplate(const StringU8& objectTemplateName, GUIObjectTemplate&& objectTemplate)
		{
			const int32 index = BinarySearch(_objectTemplates, objectTemplateName, GUIObjectTemplate::NameEvaluator());
			if (index >= 0)
			{
				MINT_ASSERT(false, "ObjectTemplate named (%s) is already registered!!!", objectTemplateName.CString());
				return GUIObjectTemplateID();
			}

			const GUIObjectTemplateID::RawType nextRawID = (_objectTemplates.IsEmpty() ? 0 : _objectTemplates.Back()->_templateID.Value() + 1);
			GUIObjectTemplateID objectTemplateID;
			objectTemplateID.Assign(nextRawID);;
			objectTemplate._templateID = objectTemplateID;
			objectTemplate._templateName = objectTemplateName;
			_objectTemplates.PushBack(MakeShared(std::move(objectTemplate)));
			return objectTemplateID;
		}

		GUIObjectTemplate& GUISystem::GUIObjectManager::AccessTemplate(const GUIObjectTemplateID& objectTemplateID)
		{
			const int32 index = BinarySearch(_objectTemplates, objectTemplateID, GUIObjectTemplate::IDEvaluator());
			if (index < 0)
			{
				static GUIObjectTemplate invalid;
				return invalid;
			}
			return *_objectTemplates[index];
		}

		GUIObjectID GUISystem::GUIObjectManager::AddObject(const GUIObjectTemplateID& objectTemplateID)
		{
			const int32 index = BinarySearch(_objectTemplates, objectTemplateID, GUIObjectTemplate::IDEvaluator());
			MINT_ASSERT(index >= 0, "This assertion must never fail!");

			SharedPtr<GUIObject> guiObject = MakeShared<GUIObject>();
			guiObject->_objectID.Assign(_nextObjectRawID);
			++_nextObjectRawID;
			_objectInstances.PushBack(guiObject);

			const SharedPtr<GUIObjectTemplate>& objectTemplate = _objectTemplates[index];
			guiObject->_collisionShape = objectTemplate->_collisionShape;
			guiObject->_components = objectTemplate->_components;
			return guiObject->_objectID;
		}

		void GUISystem::GUIObjectManager::RemoveObject(const GUIObjectID& objectID)
		{
			const int32 guiObjectInstanceFindResult = BinarySearch(_objectInstances, objectID, GUIObject::IDEvaluator());
			if (guiObjectInstanceFindResult < 0)
			{
				MINT_ASSERT(false, "Object with ID(%s) is not added.", objectID.Value());
				return;
			}

			_objectInstances.Erase(guiObjectInstanceFindResult);
		}

		void GUISystem::GUIObjectManager::UpdateObjects(const GUIObjectUpdateContext& objectUpdateContext)
		{
			_hoveredObjectID.Invalidate();

			for (SharedPtr<GUIObject>& guiObject : _objectInstances)
			{
				UpdateObject(objectUpdateContext, *guiObject);
			}

			UpdatePressedObject(objectUpdateContext);

			// MouseButtonState 가 Released 나 Up 이더라도 이번 프레임 Update 끝날 때까지 PressedObject 를 유지한다.
			InputContext& inputContext = InputContext::GetInstance();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			if (leftMouseButtonState == MouseButtonState::Released || leftMouseButtonState == MouseButtonState::Up)
			{
				_pressedObjectID.Invalidate();
			}
		}

		void GUISystem::GUIObjectManager::UpdateObject(const GUIObjectUpdateContext& objectUpdateContext, GUIObject& guiObject)
		{
			InputContext& inputContext = InputContext::GetInstance();
			const Float2& mousePosition = inputContext.GetMousePosition();
			const bool intersects = Physics::Intersect2D_GJK(*guiObject._collisionShape, Physics::PointCollisionShape2D(mousePosition - guiObject._position));
			if (intersects == false)
			{
				return;
			}

			_hoveredObjectID = guiObject._objectID;

			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			if (leftMouseButtonState == MouseButtonState::Pressed || leftMouseButtonState == MouseButtonState::Down || leftMouseButtonState == MouseButtonState::DoubleClicked)
			{
				const bool intersects1 = Physics::Intersect2D_GJK(*guiObject._collisionShape, Physics::PointCollisionShape2D(objectUpdateContext._mouseLeftButtonPressedPosition - guiObject._position));
				if (intersects1)
				{
					_pressedObjectID = guiObject._objectID;

					if (_hoveredObjectID == _pressedObjectID)
					{
						_hoveredObjectID.Invalidate();
					}
				}
			}
		}

		void GUISystem::GUIObjectManager::UpdatePressedObject(const GUIObjectUpdateContext& objectUpdateContext)
		{
			if (_pressedObjectID.IsValid() == false)
			{
				return;
			}

			InputContext& inputContext = InputContext::GetInstance();
			const Float2& mousePosition = inputContext.GetMousePosition();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			GUIObject& pressedObject = AccessObject(_pressedObjectID);
			SharedPtr<GUIDraggableComponent> guiDraggableComponent = pressedObject.GetComponent<GUIDraggableComponent>();
			if (guiDraggableComponent.IsValid() == true)
			{
				if (leftMouseButtonState == MouseButtonState::Pressed)
				{
					guiDraggableComponent->SetLocalPressedPosition(objectUpdateContext._mouseLeftButtonPressedPosition - pressedObject._position);
				}
				else
				{
					pressedObject._position = mousePosition - guiDraggableComponent->GetLocalPressedPosition();
				}
			}
		}

		GUIObject& GUISystem::GUIObjectManager::AccessObject(const GUIObjectID& objectID)
		{
			static GUIObject invalid;
			const int32 index = BinarySearch(_objectInstances, objectID, GUIObject::IDEvaluator());
			return (index < 0 ? invalid : *_objectInstances[index]);
		}

		Vector<SharedPtr<GUIObject>>& GUISystem::GUIObjectManager::AccessObjectInstances()
		{
			return _objectInstances;
		}
#pragma endregion


#pragma region GUISystem
		GUISystem::GUISystem(Rendering::GraphicsDevice& graphicsDevice)
			: _graphicsDevice{ graphicsDevice }
			, _isUpdated{ false }
		{
			__noop;
		}

		GUISystem::~GUISystem()
		{
			__noop;
		}

		GUIObjectTemplateID GUISystem::RegisterTemplate(const StringU8& name, GUIObjectTemplate&& objectTemplate)
		{
			return _objectManager.RegisterTemplate(name, std::move(objectTemplate));
		}

		GUIObjectTemplate& GUISystem::AccessTemplate(const GUIObjectTemplateID& objectTemplateID)
		{
			return _objectManager.AccessTemplate(objectTemplateID);
		}

		GUIObjectID GUISystem::AddObject(const GUIObjectTemplateID& objectTemplateID)
		{
			return _objectManager.AddObject(objectTemplateID);
		}

		void GUISystem::RemoveObject(const GUIObjectID& objectID)
		{
			_objectManager.RemoveObject(objectID);
		}

		GUIObject& GUISystem::AccessObject(const GUIObjectID& objectID)
		{
			return _objectManager.AccessObject(objectID);
		}

		void GUISystem::Update()
		{
			MINT_ASSERT(_isUpdated == false, "Don't call Update() multiple times in a frame!");

			InputContext& inputContext = InputContext::GetInstance();
			const Float2& mousePosition = inputContext.GetMousePosition();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			if (leftMouseButtonState == MouseButtonState::Pressed)
			{
				_objectUpdateContext._mouseLeftButtonPressedPosition = mousePosition;
			}

			_objectManager.UpdateObjects(_objectUpdateContext);

			_isUpdated = true;
		}

		void GUISystem::Render()
		{
			MINT_ASSERT(_isUpdated == true, "You must call Update() every frame!");

			const GUIObjectID& hoveredObjectID = _objectManager.GetHoveredObjectID();
			const GUIObjectID& pressedObjectID = _objectManager.GetPressedObjectID();

			_graphicsDevice.SetSolidCullNoneRasterizer();
			_graphicsDevice.SetViewProjectionMatrix(Float4x4::kIdentity, _graphicsDevice.GetScreenSpace2DProjectionMatrix());

			Rendering::ShapeRenderer& shapeRenderer = _graphicsDevice.GetShapeRenderer();
			Vector<SharedPtr<GUIObject>>& guiObjectInstances = _objectManager.AccessObjectInstances();
			for (const SharedPtr<GUIObject>& guiObject : guiObjectInstances)
			{
				GUIObjectInteractionState objectInteractionState = GUIObjectInteractionState::None;
				if (guiObject->_objectID == pressedObjectID)
				{
					objectInteractionState = GUIObjectInteractionState::Pressed;
				}
				else if (guiObject->_objectID == hoveredObjectID)
				{
					objectInteractionState = GUIObjectInteractionState::Hovered;
				}

				guiObject->Render(shapeRenderer, objectInteractionState);
			}

			shapeRenderer.Render();

			_isUpdated = false;
		}
#pragma endregion
	}
}
