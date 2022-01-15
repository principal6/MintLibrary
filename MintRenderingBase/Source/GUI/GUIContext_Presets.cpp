#include <stdafx.h>
#include <MintRenderingBase/Include/GUI/GUIContext.h>

#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
    namespace GUI
    {

        void GUIContext::makeTestWindow(VisibleState& inoutVisibleState)
        {
            GUI::WindowParam windowParam;
            windowParam._position = Float2(200.0f, 50.0f);
            windowParam._scrollBarType = GUI::ScrollBarType::Both;
            _controlMetaStateSet.nextSize(Float2(500.0f, 500.0f), true);
            if (beginWindow(MINT_GUI_CONTROL(L"TestWindow", windowParam, inoutVisibleState)) == true)
            {
                static VisibleState childWindowVisibleState0;
                static VisibleState childWindowVisibleState1;


                if (beginMenuBar(MINT_GUI_CONTROL(L"")) == true)
                {
                    if (beginMenuBarItem(MINT_GUI_CONTROL(L"����")) == true)
                    {
                        if (beginMenuItem(MINT_GUI_CONTROL(L"���� �����")) == true)
                        {
                            if (beginMenuItem(MINT_GUI_CONTROL(L"T")) == true)
                            {
                                endMenuItem();
                            }

                            if (beginMenuItem(MINT_GUI_CONTROL(L"TT")) == true)
                            {
                                endMenuItem();
                            }

                            endMenuItem();
                        }

                        if (beginMenuItem(MINT_GUI_CONTROL(L"�ҷ�����")) == true)
                        {
                            if (beginMenuItem(MINT_GUI_CONTROL(L"ABC")) == true)
                            {
                                if (beginMenuItem(MINT_GUI_CONTROL(L"Nest")) == true)
                                {
                                    endMenuItem();
                                }

                                endMenuItem();
                            }

                            if (beginMenuItem(MINT_GUI_CONTROL(L"DEF")) == true)
                            {
                                endMenuItem();
                            }

                            endMenuItem();
                        }

                        if (beginMenuItem(MINT_GUI_CONTROL(L"��������")) == true)
                        {
                            endMenuItem();
                        }

                        endMenuBarItem();
                    }

                    if (beginMenuBarItem(MINT_GUI_CONTROL(L"����")) == true)
                    {
                        endMenuBarItem();
                    }

                    endMenuBar();
                }

                _controlMetaStateSet.nextTooltip(L"���� �׽�Ʈ!");

                if (beginButton(MINT_GUI_CONTROL(L"�׽�Ʈ")) == true)
                {
                    endButton();
                }

                if (beginCheckBox(MINT_GUI_CONTROL(L"üũ�ڽ�")) == true)
                {
                    endCheckBox();
                }

                {
                    GUI::SliderParam sliderParam;
                    _controlMetaStateSet.nextSize(Float2(32.0f, 0.0f));
                    float value = 0.0f;
                    if (beginSlider(MINT_GUI_CONTROL(sliderParam, value)) == true)
                    {
                        endSlider();
                    }
                }


                _controlMetaStateSet.nextSameLine();
                if (beginButton(MINT_GUI_CONTROL(L"ChildWindow0")) == true)
                {
                    childWindowVisibleState0 = VisibleState::VisibleOpen;

                    endButton();
                }

                _controlMetaStateSet.nextSameLine();
                if (beginButton(MINT_GUI_CONTROL(L"ChildWindow1")) == true)
                {
                    childWindowVisibleState1 = VisibleState::VisibleOpen;

                    endButton();
                }

                if (beginButton(MINT_GUI_CONTROL(L"�׽�Ʈ2")) == true)
                {
                    endButton();
                }

                if (beginButton(MINT_GUI_CONTROL(L"�׽�Ʈ3")) == true)
                {
                    endButton();
                }

                static StringW textBoxContent;
                {
                    GUI::TextBoxParam textBoxParam;
                    _controlMetaStateSet.nextSize(Float2(240.0f, 24.0f));
                    textBoxParam._alignmentHorz = GUI::TextAlignmentHorz::Center;
                    if (beginTextBox(MINT_GUI_CONTROL(textBoxParam, textBoxContent)) == true)
                    {
                        endTextBox();
                    }
                }

                GUI::ListViewParam listViewParam;
                int16 listViewSelectedItemIndex = 0;
                if (beginListView(MINT_GUI_CONTROL(listViewSelectedItemIndex, listViewParam)) == true)
                {
                    makeListItem(MINT_GUI_CONTROL(L"������1"));
                    makeListItem(MINT_GUI_CONTROL(L"������2"));
                    makeListItem(MINT_GUI_CONTROL(L"������3"));
                    makeListItem(MINT_GUI_CONTROL(L"������4"));
                    makeListItem(MINT_GUI_CONTROL(L"������5"));
                    makeListItem(MINT_GUI_CONTROL(L"������6"));

                    endListView();
                }

                {
                    GUI::WindowParam testWindowParam;
                    _controlMetaStateSet.nextSize(Float2(200.0f, 240.0f), true);
                    testWindowParam._scrollBarType = GUI::ScrollBarType::Both;
                    testWindowParam._initialDockZone = GUI::DockZone::BottomSide;
                    if (beginWindow(MINT_GUI_CONTROL(L"1ST", testWindowParam, childWindowVisibleState0)))
                    {
                        if (beginButton(MINT_GUI_CONTROL(L"�׽�Ʈ!!")) == true)
                        {
                            endButton();
                        }

                        endWindow();
                    }
                }

                {
                    GUI::WindowParam testWindowParam;
                    _controlMetaStateSet.nextSize(Float2(100.0f, 100.0f));
                    testWindowParam._position._x = 10.0f;
                    testWindowParam._position._y = 60.0f;
                    testWindowParam._initialDockZone = GUI::DockZone::BottomSide;
                    if (beginWindow(MINT_GUI_CONTROL(L"2NDDD", testWindowParam, childWindowVisibleState1)))
                    {
                        if (beginButton(MINT_GUI_CONTROL(L"YEAH")) == true)
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
            GUI::WindowParam windowParam;
            _controlMetaStateSet.nextSize(Float2(300.0f, 400.0f), true);
            windowParam._position = Float2(20.0f, 50.0f);
            windowParam._initialDockZone = GUI::DockZone::RightSide;
            windowParam._initialDockingSize._x = 240.0f;
            if (beginWindow(MINT_GUI_CONTROL(L"TestDockedWindow", windowParam, inoutVisibleState)) == true)
            {
                if (beginButton(MINT_GUI_CONTROL(L"��ư�̿�")) == true)
                {
                    bool a = true;
                    endButton();
                }

                _controlMetaStateSet.nextSameLine();

                _controlMetaStateSet.nextTooltip(L"Button B Toolip!!");

                if (beginButton(MINT_GUI_CONTROL(L"Button B")) == true)
                {
                    endButton();
                }

                if (beginButton(MINT_GUI_CONTROL(L"Another")) == true)
                {
                    endButton();
                }

                makeLabel(MINT_GUI_CONTROL(L"A label!"));

                _controlMetaStateSet.nextSameLine();

                if (beginButton(MINT_GUI_CONTROL(L"Fourth")) == true)
                {
                    endButton();
                }

                endWindow();
            }
        }

        void GUIContext::makeDebugControlDataViewer(VisibleState& inoutVisibleState)
        {
            GUI::WindowParam windowParam;
            _controlMetaStateSet.nextSize(Float2(300.0f, 400.0f), true);
            windowParam._position = Float2(20.0f, 50.0f);
            if (beginWindow(MINT_GUI_CONTROL(L"ControlData Viewer", windowParam, inoutVisibleState)) == true)
            {
                if (isValidControl(_viewerTargetControlID) == true)
                {
                    ScopeStringW<300> buffer;
                    const ControlData& controlData = getControlData(_viewerTargetControlID);

                    formatString(buffer, L"Control ID Map Size: %u", _controlIDMap.size());
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    formatString(buffer, L"ID: %llX", controlData.getID());
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    formatString(buffer, L"Control Type: (%s)", getControlTypeWideString(controlData.getControlType()));
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    formatString(buffer, L"Text: %s", controlData._text.c_str());
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    formatString(buffer, L"Position: (%f, %f)", controlData._position._x, controlData._position._y);
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    formatString(buffer, L"InteractionSize: (%f, %f)", controlData.getInteractionSize()._x, controlData.getInteractionSize()._y);
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    formatString(buffer, L"Inner InteractionSize: (%f, %f)", controlData.getInnerInteractionSize()._x, controlData.getInnerInteractionSize()._y);
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    makeFromReflectionClass(MINT_GUI_CONTROL(controlData.getReflectionData(), &controlData));
                }

                endWindow();
            }
        }

        void GUIContext::makeFromReflectionClass(const char* const file, const int line, const ReflectionData& reflectionData, const void* const reflectionClass)
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
                    makeLabel(file, line, bufferW.c_str());
                }
                else if (memberTypeData->_typeName == "uint64")
                {
                    const uint64 memberCasted = *reinterpret_cast<const uint64*>(member);
                    formatString(bufferA, "%s: %llu", memberTypeData->_declarationName.c_str(), memberCasted);

                    StringUtil::convertScopeStringAToScopeStringW(bufferA, bufferW);
                    makeLabel(file, line, bufferW.c_str());
                }
                else if (memberTypeData->_typeName == "StringW")
                {
                    const StringW& memberCasted = *reinterpret_cast<const StringW*>(member);
                    ScopeStringW<300> bufferWTemp;
                    bufferA = memberTypeData->_declarationName.c_str();
                    StringUtil::convertScopeStringAToScopeStringW(bufferA, bufferWTemp);
                    formatString(bufferW, L"%s: %s", bufferWTemp.c_str(), memberCasted.c_str());
                    makeLabel(file, line, bufferW.c_str());
                }
                else
                {
                    continue;
                }
            }
        }
    }
}