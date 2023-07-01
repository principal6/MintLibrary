#include <MintRenderingBase/Include/GUI/GUISystem.h>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/StringReference.hpp>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintPlatform/Include/InputContext.h>
#include <MintPhysics/Include/Intersection.hpp>


namespace mint
{
	namespace Rendering
	{
#pragma region GUISystem::GUIControlManager
		GUISystem::GUIControlManager::GUIControlManager()
			: _nextControlRawID{ 0 }
		{
			__noop;
		}

		void GUISystem::GUIControlManager::DefineControl(const StringU8& name, GUIControlTemplate&& controlTemplate)
		{
			const KeyValuePair controlTemplateFindResult = _controlTemplateMap.Find(name);
			if (controlTemplateFindResult.IsValid() == true)
			{
				MINT_ASSERT(false, "Control with name(%s) is already defined!!!", name.CString());
				return;
			}

			_controlTemplateMap.Insert(name, MakeShared(std::move(controlTemplate)));
		}

		GUIControlID GUISystem::GUIControlManager::AddControl(const StringU8& controlName)
		{
			const KeyValuePair controlTemplateFindResult = _controlTemplateMap.Find(controlName);
			if (controlTemplateFindResult.IsValid() == false)
			{
				MINT_ASSERT(false, "Control with name(%s) is not defined!!!", controlName.CString());
				return GUIControlID();
			}

			const GUIControlID controlID = GUIControlID(_nextControlRawID);
			SharedPtr<GUIControl> control = MakeShared<GUIControl>();
			_controlInstanceMap.Insert(controlID, control);

			const SharedPtr<GUIControlTemplate>& controlTemplate = *controlTemplateFindResult._value;
			control->_controlID = controlID;
			control->_collisionShape = controlTemplate->_collisionShape;
			control->_components = controlTemplate->_components;
			return control->_controlID;
		}

		void GUISystem::GUIControlManager::RemoveControl(const GUIControlID& controlID)
		{
			const KeyValuePair controlInstanceFindResult = _controlInstanceMap.Find(controlID);
			if (controlInstanceFindResult.IsValid() == false)
			{
				MINT_ASSERT(false, "Control with ID(%s) is not added.", controlID._raw);
				return;
			}

			_controlInstanceMap.Erase(controlID);
		}

		void GUISystem::GUIControlManager::UpdateControls(const GUIControlUpdateContext& controlUpdateContext)
		{
			using namespace Platform;

			_hoveredControlID.Invalidate();
			_pressedControlID.Invalidate();

			InputContext& inputContext = InputContext::GetInstance();
			const Float2& mousePosition = inputContext.GetMousePosition();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			for (SharedPtr<GUIControl>& control : _controlInstanceMap)
			{
				UpdateControl(controlUpdateContext, *control);
			}
		}

		void GUISystem::GUIControlManager::UpdateControl(const GUIControlUpdateContext& controlUpdateContext, GUIControl& control)
		{
			SharedPtrViewer<Physics::ConvexShape2D> controlCollisionShape = control._collisionShape;
			controlCollisionShape->_center = control._position;

			using namespace Platform;

			InputContext& inputContext = InputContext::GetInstance();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			const Float2& mousePosition = inputContext.GetMousePosition();
			const bool intersects = Physics::Intersect2D_GJK(*controlCollisionShape, Physics::PointShape2D(mousePosition));
			if (intersects)
			{
				_hoveredControlID = control._controlID;

				if (leftMouseButtonState == MouseButtonState::Down || leftMouseButtonState == MouseButtonState::DoubleClicked)
				{
					const bool intersects1 = Physics::Intersect2D_GJK(*controlCollisionShape, Physics::PointShape2D(controlUpdateContext._mouseLeftButtonPressedPosition));
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
			KeyValuePair findResult = _controlInstanceMap.Find(controlID);
			return (findResult.IsValid() ? *(*findResult._value) : invalid);
		}

		HashMap<GUIControlID, SharedPtr<GUIControl>>& GUISystem::GUIControlManager::AccessControlInstanceMap()
		{
			return _controlInstanceMap;
		}
#pragma endregion


#pragma region GUISystem
		GUISystem::GUISystem(GraphicDevice& graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _isUpdated{ false }
		{
			__noop;
		}

		GUISystem::~GUISystem()
		{
			__noop;
		}

		void GUISystem::DefineControl(const StringU8& name, GUIControlTemplate&& controlTemplate)
		{
			_controlManager.DefineControl(name, std::move(controlTemplate));
		}

		GUIControlID GUISystem::AddControl(const StringU8& controlName)
		{
			return _controlManager.AddControl(controlName);
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

			using namespace Platform;
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

			HashMap<GUIControlID, SharedPtr<GUIControl>>& controlInstanceMap = _controlManager.AccessControlInstanceMap();
			for (const SharedPtr<GUIControl>& control : controlInstanceMap)
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
