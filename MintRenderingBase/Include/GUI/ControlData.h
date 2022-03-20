#pragma once


#ifndef _MINT_GUI_CONTROL_DATA_H_
#define _MINT_GUI_CONTROL_DATA_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Rect.h>


namespace mint
{
    // Use MINT_FILE_LINE macro
    // in order to create a FileLine instance.
    struct FileLine
    {
        explicit    FileLine(const char* const file, const int line) : _file{ file }, _line{ line } { __noop; }

        const char* _file;
        int         _line;
    };
#define MINT_FILE_LINE FileLine(__FILE__, __LINE__)


    namespace Rendering
    {
        enum class ControlType
        {
            Label,
            Button,
            Window,
            COUNT
        };

        class ControlID
        {
        public:
                            ControlID() : _rawID{ 0 } { __noop; }
                            ControlID(const uint64 rawID) : _rawID{ rawID } { __noop; }
                            ~ControlID() = default;

            const bool      operator==(const ControlID& rhs) const { return _rawID == rhs._rawID; }
            const bool      operator!=(const ControlID& rhs) const { return _rawID != rhs._rawID; }
            const uint64    getRawID() const { return _rawID; }

        private:
            uint64          _rawID;
        };

        
        class ControlData
        {
        public:
            static const ControlID  generateID(const FileLine& fileLine, const ControlType type, const wchar_t* const text, const ControlID& parentControlID);

        public:
                                ControlData() : ControlData(ControlID(), ControlType::COUNT) { __noop; }
                                ControlData(const ControlID& ID, const ControlType type);
                                ~ControlData() = default;

        public:
            void                computeZones();
            void                setPressedMousePosition(const Float2& absolutePressedMousePosition);
            void                clearPressedMousePosition();

        public:
            const ControlID&    getID() const { return _ID; }
            const ControlType&  getType() const { return _type; }
            const bool          hasValidType() const { return _type != ControlType::COUNT; }
            const uint64&       getAccessCount() const { return _accessCount; }
            const Float2&       getAbsolutePressedPosition() const { return _absolutePressedPosition; }
            const Float2&       getAbsolutePressedMousePosition() const { return _absolutePressedMousePosition; }
            const Float2&       getRelativePressedMousePosition() const { return _relativePressedMousePosition; }

        private:
            void                computeContentZone();
            void                computeVisibleContentZone();
            void                computeTitleBarZone();

        public:
            Float2              _relativePosition; // Relative to parent control
            Float2              _absolutePosition; // In screen space
            Float2              _size;
            Float2              _nextChildRelativePosition;

        public:
            enum class InteractionState
            {
                None,
                Hovering,
                Pressing,
                Clicked,
                Dragged,
                //Resized,
            };
            InteractionState    _interactionState;

        public:
            struct Zones
            {
                Rect            _contentZone;
                Rect            _visibleContentZone;
                Rect            _titleBarZone;
                Rect            _menuBarZone;
                //Rect            _dockZone; // DockZone 은 여러 개가 될 것.
            };
            Zones               _zones;
        
        public:
            union PerTypeData
            {
                struct WindowData
                {
                    float   _titleBarHeight = 0.0f;
                    float   _menuBarHeight = 0.0f;
                } _windowData{};
            };
            PerTypeData         _perTypeData;

        private:
            Float2              _absolutePressedPosition;
            Float2              _absolutePressedMousePosition;
            Float2              _relativePressedMousePosition;

        private:
            ControlID           _ID;
            ControlType         _type;
            uint64              _accessCount;
        };
    }
}


#endif // !_MINT_GUI_CONTROL_DATA_H_
