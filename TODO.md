# TODO
## ID class 가 별도로 nextID 를 저장할 필요가 없도록 개선하기... => Singleton Inner Class 가 ID 생성하면 될 듯???
## Tree Print 기능 추가, Tree 를 가변트리/고정트리로 나눠서 Cache Miss 개선( Vector/StackVector 이용 )
## ShapeRenderer 에 Scale 기능 넣기!
## GUI 개편
컨트롤 배치 등 꾸미는 쪽은 Script 이용해서 Hot Reload 가능하면 좋을 듯!
```cpp
void OnClicked(GUIEntity& object)
{

}

void OnDragEnd(GUIEntity& object)
{

}

void TestGUI(GraphicsDevice& graphicsDevice)
{
    GUIFactory guiFactory;
    {
        GUIEntityTemplate guiObjectTemplate;
        SharedPtr<GUIShapeComponent> guiShapeComponent = guiFactory.CreateComponent<GUIShapeComponent>();
        SharedPtr<Shape2D> defaultShape;
        SharedPtr<Shape2D> pressedShape;
        SharedPtr<Shape2D> focusedShape;
        guiShapeComponent->SetShape(GUIShapeSlot::Default, defaultShape);
        guiShapeComponent->SetShape(GUIShapeSlot::Pressed, pressedShape);
        guiShapeComponent->SetShape(GUIShapeSlot::Focused, focusedShape);
        guiObjectTemplate.AddComponent(move(guiShapeComponent));

        SharedPtr<GUITextComponent> guiTextComponent = guiFactory.CreateComponent<GUITextComponent>();
        float2 offset(0, 0);
        guiTextComponent->SetText("Test", offset);
        guiObjectTemplate.AddComponent(move(guiTextComponent));

        guiObjectTemplate.AddComponent(guiFactory.CreateComponent<GUIClickableComponent>());

        guiObjectTemplate.AddComponent(guiFactory.CreateComponent<GUIDraggableComponent>());

        guiFactory.RegisterObjectTemplate(guiObjectTemplate, "TestObjectTemplate");
    }
    
    GUIContext guiContext( graphicsDevice );
    SharedPtr<GUIEntity> guiObject = guiFactory.CreateObject("TestObjectTemplate");
    guiObject->AddCallback(GUICallback::OnClicked, OnClicked);
    guiObject->AddCallback(GUICallback::OnDragEnd, OnDragEnd);

    guiContext.AddObject(guiObject);
    guiContext.Render();
}
```

## Camera 클래스 만들자! (Object, Component 래핑용도...?)
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
