#pragma once


#ifndef _MINT_GUI_INTERACTION_MODULES_H_
#define _MINT_GUI_INTERACTION_MODULES_H_


#include <MintRenderingBase/Include/GUI/ControlData.h>


namespace mint
{
    namespace Rendering
    {
        namespace GUI
        {
#pragma region InteractionModule
            template<typename Input>
            class InteractionModule
            {
            public:
                virtual bool                    begin(const Input& input) abstract;
                virtual void                    end() abstract;

            public:
                virtual bool                    isInteracting() const abstract;
            };
#pragma endregion


#pragma region InteractionMousePressModule
            struct InteractionMousePressModuleInput
            {
                ControlID           _controlID;
                Float2              _controlPosition;
                Float2              _mousePressedPosition;
            };

            template<typename Input>
            class InteractionMousePressModule : public InteractionModule<Input>
            {
            public:
                virtual bool                    begin(const Input& input) override;
                virtual void                    end() override;

            public:
                virtual bool                    isInteracting() const override;
                virtual bool                    isInteractingWith(const ControlID& controlID) const;

            public:
                Float2                          computeRelativeMousePressedPosition() const;
                const Float2&                   getMousePressedPosition() const { return _input._mousePressedPosition; }
                const Float2&                   getInitialControlPosition() const { return _input._controlPosition; }
                const ControlID&                getControlID() const { return _input._controlID; }

            protected:
                Input                           _input;
            };
#pragma endregion

            
            using DraggingModule = InteractionMousePressModule<InteractionMousePressModuleInput>;
            using FocusingModule = InteractionMousePressModule<InteractionMousePressModuleInput>;


            struct ResizingModuleInput : public InteractionMousePressModuleInput
            {
                Float2                          _controlSize;
                ControlData::ResizingFlags      _resizingInteraction;
            };
            class ResizingModule final : public InteractionMousePressModule<ResizingModuleInput>
            {
            public:
                static void                         makeOuterAndInenrRects(const ControlData& controlData, const Rect& outerResizingDistances, const Rect& innerResizingDistances, Rect& outerRect, Rect& innerRect);
                static ControlData::ResizingFlags   makeResizingFlags(const Float2& mousePosition, const ControlData& controlData, const Rect& outerRect, const Rect& innerRect);

            public:
                virtual bool                        begin(const ResizingModuleInput& resizingModuleInput) override;

            public:
                MINT_INLINE const Float2&           getInitialControlSize() const { return _input._controlSize; }
                MINT_INLINE const ControlData::ResizingFlags&   getResizingFlags() const { return _input._resizingInteraction; }
            };


            struct DockingModuleInput
            {
                ControlID _dockControlID;
                ControlID _shipControlID;
            };
            class DockingModule final : public InteractionModule<DockingModuleInput>
            {
            public:
                virtual bool                    begin(const DockingModuleInput& dockingShipModuleInput) override;
                virtual void                    end() override;

            public:
                virtual bool                    isInteracting() const override;
                bool                            isDockControl(const ControlID& controlID) const;
                bool                            isShipControl(const ControlID& controlID) const;
            
            public:
                const ControlID&                getDockControlID() const { return _input._dockControlID; }
                const ControlID&                getShipControlID() const { return _input._shipControlID; }

            private:
                DockingModuleInput          _input;
            };
        }
    }
}


#endif // !_MINT_GUI_INTERACTION_MODULES_H_
