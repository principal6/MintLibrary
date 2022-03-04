#include <stdafx.h>
#include <MintRenderingBase/Include/GUI/GUIContext.h>

#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
    namespace GUI
    {
        void GUIContext::makeTestWindow(VisibleState& inoutVisibleState)
        {
            WindowParam windowParam;
            windowParam._position = Float2(200.0f, 50.0f);
            windowParam._scrollBarType = ScrollBarType::Both;
            _controlMetaStateSet.nextSize(Float2(500.0f, 500.0f), true);
            if (beginWindow(MINT_FILE_LINE(), L"TestWindow", windowParam, inoutVisibleState) == true)
            {
                static VisibleState childWindowVisibleState0;
                static VisibleState childWindowVisibleState1;


                if (beginMenuBar(MINT_FILE_LINE(), L"") == true)
                {
                    if (beginMenuBarItem(MINT_FILE_LINE(), L"파일") == true)
                    {
                        if (beginMenuItem(MINT_FILE_LINE(), L"새로 만들기") == true)
                        {
                            if (beginMenuItem(MINT_FILE_LINE(), L"T") == true)
                            {
                                endMenuItem();
                            }

                            if (beginMenuItem(MINT_FILE_LINE(), L"TT") == true)
                            {
                                endMenuItem();
                            }

                            endMenuItem();
                        }

                        if (beginMenuItem(MINT_FILE_LINE(), L"불러오기") == true)
                        {
                            if (beginMenuItem(MINT_FILE_LINE(), L"ABC") == true)
                            {
                                if (beginMenuItem(MINT_FILE_LINE(), L"Nest") == true)
                                {
                                    endMenuItem();
                                }

                                endMenuItem();
                            }

                            if (beginMenuItem(MINT_FILE_LINE(), L"DEF") == true)
                            {
                                endMenuItem();
                            }

                            endMenuItem();
                        }

                        if (beginMenuItem(MINT_FILE_LINE(), L"내보내기") == true)
                        {
                            endMenuItem();
                        }

                        endMenuBarItem();
                    }

                    if (beginMenuBarItem(MINT_FILE_LINE(), L"도움말") == true)
                    {
                        endMenuBarItem();
                    }

                    endMenuBar();
                }

                _controlMetaStateSet.nextTooltip(L"툴팁 테스트!");

                if (beginButton(MINT_FILE_LINE(), L"테스트") == true)
                {
                    endButton();
                }

                if (beginCheckBox(MINT_FILE_LINE(), L"체크박스") == true)
                {
                    endCheckBox();
                }

                {
                    SliderParam sliderParam;
                    _controlMetaStateSet.nextSize(Float2(32.0f, 0.0f));
                    float value = 0.0f;
                    if (beginSlider(MINT_FILE_LINE(), sliderParam, value) == true)
                    {
                        endSlider();
                    }
                }


                _controlMetaStateSet.nextSameLine();
                if (beginButton(MINT_FILE_LINE(), L"ChildWindow0") == true)
                {
                    childWindowVisibleState0 = VisibleState::VisibleOpen;

                    endButton();
                }

                _controlMetaStateSet.nextSameLine();
                if (beginButton(MINT_FILE_LINE(), L"ChildWindow1") == true)
                {
                    childWindowVisibleState1 = VisibleState::VisibleOpen;

                    endButton();
                }

                if (beginButton(MINT_FILE_LINE(), L"테스트2") == true)
                {
                    endButton();
                }

                if (beginButton(MINT_FILE_LINE(), L"테스트3") == true)
                {
                    endButton();
                }

                static StringW textBoxContent;
                {
                    TextBoxParam textBoxParam;
                    _controlMetaStateSet.nextSize(Float2(240.0f, 24.0f));
                    textBoxParam._alignmentHorz = TextAlignmentHorz::Center;
                    if (beginTextBox(MINT_FILE_LINE(), textBoxParam, textBoxContent) == true)
                    {
                        endTextBox();
                    }
                }

                ListViewParam listViewParam;
                int16 listViewSelectedItemIndex = 0;
                if (beginListView(MINT_FILE_LINE(), listViewSelectedItemIndex, listViewParam) == true)
                {
                    makeListItem(MINT_FILE_LINE(), L"아이템1");
                    makeListItem(MINT_FILE_LINE(), L"아이템2");
                    makeListItem(MINT_FILE_LINE(), L"아이템3");
                    makeListItem(MINT_FILE_LINE(), L"아이템4");
                    makeListItem(MINT_FILE_LINE(), L"아이템5");
                    makeListItem(MINT_FILE_LINE(), L"아이템6");

                    endListView();
                }

                {
                    WindowParam testWindowParam;
                    _controlMetaStateSet.nextSize(Float2(200.0f, 240.0f), true);
                    testWindowParam._scrollBarType = ScrollBarType::Both;
                    testWindowParam._initialDockZone = DockZone::BottomSide;
                    if (beginWindow(MINT_FILE_LINE(), L"1ST", testWindowParam, childWindowVisibleState0))
                    {
                        if (beginButton(MINT_FILE_LINE(), L"테스트!!") == true)
                        {
                            endButton();
                        }

                        endWindow();
                    }
                }

                {
                    WindowParam testWindowParam;
                    _controlMetaStateSet.nextSize(Float2(100.0f, 100.0f));
                    testWindowParam._position._x = 10.0f;
                    testWindowParam._position._y = 60.0f;
                    testWindowParam._initialDockZone = DockZone::BottomSide;
                    if (beginWindow(MINT_FILE_LINE(), L"2NDDD", testWindowParam, childWindowVisibleState1))
                    {
                        if (beginButton(MINT_FILE_LINE(), L"YEAH") == true)
                        {
                            endButton();
                        }

                        endWindow();
                    }
                }

                endWindow();
            }
        }

        void GUIContext::makeTestDockedWindow(VisibleState& inoutVisibleState)
        {
            WindowParam windowParam;
            _controlMetaStateSet.nextSize(Float2(300.0f, 400.0f), true);
            windowParam._position = Float2(20.0f, 50.0f);
            windowParam._initialDockZone = DockZone::RightSide;
            windowParam._initialDockingSize._x = 240.0f;
            if (beginWindow(MINT_FILE_LINE(), L"TestDockedWindow", windowParam, inoutVisibleState) == true)
            {
                if (beginButton(MINT_FILE_LINE(), L"버튼이요") == true)
                {
                    bool a = true;
                    endButton();
                }

                _controlMetaStateSet.nextSameLine();

                _controlMetaStateSet.nextTooltip(L"Button B Toolip!!");

                if (beginButton(MINT_FILE_LINE(), L"Button B") == true)
                {
                    endButton();
                }

                if (beginButton(MINT_FILE_LINE(), L"Another") == true)
                {
                    endButton();
                }

                makeLabel(MINT_FILE_LINE(), L"A label!");

                _controlMetaStateSet.nextSameLine();

                if (beginButton(MINT_FILE_LINE(), L"Fourth") == true)
                {
                    endButton();
                }

                endWindow();
            }
        }

        void GUIContext::makeDebugControlDataViewer(VisibleState& inoutVisibleState)
        {
            WindowParam windowParam;
            _controlMetaStateSet.nextSize(Float2(300.0f, 400.0f), true);
            windowParam._position = Float2(20.0f, 50.0f);
            windowParam._scrollBarType = ScrollBarType::Both;
            if (beginWindow(MINT_FILE_LINE(), L"ControlData Viewer", windowParam, inoutVisibleState) == true)
            {
                ScopeStringW<300> buffer;
                if (_controlInteractionStateSet.hasHoveredControl())
                {
                    const ControlData& hoveredControlData = getControlData(_controlInteractionStateSet.getHoveredControlID());
                    formatString(buffer, L"Hovered Control ID: %llX", hoveredControlData.getID().getRawValue());
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    formatString(buffer, L"Hovered Control Type: (%s)", getControlTypeWideString(hoveredControlData.getControlType()));
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());
                }

                if (_controlInteractionStateSet.hasPressedControl())
                {
                    const ControlData& pressedControlData = getControlData(_controlInteractionStateSet.getPressedControlID());
                    formatString(buffer, L"Pressed Control ID: %llX", pressedControlData.getID().getRawValue());
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    formatString(buffer, L"Pressed Control Type: (%s)", getControlTypeWideString(pressedControlData.getControlType()));
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());
                }

                if (_controlInteractionStateSet.hasFocusedControl())
                {
                    const ControlData& focusedControlData = getControlData(_controlInteractionStateSet.getFocusedControlID());
                    formatString(buffer, L"Focused Control ID: %llX", focusedControlData.getID().getRawValue());
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    formatString(buffer, L"Focused Control Type: (%s)", getControlTypeWideString(focusedControlData.getControlType()));
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());
                }

                if (isValidControl(_viewerTargetControlID) == true)
                {
                    const ControlData& viewerTargetControlData = getControlData(_viewerTargetControlID);

                    formatString(buffer, L"Control ID Map Size: %u", _controlIDMap.size());
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    formatString(buffer, L"ID: %llX", viewerTargetControlData.getID());
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    formatString(buffer, L"Control Type: (%s)", getControlTypeWideString(viewerTargetControlData.getControlType()));
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    formatString(buffer, L"Text: %s", viewerTargetControlData._text.c_str());
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    formatString(buffer, L"Position: (%f, %f)", viewerTargetControlData._position._x, viewerTargetControlData._position._y);
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    formatString(buffer, L"InteractionSize: (%f, %f)", viewerTargetControlData.getInteractionSize()._x, viewerTargetControlData.getInteractionSize()._y);
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    formatString(buffer, L"Inner InteractionSize: (%f, %f)", viewerTargetControlData.getInnerInteractionSize()._x, viewerTargetControlData.getInnerInteractionSize()._y);
                    makeLabel(MINT_FILE_LINE(), buffer.c_str());

                    makeFromReflectionClass(MINT_FILE_LINE(), viewerTargetControlData.getReflectionData(), &viewerTargetControlData);
                }

                endWindow();
            }
        }

        void GUIContext::makeFromReflectionClass(const FileLine& fileLine, const ReflectionData& reflectionData, const void* const reflectionClass)
        {
            ScopeStringA<300> bufferA;
            ScopeStringW<300> bufferW;

            const uint32 memberCount = reflectionData._memberTypeDatas.size();
            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                TypeBaseData* const memberTypeData = reflectionData._memberTypeDatas[memberIndex];
                const char* const member = reinterpret_cast<const char*>(reflectionClass) + memberTypeData->_offset;
                if (memberTypeData->_typeName == "Float2")
                {
                    const Float2& memberCasted = *reinterpret_cast<const Float2*>(member);
                    formatString(bufferA, "%s: (%f, %f)", memberTypeData->_declarationName.c_str(), memberCasted._x, memberCasted._y);

                    StringUtil::convertScopeStringAToScopeStringW(bufferA, bufferW);
                    makeLabel(fileLine, bufferW.c_str());
                }
                else if (memberTypeData->_typeName == "uint64")
                {
                    const uint64 memberCasted = *reinterpret_cast<const uint64*>(member);
                    formatString(bufferA, "%s: %llu", memberTypeData->_declarationName.c_str(), memberCasted);

                    StringUtil::convertScopeStringAToScopeStringW(bufferA, bufferW);
                    makeLabel(fileLine, bufferW.c_str());
                }
                else if (memberTypeData->_typeName == "StringW")
                {
                    const StringW& memberCasted = *reinterpret_cast<const StringW*>(member);
                    ScopeStringW<300> bufferWTemp;
                    bufferA = memberTypeData->_declarationName.c_str();
                    StringUtil::convertScopeStringAToScopeStringW(bufferA, bufferWTemp);
                    formatString(bufferW, L"%s: %s", bufferWTemp.c_str(), memberCasted.c_str());
                    makeLabel(fileLine, bufferW.c_str());
                }
                else
                {
                    continue;
                }
            }
        }
    }
}
