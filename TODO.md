# TODO
## GUI 개편
컨트롤 배치 등 꾸미는 쪽은 Script 이용해서 Hot Reload 가능하면 좋을 듯!
```cpp
void OnClicked(GUIObject& object)
{

}

void OnDragEnd(GUIObject& object)
{

}

void TestGUI(Window& window)
{
    GUIFactory factory;
    {
        GUIObjectTemplate objectTemplate;
        OwnPtr<GUIShapeComponent> shapeComponent = factory.CreateComponent<GUIShapeComponent>();
        SharedPtr<Shape2D> defaultShape;
        SharedPtr<Shape2D> pressedShape;
        SharedPtr<Shape2D> focusedShape;
        shapeComponent->SetShape(GUIShapeSlot::Default, defaultShape);
        shapeComponent->SetShape(GUIShapeSlot::Pressed, pressedShape);
        shapeComponent->SetShape(GUIShapeSlot::Focused, focusedShape);
        objectTemplate.AddComponent(move(shapeComponent));

        OwnPtr<GUITextComponent> textComponent = factory.CreateComponent<GUITextComponent>();
        float2 offset(0, 0);
        textComponent->SetText("Test", offset);
        objectTemplate.AddComponent(move(textComponent));

        objectTemplate.AddComponent(factory.CreateComponent<GUIClickableComponent>());

        objectTemplate.AddComponent(factory.CreateComponent<GUIDraggableComponent>());

        factory.AddObjectTemplate(objectTemplate, "TestObjectTemplate");
    }
    
    GUIContext context( window );
    OwnPtr<GUIObject> object = factory.CreateObject("TestObjectTemplate");
    object->AddCallback(GUICallback::OnClicked, OnClicked);
    object->AddCallback(GUICallback::OnDragEnd, OnDragEnd);

    context.AddObject(object);
    context.Render();
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
