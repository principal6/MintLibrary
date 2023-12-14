#include <MintGUI/Include/GUISystem.h>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/StringReference.hpp>
#include <MintContainer/Include/Algorithm.hpp>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintPlatform/Include/InputContext.h>
#include <MintPhysics/Include/Intersection.hpp>


namespace mint
{
	namespace GUI
	{
#pragma region GUISystem::GUIControlManager
		GUISystem::GUIControlManager::GUIControlManager()
			: _nextControlRawID{ 0 }
		{
			__noop;
		}

		GUIControlTemplateID GUISystem::GUIControlManager::RegisterTemplate(const StringU8& controlTemplateName, GUIControlTemplate&& controlTemplate)
		{
			const int32 index = BinarySearch(_controlTemplates, controlTemplateName, GUIControlTemplate::NameEvaluator());
			if (index >= 0)
			{
				MINT_ASSERT(false, "ControlTemplate named (%s) is already registered!!!", controlTemplateName.CString());
				return GUIControlTemplateID();
			}

			const GUIControlTemplateID::RawType nextRawID = (_controlTemplates.IsEmpty() ? 0 : _controlTemplates.Back()->_controlTemplateID.Value() + 1);
			GUIControlTemplateID controlTemplateID;
			controlTemplateID.Assign(nextRawID);;
			controlTemplate._controlTemplateID = controlTemplateID;
			controlTemplate._controlTemplateName = controlTemplateName;
			_controlTemplates.PushBack(MakeShared(std::move(controlTemplate)));
			return controlTemplateID;
		}

		GUIControlTemplate& GUISystem::GUIControlManager::AccessTemplate(const GUIControlTemplateID& controlTemplateID)
		{
			const int32 index = BinarySearch(_controlTemplates, controlTemplateID, GUIControlTemplate::IDEvaluator());
			if (index < 0)
			{
				static GUIControlTemplate invalid;
				return invalid;
			}
			return *_controlTemplates[index];
		}

		GUIControlID GUISystem::GUIControlManager::AddControl(const GUIControlTemplateID& controlTemplateID)
		{
			const int32 index = BinarySearch(_controlTemplates, controlTemplateID, GUIControlTemplate::IDEvaluator());
			MINT_ASSERT(index >= 0, "This assertion must never fail!");

			SharedPtr<GUIControl> control = MakeShared<GUIControl>();
			control->_controlID.Assign(_nextControlRawID);
			_controlInstances.PushBack(control);

			const SharedPtr<GUIControlTemplate>& controlTemplate = _controlTemplates[index];
			control->_collisionShape = controlTemplate->_collisionShape;
			control->_components = controlTemplate->_components;
			return control->_controlID;
		}

		void GUISystem::GUIControlManager::RemoveControl(const GUIControlID& controlID)
		{
			const int32 controlInstanceFindResult = BinarySearch(_controlInstances, controlID, GUIControl::IDEvaluator());
			if (controlInstanceFindResult < 0)
			{
				MINT_ASSERT(false, "Control with ID(%s) is not added.", controlID.Value());
				return;
			}

			_controlInstances.Erase(controlInstanceFindResult);
		}

		void GUISystem::GUIControlManager::UpdateControls(const GUIControlUpdateContext& controlUpdateContext)
		{
			_hoveredControlID.Invalidate();
			_pressedControlID.Invalidate();

			InputContext& inputContext = InputContext::GetInstance();
			const Float2& mousePosition = inputContext.GetMousePosition();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			for (SharedPtr<GUIControl>& control : _controlInstances)
			{
				UpdateControl(controlUpdateContext, *control);
			}
		}

		void GUISystem::GUIControlManager::UpdateControl(const GUIControlUpdateContext& controlUpdateContext, GUIControl& control)
		{
			SharedPtrViewer<Physics::ConvexCollisionShape2D> controlCollisionShape = control._collisionShape;
			controlCollisionShape->_center = control._position;

			InputContext& inputContext = InputContext::GetInstance();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			const Float2& mousePosition = inputContext.GetMousePosition();
			const bool intersects = Physics::Intersect2D_GJK(*controlCollisionShape, Physics::PointCollisionShape2D(mousePosition));
			if (intersects)
			{
				_hoveredControlID = control._controlID;

				if (leftMouseButtonState == MouseButtonState::Down || leftMouseButtonState == MouseButtonState::DoubleClicked)
				{
					const bool intersects1 = Physics::Intersect2D_GJK(*controlCollisionShape, Physics::PointCollisionShape2D(controlUpdateContext._mouseLeftButtonPressedPosition));
					if (intersects1)
					{
						_pressedControlID = control._controlID;

						if (_hoveredControlID == _pressedControlID)
						{
							_hoveredControlID.Invalidate();
						}
					}
				}
			}
		}

		GUIControl& GUISystem::GUIControlManager::AccessControl(const GUIControlID& controlID)
		{
			static GUIControl invalid;
			const int32 index = BinarySearch(_controlInstances, controlID, GUIControl::IDEvaluator());
			return (index < 0 ? invalid : *_controlInstances[index]);
		}

		Vector<SharedPtr<GUIControl>>& GUISystem::GUIControlManager::AccessControlInstances()
		{
			return _controlInstances;
		}
#pragma endregion


#pragma region GUISystem
		GUISystem::GUISystem(Rendering::GraphicDevice& graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _isUpdated{ false }
		{
			__noop;
		}

		GUISystem::~GUISystem()
		{
			__noop;
		}

		GUIControlTemplateID GUISystem::RegisterTemplate(const StringU8& name, GUIControlTemplate&& controlTemplate)
		{
			return _controlManager.RegisterTemplate(name, std::move(controlTemplate));
		}

		GUIControlTemplate& GUISystem::AccessTemplate(const GUIControlTemplateID& controlTemplateID)
		{
			return _controlManager.AccessTemplate(controlTemplateID);
		}

		GUIControlID GUISystem::AddControl(const GUIControlTemplateID& controlTemplateID)
		{
			return _controlManager.AddControl(controlTemplateID);
		}

		void GUISystem::RemoveControl(const GUIControlID& controlID)
		{
			_controlManager.RemoveControl(controlID);
		}

		GUIControl& GUISystem::AccessControl(const GUIControlID& controlID)
		{
			return _controlManager.AccessControl(controlID);
		}

		void GUISystem::Update()
		{
			MINT_ASSERT(_isUpdated == false, "Don't call Update() multiple times in a frame!");

			InputContext& inputContext = InputContext::GetInstance();
			const Float2& mousePosition = inputContext.GetMousePosition();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			if (leftMouseButtonState == MouseButtonState::Pressed)
			{
				_controlUpdateContext._mouseLeftButtonPressedPosition = mousePosition;
			}

			_controlManager.UpdateControls(_controlUpdateContext);

			_isUpdated = true;
		}

		void GUISystem::Render()
		{
			MINT_ASSERT(_isUpdated == true, "You must call Update() every frame!");

			const GUIControlID& hoveredControlID = _controlManager.GetHoveredControlID();
			const GUIControlID& pressedControlID = _controlManager.GetPressedControlID();

			Vector<SharedPtr<GUIControl>>& controlInstances = _controlManager.AccessControlInstances();
			for (const SharedPtr<GUIControl>& control : controlInstances)
			{
				GUIControlInteractionState controlInteractionState = GUIControlInteractionState::None;
				if (control->_controlID == pressedControlID)
				{
					controlInteractionState = GUIControlInteractionState::Pressed;
				}
				else if (control->_controlID == hoveredControlID)
				{
					controlInteractionState = GUIControlInteractionState::Hovered;
				}

				control->Render(_graphicDevice, controlInteractionState);
			}

			_isUpdated = false;
		}
#pragma endregion
	}
}
