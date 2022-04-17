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
            class InteractionModule
            {
            public:
                const bool                      isInteracting() const;
                const bool                      isInteractingWith(const ControlData& controlData) const;
                virtual const bool              begin(const ControlData& controlData, const Float2& mousePressedPosition, const void* const customData) abstract;
                MINT_INLINE virtual void        end() { _controlID.invalidate(); }

            public:
                Float2                          computeRelativeMousePressedPosition() const;
                MINT_INLINE const Float2&       getMousePressedPosition() const { return _mousePressedPosition; }
                MINT_INLINE const Float2&       getInitialControlPosition() const { return _initialControlPosition; }
                MINT_INLINE const ControlID&    getControlID() const { return _controlID; }

            protected:
                const bool                      beginInternal(const ControlData& controlData, const Float2& mousePressedPosition);

            protected:
                ControlID                       _controlID;
                Float2                          _mousePressedPosition = Float2::kNan;
                Float2                          _initialControlPosition = Float2::kNan;
            };


            class DraggingModule final : public InteractionModule
            {
            public:
                virtual const bool              begin(const ControlData& controlData, const Float2& mousePressedPosition, const void* const customData = nullptr) override { return beginInternal(controlData, mousePressedPosition); }
            };


            class ResizingModule final : public InteractionModule
            {
            public:
                static void                         makeOuterAndInenrRects(const ControlData& controlData, const Rect& outerResizingDistances, const Rect& innerResizingDistances, Rect& outerRect, Rect& innerRect);
                static ControlData::ResizingFlags   makeResizingFlags(const Float2& mousePosition, const ControlData& controlData, const Rect& outerRect, const Rect& innerRect);

            public:
                virtual const bool                  begin(const ControlData& controlData, const Float2& mousePressedPosition, const void* const customData) override;

            public:
                MINT_INLINE const Float2&           getInitialControlSize() const { return _initialControlSize; }
                MINT_INLINE const ControlData::ResizingFlags&   getResizingFlags() const { return _resizingFlags; }

            private:
                Float2                              _initialControlSize;
                ControlData::ResizingFlags          _resizingFlags;
            };


            class FocusingModule final : public InteractionModule
            {
            public:
                virtual const bool              begin(const ControlData& controlData, const Float2& mousePressedPosition, const void* const customData = nullptr) override { return beginInternal(controlData, mousePressedPosition); }
            };
        }
    }
}


#endif // !_MINT_GUI_INTERACTION_MODULES_H_
