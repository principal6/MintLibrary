#include <MintRenderingBase/Include/GUI/GUIContext.h>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintReflection/Include/Reflection.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/GUI/ControlData.hpp>
#include <MintRenderingBase/Include/GUI/GUIInteractionModule.hpp>

#include <MintPlatform/Include/WindowsWindow.h>
#include <MintPlatform/Include/InputContext.h>


namespace mint
{
	using Platform::InputContext;
	using Platform::MouseButton;
	using Platform::CursorType;


	namespace Rendering
	{
		namespace GUI
		{
			GUIContext::GUIContext(GraphicDevice& graphicDevice)
				: _graphicDevice{ graphicDevice }
				, _rendererContext{ graphicDevice }
				, _fontSize{ 0.0f }
				, _currentCursor{ CursorType::Arrow }
			{
				__noop;
			}

			GUIContext::~GUIContext()
			{
				__noop;
			}

			void GUIContext::Initialize()
			{
				//_caretBlinkIntervalMs = _graphicDevice.GetWindow().GetCaretBlinkIntervalMs();

				const FontData& fontData = _graphicDevice.GetShapeRendererContext().GetFontData();
				_fontSize = static_cast<float>(fontData._fontSize);
				if (_rendererContext.InitializeFontData(fontData) == false)
				{
					MINT_ASSERT(false, "ShapeRendererContext::initializeFont() 에 실패했습니다!");
				}

				_rendererContext.InitializeShaders();
				_rendererContext.SetUseMultipleViewports();

				const Float2& windowSize = Float2(_graphicDevice.GetWindowSize());
				UpdateScreenSize(windowSize);

				// ROOT
				_rootControlID = ControlID(kUint64Max);
				ControlData& rootControlData = AccessControlData(_rootControlID, ControlType::COUNT);
				rootControlData._absolutePosition = Float2::kZero;
				//rootControlData._size = windowSize;
				_controlStack.PushBack(_rootControlID);
			}

			void GUIContext::ProcessEvent() noexcept
			{
				const InputContext& inputContext = InputContext::GetInstance();
				if (inputContext.IsMouseButtonPressed())
				{
					_mousePressedPosition = inputContext.GetMousePosition();
				}
				if (inputContext.IsMouseButtonReleased() == false && inputContext.IsMouseButtonUp(MouseButton::Left) == true)
				{
					_mousePressedPosition = Float2::kNan;
				}

				_currentCursor = CursorType::Arrow;
			}

			void GUIContext::UpdateScreenSize(const Float2& newScreenSize)
			{
				// TODO
			}

			void GUIContext::Render() noexcept
			{
				MINT_ASSERT(_controlStack.Size() <= 1, "begin- 호출 횟수가 end- 호출 횟수보다 많습니다!!!");

				if (_focusingModule.IsInteracting())
				{
					// FocusedWindow 가 다른 Window (priority 0) 에 비해 위에 그려지도록 priority 1 로 설정.
					_rendererContext.AccessLowLevelRenderer().SetOrdinalRenderCommandGroupPriority(_focusingModule.GetControlID().GetRawID(), 1);
				}

				if (_debugSwitch._raw != 0)
				{
					for (const ControlID& controlID : _controlIDsOfCurrentFrame)
					{
						ControlData& controlData = AccessControlData(controlID);
						DebugRender_Control(controlData);
					}
				}
				_controlIDsOfCurrentFrame.Clear();

				_graphicDevice.AccessWindow().SetCursorType(_currentCursor);

				_rendererContext.Render();
				_rendererContext.Flush();
			}

			void GUIContext::NextControlSameLine()
			{
				_nextControlDesc._sameLine = true;
			}

			void GUIContext::NextControlPosition(const Float2& position)
			{
				_nextControlDesc._position = position;
			}

			void GUIContext::NextControlSize(const Float2& contentSize)
			{
				_nextControlDesc._size = contentSize;
			}

			void GUIContext::NextControlMargin(const Rect& margin)
			{
				_nextControlDesc._margin = margin;
			}

			void GUIContext::NextControlPadding(const Rect& padding)
			{
				_nextControlDesc._padding = padding;
			}

			void GUIContext::MakeLabel(const FileLine& fileLine, const LabelDesc& labelDesc)
			{
				static constexpr ControlType kControlType = ControlType::Label;
				const ControlID parentControlID = AccessStackParentControlData().GetID();
				const ControlID controlID = ControlData::GenerateID(fileLine, kControlType, labelDesc._text, parentControlID);
				_controlIDsOfCurrentFrame.PushBack(controlID);

				ControlData& controlData = AccessControlData(controlID, kControlType);
				controlData._parentID = parentControlID;
				controlData._text = labelDesc._text;
				UpdateControlData(controlData);
				MakeLabel_Render(labelDesc, controlData);
			}

			bool GUIContext::MakeButton(const FileLine& fileLine, const ButtonDesc& buttonDesc)
			{
				static constexpr ControlType kControlType = ControlType::Button;
				const ControlID parentControlID = AccessStackParentControlData().GetID();
				const ControlID controlID = ControlData::GenerateID(fileLine, kControlType, buttonDesc._text, parentControlID);
				_controlIDsOfCurrentFrame.PushBack(controlID);

				ControlData& controlData = AccessControlData(controlID, kControlType);
				controlData._parentID = parentControlID;
				controlData._text = buttonDesc._text;
				UpdateControlData(controlData);
				MakeButton_Render(buttonDesc, controlData);
				return controlData._mouseInteractionState == ControlData::MouseInteractionState::Clicked;
			}

			bool GUIContext::BeginWindow(const FileLine& fileLine, const WindowDesc& windowDesc)
			{
				static constexpr ControlType kControlType = ControlType::Window;
				const ControlID parentControlID = AccessStackParentControlData().GetID();
				const ControlID controlID = ControlData::GenerateID(fileLine, kControlType, windowDesc._title, parentControlID);
				_controlIDsOfCurrentFrame.PushBack(controlID);

				if (_isInBeginWindow)
				{
					MINT_NEVER;
				}
				_isInBeginWindow = true;
				_rendererContext.AccessLowLevelRenderer().BeginOrdinalRenderCommands(controlID.GetRawID());

				ControlData& controlData = AccessControlData(controlID, kControlType);
				controlData._parentID = parentControlID;
				controlData._text = windowDesc._title;
				ControlData& parentControlData = AccessControlData(parentControlID);
				const bool isNewlyCreated = (controlData.GetAccessCount() == 0);
				if (isNewlyCreated)
				{
					ControlData::PerTypeData::WindowData& windowData = controlData._perTypeData._windowData;
					windowData._titleBarHeight = _fontSize + _theme._titleBarPadding.Vert();
					controlData._resizingMask.SetAllTrue();
					controlData._resizableMinSize = Float2(windowData._titleBarHeight * 2.0f);
					controlData._generalTraits._isFocusable = true;
					controlData._generalTraits._isDraggable = true;

					NextControlPosition(windowDesc._initialPosition);
					NextControlSize(windowDesc._initialSize);
				}
				else
				{
					NextControlPosition(controlData.ComputeRelativePosition(parentControlData));
					NextControlSize(controlData._size);
				}
				// No margin for window controls
				NextControlMargin(Rect());

				UpdateControlData(controlData);
				BeginWindow_Render(controlData);
				_controlStack.PushBack(controlID);

				{
					ButtonDesc closeButtonDesc;
					closeButtonDesc._text = L"@CloseButton";
					closeButtonDesc._isRoundButton = true;
					closeButtonDesc._customizeColor = true;
					closeButtonDesc._customizedColorSet = _theme._closeButtonColorSet;
					const float titleBarHeight = controlData._zones._titleBarZone.Height();
					const float radius = _theme._systemButtonRadius;
					NextControlPosition(Float2(controlData._size._x - _theme._titleBarPadding.Right() - radius * 2.0f, titleBarHeight * 0.5f - radius));
					NextControlSize(Float2(radius * 2.0f));
					if (MakeButton(fileLine, closeButtonDesc))
					{

					}
				}

				return true;
			}

			void GUIContext::EndWindow()
			{
				_isInBeginWindow = false;
				_rendererContext.AccessLowLevelRenderer().EndOrdinalRenderCommands();

				if (_controlStack.IsEmpty())
				{
					MINT_ASSERT(false, "end- 호출 횟수가 begin- 호출 횟수보다 많습니다!");
					return;
				}
				MINT_ASSERT(AccessControlData(_controlStack.Back()).GetType() == ControlType::Window, "Control Stack 이 비정상적입니다!");

				_controlStack.PopBack();
			}

			void GUIContext::MakeLabel_Render(const LabelDesc& labelDesc, const ControlData& controlData)
			{
				const Color& backgroundColor = labelDesc.GetBackgroundColor(_theme);
				if (backgroundColor.A() > 0.0f)
				{
					_rendererContext.SetColor(backgroundColor);
					_rendererContext.SetPosition(ComputeShapePosition(controlData.GetID()));
					_rendererContext.DrawRectangle(controlData._size, 0.0f, 0.0f);
				}
				FontRenderingOption fontRenderingOption;
				fontRenderingOption._directionHorz = labelDesc._directionHorz;
				fontRenderingOption._directionVert = labelDesc._directionVert;
				DrawText_(controlData.GetID(), labelDesc.GetTextColor(_theme), fontRenderingOption);
			}

			void GUIContext::MakeButton_Render(const ButtonDesc& buttonDesc, const ControlData& controlData)
			{
				const HoverPressColorSet& hoverPressColorSet = (buttonDesc._customizeColor) ? buttonDesc._customizedColorSet : _theme._hoverPressColorSet;
				_rendererContext.SetColor(hoverPressColorSet.ChooseColorByInteractionState(controlData._mouseInteractionState));
				_rendererContext.SetPosition(ComputeShapePosition(controlData.GetID()));
				if (buttonDesc._isRoundButton)
				{
					const float radius = controlData._size._x * 0.5f;
					_rendererContext.DrawCircle(radius);
				}
				else
				{
					_rendererContext.DrawRoundedRectangle(controlData._size, ComputeRoundness(controlData.GetID()), 0.0f, 0.0f);

					FontRenderingOption fontRenderingOption;
					fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
					fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
					DrawText_(controlData.GetID(), _theme._textColor, fontRenderingOption);
				}
			}

			void GUIContext::BeginWindow_Render(const ControlData& controlData)
			{
				_rendererContext.SetPosition(ComputeShapePosition(controlData.GetID()));

				const bool isFocused = _focusingModule.IsInteractingWith(controlData.GetID());
				const float titleBarHeight = controlData._zones._titleBarZone.Height();
				StackVector<ShapeRendererContext::Split, 3> splits;
				splits.PushBack(ShapeRendererContext::Split(titleBarHeight / controlData._size._y, (isFocused ? _theme._windowTitleBarFocusedColor : _theme._windowTitleBarUnfocusedColor)));
				splits.PushBack(ShapeRendererContext::Split(1.0f, _theme._windowBackgroundColor));
				_rendererContext.DrawRoundedRectangleVertSplit(controlData._size, _theme._roundnessInPixel, splits, 0.0f);

				FontRenderingOption titleBarFontRenderingOption;
				titleBarFontRenderingOption._directionHorz = TextRenderDirectionHorz::Rightward;
				titleBarFontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
				const ControlData& parentControlData = AccessControlData(controlData._parentID);
				const Float2 titleBarTextPosition = controlData.ComputeRelativePosition(parentControlData) + Float2(_theme._titleBarPadding.Left(), 0.0f);
				const Float2 titleBarSize = Float2(controlData._size._x, titleBarHeight);
				DrawText_(titleBarTextPosition, titleBarSize, controlData._text, _theme._textColor, titleBarFontRenderingOption);
			}

			ControlData& GUIContext::AccessControlData(const ControlID& controlID) const
			{
				static ControlData invalid;
				auto found = _controlDataMap.Find(controlID);
				if (found.IsValid())
				{
					uint64& accessCount = const_cast<uint64&>(found._value->GetAccessCount());
					++accessCount;
					return *found._value;
				}
				return invalid;
			}

			ControlData& GUIContext::AccessControlData(const ControlID& controlID, const ControlType controlType)
			{
				ControlData& controlData = AccessControlData(controlID);
				if (controlData.GetID() != controlID)
				{
					_controlDataMap.Insert(controlID, ControlData(controlID, controlType));
					return *_controlDataMap.Find(controlID)._value;
				}
				return controlData;
			}

			ControlID GUIContext::FindAncestorWindowControl(const ControlID& controlID) const
			{
				ControlData& controlData = AccessControlData(controlID);
				if (controlData.GetType() == ControlType::Window)
				{
					return controlID;
				}

				ControlID foundControlID = controlData._parentID;
				while (foundControlID.IsValid())
				{
					ControlData& currentControlData = AccessControlData(foundControlID);
					if (currentControlData.GetType() == ControlType::Window)
					{
						return foundControlID;
					}
					foundControlID = currentControlData._parentID;
				}
				return _rootControlID;
			}

			ControlData& GUIContext::AccessStackParentControlData()
			{
				return AccessControlData(_controlStack.Back());
			}

			void GUIContext::UpdateControlData(ControlData& controlData)
			{
				UpdateControlData_ProcessResizing(controlData);
				UpdateControlData_ProcessDragging(controlData);

				UpdateControlData_RenderingData(controlData);
				UpdateControlData_Interaction(controlData);
				UpdateControlData_ResetNextControlDesc();
			}

			void GUIContext::UpdateControlData_ProcessResizing(const ControlData& controlData)
			{
				if (_resizingModule.IsInteractingWith(controlData.GetID()) == false)
				{
					return;
				}

				const InputContext& inputContext = InputContext::GetInstance();
				const ControlData::ResizingFlags& resizingFlags = _resizingModule.GetResizingFlags();
				SelectResizingCursorType(resizingFlags);

				Float2 displacementSize = inputContext.GetMousePosition() - _resizingModule.GetMousePressedPosition();
				if (resizingFlags._left == false && resizingFlags._right == false)
				{
					displacementSize._x = 0.0f;
				}
				if (resizingFlags._top == false && resizingFlags._bottom == false)
				{
					displacementSize._y = 0.0f;
				}

				if (resizingFlags._left == true || resizingFlags._top == true)
				{
					Float2 displacementPosition = displacementSize;
					if (resizingFlags._left)
					{
						displacementSize._x *= -1.0f;
					}
					else
					{
						displacementPosition._x = 0.0f;
					}

					if (resizingFlags._top)
					{
						displacementSize._y *= -1.0f;
					}
					else
					{
						displacementPosition._y *= 0.0f;
					}

					const Float2 maxPosition = _resizingModule.GetInitialControlPosition() + _resizingModule.GetInitialControlSize() - controlData._resizableMinSize;
					NextControlPosition(Float2::Min(_resizingModule.GetInitialControlPosition() + displacementPosition, maxPosition));
				}

				NextControlSize(Float2::Max(_resizingModule.GetInitialControlSize() + displacementSize, controlData._resizableMinSize));
			}

			void GUIContext::UpdateControlData_ProcessDragging(const ControlData& controlData)
			{
				if (_draggingModule.IsInteractingWith(controlData.GetID()) == false)
				{
					return;
				}

				const InputContext& inputContext = InputContext::GetInstance();
				const Float2 displacement = inputContext.GetMousePosition() - _draggingModule.GetMousePressedPosition();
				const Float2 absolutePosition = _draggingModule.GetInitialControlPosition() + displacement;
				const Float2 relativePosition = absolutePosition - AccessControlData(controlData._parentID)._absolutePosition;
				NextControlPosition(relativePosition);
			}

			void GUIContext::UpdateControlData_RenderingData(ControlData& controlData)
			{
				const Float2 controlRelativePosition = _nextControlDesc._position;
				const Float2 controlSize = _nextControlDesc._size;

				controlData._nextChildNextLinePosition = controlData._zones._contentZone.Position();

				// Position
				const bool isAutoPositioned = controlRelativePosition.IsNAN();
				ControlData& parentControlData = AccessControlData(controlData._parentID);
				const Float2& parentNextChildPosition = (_nextControlDesc._sameLine ? parentControlData._nextChildSameLinePosition : parentControlData._nextChildNextLinePosition);
				const Float2 relativePosition = (isAutoPositioned ? parentNextChildPosition : controlRelativePosition);
				controlData._absolutePosition = relativePosition;
				controlData._absolutePosition += parentControlData._absolutePosition;
				if (isAutoPositioned)
				{
					// Only auto-positioned controls need margin
					controlData._absolutePosition._x += _nextControlDesc._margin.Left();
					controlData._absolutePosition._y += _nextControlDesc._margin.Top();
				}

				// Size
				const bool isAutoSized = controlSize.IsNAN();
				if (isAutoSized)
				{
					const FontData& fontData = _rendererContext.GetFontData();
					const float textWidth = fontData.ComputeTextWidth(controlData._text, StringUtil::Length(controlData._text));
					controlData._size._x = textWidth + _nextControlDesc._padding.Horz();
					controlData._size._y = _fontSize + _nextControlDesc._padding.Vert();
				}
				else
				{
					controlData._size = controlSize;
				}

				if (isAutoPositioned)
				{
					// If its position is specified, the control does not affect its parent's _nextChildSameLinePosition nor _nextChildNextLinePosition.
					parentControlData._nextChildSameLinePosition = relativePosition + Float2(controlData._size._x + _nextControlDesc._margin.Right(), 0.0f);

					parentControlData._nextChildNextLinePosition._x = parentControlData._zones._contentZone.Position()._x;
					parentControlData._nextChildNextLinePosition._y = relativePosition._y + controlData._size._y + _nextControlDesc._margin.Bottom();
				}

				parentControlData._zones._contentZone.ExpandRightBottom(Rect(relativePosition, controlData._size));

				controlData.UpdateZones();
			}

			void GUIContext::UpdateControlData_Interaction(ControlData& controlData)
			{
				const InputContext& inputContext = InputContext::GetInstance();
				controlData._mouseInteractionState = ControlData::MouseInteractionState::None;
				const Float2& mousePosition = inputContext.GetMousePosition();
				if (_focusingModule.IsInteracting())
				{
					const ControlID ancestorWindowControlID = FindAncestorWindowControl(controlData.GetID());
					if (_focusingModule.IsInteractingWith(ancestorWindowControlID) == false)
					{
						// FocusedWindow 에 속하지 않은 ControlData 인데, FocusedWindow 영역 내에 Mouse 가 있는 경우,
						// interaction 을 진행하지 않는다!
						const ControlData& focusedControlData = AccessControlData(_focusingModule.GetControlID());
						const Rect focusedControlRect = Rect(focusedControlData._absolutePosition, focusedControlData._size);
						if (focusedControlRect.Contains(mousePosition))
						{
							return;
						}
					}
				}

				const bool isMouseLeftUp = inputContext.IsMouseButtonUp(MouseButton::Left);
				const bool isMousePositionIn = Rect(controlData._absolutePosition, controlData._size).Contains(mousePosition);
				const Float2 relativePressedMousePosition = _mousePressedPosition - controlData._absolutePosition;
				if (isMousePositionIn)
				{
					const bool isPressedMousePositionIn = Rect(Float2::kZero, controlData._size).Contains(relativePressedMousePosition);
					controlData._mouseInteractionState = isPressedMousePositionIn ? ControlData::MouseInteractionState::Pressing : ControlData::MouseInteractionState::Hovering;
					if (isPressedMousePositionIn == true && isMouseLeftUp == true)
					{
						controlData._mouseInteractionState = ControlData::MouseInteractionState::Clicked;
					}
				}

				UpdateControlData_Interaction_focusing(controlData);
				UpdateControlData_Interaction_resizing(controlData);
				UpdateControlData_Interaction_dragging(controlData);
			}

			void GUIContext::UpdateControlData_Interaction_focusing(ControlData& controlData)
			{
				if (controlData._generalTraits._isFocusable == false)
				{
					return;
				}

				// 이미 Focus 되어 있는 컨트롤
				if (_focusingModule.IsInteractingWith(controlData.GetID()) == true)
				{
					return;
				}

				if (controlData._mouseInteractionState == ControlData::MouseInteractionState::Clicked
					|| _draggingModule.IsInteractingWith(controlData.GetID()) == true
					|| _resizingModule.IsInteractingWith(controlData.GetID()) == true)
				{
					_focusingModule.End();

					InteractionMousePressModuleInput interactionMousePressModuleInput;
					interactionMousePressModuleInput._controlID = controlData.GetID();
					interactionMousePressModuleInput._controlPosition = controlData._absolutePosition;
					interactionMousePressModuleInput._mousePressedPosition = _mousePressedPosition;
					_focusingModule.Begin(interactionMousePressModuleInput);
				}
			}

			void GUIContext::UpdateControlData_Interaction_resizing(ControlData& controlData)
			{
				// Dragging 중에는 Resizing 을 하지 않는다.
				if (_draggingModule.IsInteracting())
				{
					return;
				}

				const InputContext& inputContext = InputContext::GetInstance();
				const bool isMouseLeftUp = inputContext.IsMouseButtonUp(MouseButton::Left);
				if (isMouseLeftUp)
				{
					_resizingModule.End();
				}

				if (controlData._resizingMask.IsAllFalse())
				{
					return;
				}

				Rect outerRect;
				Rect innerRect;
				ResizingModule::MakeOuterAndInenrRects(controlData, _theme._outerResizingDistances, _theme._innerResizingDistances, outerRect, innerRect);
				const Float2& mousePosition = inputContext.GetMousePosition();
				if (outerRect.Contains(mousePosition) == true && innerRect.Contains(mousePosition) == false)
				{
					// Hover
					const ControlData::ResizingFlags resizingInteraction = ResizingModule::MakeResizingFlags(mousePosition, controlData, outerRect, innerRect);
					SelectResizingCursorType(resizingInteraction);
				}

				if (inputContext.IsMouseButtonDown(MouseButton::Left))
				{
					if (outerRect.Contains(_mousePressedPosition) == true && innerRect.Contains(_mousePressedPosition) == false)
					{
						ResizingModuleInput resizingModuleInput;
						resizingModuleInput._controlID = controlData.GetID();
						resizingModuleInput._controlPosition = controlData._absolutePosition;
						resizingModuleInput._controlSize = controlData._size;
						resizingModuleInput._mousePressedPosition = _mousePressedPosition;
						resizingModuleInput._resizingInteraction = ResizingModule::MakeResizingFlags(_mousePressedPosition, controlData, outerRect, innerRect);
						_resizingModule.Begin(resizingModuleInput);
					}
				}
				else
				{
					_resizingModule.End();
				}
			}

			void GUIContext::UpdateControlData_Interaction_dragging(ControlData& controlData)
			{
				if (controlData._generalTraits._isDraggable == false)
				{
					return;
				}

				// Resizing 중에는 Dragging 을 하지 않는다.
				if (_resizingModule.IsInteracting())
				{
					return;
				}

				const InputContext& inputContext = InputContext::GetInstance();
				const bool isMouseLeftUp = inputContext.IsMouseButtonUp(MouseButton::Left);
				if (isMouseLeftUp)
				{
					_draggingModule.End();
					return;
				}

				// ParentControl 에 beginDragging 을 호출했지만 ChildControl 과도 Interaction 을 하고 있다면 ParentControl 에 endDragging 을 호출한다.
				const ControlData& parentControlData = AccessControlData(controlData._parentID);
				if (_draggingModule.IsInteractingWith(parentControlData.GetID()))
				{
					const Float2 draggingRelativePressedMousePosition = _draggingModule.ComputeRelativeMousePressedPosition() - controlData.ComputeRelativePosition(parentControlData);
					if (controlData._zones._visibleContentZone.Contains(draggingRelativePressedMousePosition))
					{
						_draggingModule.End();
					}
				}

				// TODO: Draggable Control 에 대한 처리도 추가
				const Float2 relativePressedMousePosition = _mousePressedPosition - controlData._absolutePosition;
				if (controlData._zones._titleBarZone.Contains(relativePressedMousePosition))
				{
					InteractionMousePressModuleInput interactionMousePressModuleInput;
					interactionMousePressModuleInput._controlID = controlData.GetID();
					interactionMousePressModuleInput._controlPosition = controlData._absolutePosition;
					interactionMousePressModuleInput._mousePressedPosition = _mousePressedPosition;
					_draggingModule.Begin(interactionMousePressModuleInput);
				}
			}

			void GUIContext::UpdateControlData_ResetNextControlDesc()
			{
				_nextControlDesc._sameLine = false;
				_nextControlDesc._position = Float2::kNan;
				_nextControlDesc._size = Float2::kNan;
				_nextControlDesc._margin = _theme._defaultMargin;
				_nextControlDesc._padding = _theme._defaultPadding;
			}

			void GUIContext::SelectResizingCursorType(const ControlData::ResizingFlags& resizingFlags)
			{
				if ((resizingFlags._top && resizingFlags._left) || (resizingFlags._bottom && resizingFlags._right))
				{
					_currentCursor = CursorType::SizeLeftTilted;
				}
				else if ((resizingFlags._top && resizingFlags._right) || (resizingFlags._bottom && resizingFlags._left))
				{
					_currentCursor = CursorType::SizeRightTilted;
				}
				else if (resizingFlags._top || resizingFlags._bottom)
				{
					_currentCursor = CursorType::SizeVert;
				}
				else if (resizingFlags._left || resizingFlags._right)
				{
					_currentCursor = CursorType::SizeHorz;
				}
			}

			void GUIContext::DrawText_(const ControlID& controlID, const Color& color, const FontRenderingOption& fontRenderingOption)
			{
				const ControlData& controlData = AccessControlData(controlID);
				DrawText_(controlData._absolutePosition, controlData._size, controlData._text, color, fontRenderingOption);
			}

			void GUIContext::DrawText_(const Float2& position, const Float2& size, const wchar_t* const text, const Color& color, const FontRenderingOption& fontRenderingOption)
			{
				_rendererContext.SetTextColor(color);

				const Float2 halfSize = size * 0.5f;
				Float2 finalPosition = position + halfSize;
				if (fontRenderingOption._directionHorz != TextRenderDirectionHorz::Centered)
				{
					finalPosition._x += (fontRenderingOption._directionHorz == TextRenderDirectionHorz::Rightward ? -halfSize._x : halfSize._x);
				}
				if (fontRenderingOption._directionVert != TextRenderDirectionVert::Centered)
				{
					finalPosition._y += (fontRenderingOption._directionVert == TextRenderDirectionVert::Downward ? -halfSize._y : halfSize._y);
				}
				_rendererContext.DrawDynamicText(text, Float4(finalPosition), fontRenderingOption);
			}

			Float4 GUIContext::ComputeShapePosition(const ControlID& controlID) const
			{
				const ControlData& controlData = AccessControlData(controlID);
				return ComputeShapePosition(controlData._absolutePosition, controlData._size);
			}

			Float4 GUIContext::ComputeShapePosition(const Float2& position, const Float2& size) const
			{
				return Float4(position + size * 0.5f);
			}

			float GUIContext::ComputeRoundness(const ControlID& controlID) const
			{
				const ControlData& controlData = AccessControlData(controlID);
				return _rendererContext.ComputeNormalizedRoundness(controlData._size.GetMinElement(), _theme._roundnessInPixel);
			}

			void GUIContext::DebugRender_Control(const ControlData& controlData)
			{
				if (_debugSwitch._renderZoneOverlay)
				{
					static const float OVERLAY_ALPHA = 0.125f;
					const Float2 titleBarZoneSize = controlData._zones._titleBarZone.Size();
					if (titleBarZoneSize._x != 0.0f && titleBarZoneSize._y != 0.0f)
					{
						_rendererContext.SetColor(Color(1.0f, 0.5f, 0.25f, OVERLAY_ALPHA));
						_rendererContext.SetPosition(ComputeShapePosition(controlData._absolutePosition + controlData._zones._titleBarZone.Position(), titleBarZoneSize));
						_rendererContext.DrawRectangle(titleBarZoneSize, 0.0f, 0.0f);
					}

					const Float2 contentZoneSize = controlData._zones._contentZone.Size();
					if (contentZoneSize._x != 0.0f && contentZoneSize._y != 0.0f)
					{
						_rendererContext.SetColor(Color(0.25f, 1.0f, 0.5f, OVERLAY_ALPHA));
						_rendererContext.SetPosition(ComputeShapePosition(controlData._absolutePosition + controlData._zones._contentZone.Position(), contentZoneSize));
						_rendererContext.DrawRectangle(contentZoneSize, 0.0f, 0.0f);
					}

					const Float2 visibleContentZoneSize = controlData._zones._visibleContentZone.Size();
					if (visibleContentZoneSize._x != 0.0f && visibleContentZoneSize._y != 0.0f)
					{
						_rendererContext.SetColor(Color(0.25f, 0.25f, 1.0f, OVERLAY_ALPHA));
						_rendererContext.SetPosition(ComputeShapePosition(controlData._absolutePosition + controlData._zones._visibleContentZone.Position(), visibleContentZoneSize));
						_rendererContext.DrawRectangle(visibleContentZoneSize, 0.0f, 0.0f);
					}
				}

				if (_debugSwitch._renderMousePoints)
				{
					static const float POINT_RADIUS = 4.0f;
					if (_draggingModule.IsInteractingWith(controlData.GetID()))
					{
						const InputContext& inputContext = InputContext::GetInstance();
						_rendererContext.SetColor(Color::kBlue);
						_rendererContext.SetPosition(Float4(_draggingModule.GetMousePressedPosition()));
						_rendererContext.DrawCircle(POINT_RADIUS);

						_rendererContext.SetColor(Color::kRed);
						_rendererContext.SetPosition(Float4(controlData._absolutePosition + _draggingModule.GetMousePressedPosition() - _draggingModule.GetInitialControlPosition()));
						_rendererContext.DrawCircle(POINT_RADIUS);
					}
				}

				if (_debugSwitch._renderResizingArea && controlData._resizingMask.IsAnyTrue())
				{
					const Rect controlRect = Rect(controlData._absolutePosition, controlData._size);
					Rect outerRect = controlRect;
					outerRect.ExpandByQuantity(_theme._outerResizingDistances);
					Rect innerRect = controlRect;
					innerRect.ShrinkByQuantity(_theme._innerResizingDistances);

					_rendererContext.SetColor(Color(0.0f, 0.0f, 1.0f, 0.25f));
					_rendererContext.SetPosition(ComputeShapePosition(outerRect.Position(), outerRect.Size()));
					_rendererContext.DrawRectangle(outerRect.Size(), 0.0f, 0.0f);

					_rendererContext.SetColor(Color(0.0f, 1.0f, 0.0f, 0.25f));
					_rendererContext.SetPosition(ComputeShapePosition(innerRect.Position(), innerRect.Size()));
					_rendererContext.DrawRectangle(innerRect.Size(), 0.0f, 0.0f);
				}
			}
		}
	}
}
