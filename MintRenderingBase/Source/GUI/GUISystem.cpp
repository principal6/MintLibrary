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
#pragma region GUIControl
		struct GUISystem::GUIControlDefinition
		{
			GUIControlDefinition() = default;
			GUIControlDefinition(const GUIControlDesc& controlDesc, const Physics::ConvexShape2D& controlCollisionShape)
				: _controlDesc{ controlDesc }
				, _controlCollisionShape{ controlCollisionShape }
			{
				__noop;
			}
			GUIControlDesc _controlDesc;
			Physics::ConvexShape2D _controlCollisionShape;
		};
#pragma endregion


#pragma region GUISystem::GUIControlManager
		GUISystem::GUIControlManager::GUIControlManager()
			: _nextControlRawID{ 0 }
		{
			__noop;
		}

		void GUISystem::GUIControlManager::DefineControl(const GUIControlDesc& controlDesc)
		{
			const KeyValuePair controlDefinitionFindResult = _controlDefinitionMap.Find(controlDesc._name);
			if (controlDefinitionFindResult.IsValid() == true)
			{
				MINT_ASSERT(false, "Control with name(%s) is already defined!!!", controlDesc._name);
				return;
			}

			const uint32 index = _controlDefinitions.Size();
			_controlDefinitions.PushBack(GUIControlDefinition(controlDesc, Physics::ConvexShape2D::MakeFromRenderingShape(Float2::kZero, controlDesc._normalShape)));
			_controlDefinitionMap.Insert(controlDesc._name, index);
		}

		GUIControlID GUISystem::GUIControlManager::AddControl(const StringU8& controlName)
		{
			const KeyValuePair controlDefinitionFindResult = _controlDefinitionMap.Find(controlName);
			if (controlDefinitionFindResult.IsValid() == false)
			{
				MINT_ASSERT(false, "Control with name(%s) is not defined!!!", controlName.CString());
				return GUIControlID();
			}

			const GUIControlID controlID = GUIControlID(_nextControlRawID);
			_controlInstanceMap.Insert(controlID, GUIControl());

			GUIControl& control = AccessControl(controlID);
			control._controlID = controlID;
			control._controlDefinitionIndex = *controlDefinitionFindResult._value;
			return control._controlID;
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

			InputContext& inputContext = InputContext::GetInstance();
			const Float2& mousePosition = inputContext.GetMousePosition();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			for (GUIControl& control : _controlInstanceMap)
			{
				UpdateControl(controlUpdateContext, control);
			}
		}

		void GUISystem::GUIControlManager::UpdateControl(const GUIControlUpdateContext& controlUpdateContext, GUIControl& control)
		{
			GUIControlDefinition& controlDefinition = _controlDefinitions[control._controlDefinitionIndex];
			Physics::ConvexShape2D& controlCollisionShape = controlDefinition._controlCollisionShape;
			controlCollisionShape._center = control._position;

			using namespace Platform;

			InputContext& inputContext = InputContext::GetInstance();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
			const Float2& mousePosition = inputContext.GetMousePosition();
			const bool intersects = Physics::Intersect2D_GJK(controlCollisionShape, Physics::PointShape2D(mousePosition));
			if (intersects)
			{
				control._controlState = GUIControlState::Hovered;

				if (leftMouseButtonState == MouseButtonState::Down || leftMouseButtonState == MouseButtonState::DoubleClicked)
				{
					const bool intersects1 = Physics::Intersect2D_GJK(controlCollisionShape, Physics::PointShape2D(controlUpdateContext._mouseLeftButtonPressedPosition));
					if (intersects1)
					{
						control._controlState = GUIControlState::Pressed;
					}
				}
			}
			else
			{
				control._controlState = GUIControlState::Normal;
			}
		}

		GUIControl& GUISystem::GUIControlManager::AccessControl(const GUIControlID& controlID)
		{
			static GUIControl invalid;
			KeyValuePair findResult = _controlInstanceMap.Find(controlID);
			return (findResult.IsValid() ? *findResult._value : invalid);
		}

		HashMap<GUIControlID, GUIControl>& GUISystem::GUIControlManager::AccessControlInstanceMap()
		{
			return _controlInstanceMap;
		}

		const GUISystem::GUIControlDefinition& GUISystem::GUIControlManager::GetControlDefinition(const uint32 controlDefinitionIndex)
		{
			return _controlDefinitions[controlDefinitionIndex];
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

		void GUISystem::DefineControl(const GUIControlDesc& controlDesc)
		{
			_controlManager.DefineControl(controlDesc);
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

			ShapeRendererContext& shapeRendererContext = _graphicDevice.GetShapeRendererContext();
			HashMap<GUIControlID, GUIControl>& controlInstanceMap = _controlManager.AccessControlInstanceMap();
			for (const GUIControl& control : controlInstanceMap)
			{
				const Float4 controlPosition{ control._position };
				shapeRendererContext.SetPosition(controlPosition);

				const GUIControlDefinition& controlDefinition = _controlManager.GetControlDefinition(control._controlDefinitionIndex);
				const GUIControlDesc& controlDesc = controlDefinition._controlDesc;
				switch (control._controlState)
				{
				case GUIControlState::Normal:
					shapeRendererContext.AddShape(controlDesc._normalShape);
					break;
				case GUIControlState::Hovered:
					shapeRendererContext.AddShape(controlDesc._hoveredShape);
					break;
				case GUIControlState::Pressed:
					shapeRendererContext.AddShape(controlDesc._pressedShape);
					break;
				default:
					MINT_NEVER;
					break;
				}

				if (control._text.IsEmpty() == false)
				{
					// TODO
					FontRenderingOption fontRenderingOption;
					fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
					fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
					shapeRendererContext.DrawDynamicText(control._text.CString(), controlPosition, fontRenderingOption);
				}
			}

			_isUpdated = false;
		}
#pragma endregion
	}
}
