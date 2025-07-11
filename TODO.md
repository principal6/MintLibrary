# TODO
## ECS 관련 클래스 ECS 프로젝트 사용하도록 리팩토링!! 특히 GUI...
## StackVector => InlineVector 로 변경, 기능 확장!
## GUI 개편
### Manager classes for GUI system
ControlRegistry
FocusManager - Focus, Tab navigation, Visual focus effects
	- SetFocus, AdvanceFocus, ClearFocus
ZOrderManager
InputManager
	- Hit testing on controls
EventRouter => Event Routing, Decoupled communication between components or systems
	- Raise events, Register listeners, Route events up or down
ThemeManager
AnimationManager
TooltipManager

### Components
> Renderable Component
>> ShapeComponent: Holds and draws shapes( Border, Background, etc )
>> TextContentComponent: Holds text content
>> TextEditComponent: Manges Caret, Selection, Clipboard
>> TooltipComponent: Control can have tooltip

> Behavioral Component
>> ValueBindingComponent: Bind a value to a control
>> HoverableComponent: Control recieves hover events
>> ClickableComponent: Control recieves click events
>> FocusableComponent: Control recieves focus events. => Tab navigation, input routing
>> DraggableComponent: Control recieves drag events( TitleBar needs DragZone! )
>> ToggleableComponent: Control recieves toggle events
>> ResizableComponent: Control recieves resize events
>> ItemProviderComponent: It can access a list of data and can rebuild UI items on demand, and handles selection!
//>> SelectableComponent: A list of controls can be selected
>> ControlLayoutComponent: Organize layout of controls
>> GroupComponent: Control can be grouped with other controls
>> ModalComponent: Control can be modal
>> ZOrderComponent: It determines the z-order of the control

### Controls
"Controls can have child controls."
[Label] = Shape + TextContent
[Button] = [Label] + Clickable
[Thumb]: Shape + Clickable + Draggable
[Slider] = Shape + [Thumb] + ValueBinding
[ScrollBar] = [Buttons]s + [Slider]
[TextEdit] = Shape + TextContent + TextEdit + [ScrollBar]
[Panel] = Shape + [ScrollBar] + [Control]s + EntityLayout
[ListBox] = [Panel]{ [Label]s + [ScrollBar] } + ItemProvider
[ComboBox] = [Label] + [Button] + [Panel]{ [Label]s + [ScrollBar] } + ItemProvider
[CheckBox] = [Label] + Shape + Toggle
[RadioButton] = [Label] + Shape + Toggle + Group
[TitleBar] = Shape + TextContent + [Button]s
[Expander] = [TitleBar] + [Panel]
[Window] = [TitleBar] + [Panel] + Resizable
[SliderEdit] = [TextEdit] + [Track]

### TODO
[ProgressBar]
[Image]

## Renderer - MeshRenderer 추가
## Tree Print 기능 추가, Tree 를 가변트리/고정트리로 나눠서 Cache Miss 개선( Vector/StackVector 이용 )
## ShapeRenderer 에 Scale 기능 넣기!

## Camera 클래스 만들자! (SceneObject, Component 래핑용도...?)
## 기본 그래프 그리면서 확대,축소 카메라까지 쉽게 테스트할 수 있는 시스템 만들기!!!

## Euler's method error evaluation (with graph)
 => Physics integrator 관련!

## 간단한 Constraint 구현 (원 Constraint)
## 간단한 충돌 구현 #1 (2D, 원 끼리 충돌)
## 간단한 충돌 구현 #2 (2D, 다각형 충돌)

## GUI 렌더링!!

## wchar_t => UTF8 로 변환!! UTF8 FontData, DrawDynamicText 등

## Language library 정리
ILexer, IParse 등 코드 정리
test 추가
CPPHLSL 코드 정리, 로직 개선
