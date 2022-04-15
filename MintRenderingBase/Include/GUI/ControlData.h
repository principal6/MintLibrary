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
        namespace GUI
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

            public:
                const bool      operator==(const ControlID& rhs) const { return _rawID == rhs._rawID; }
                const bool      operator!=(const ControlID& rhs) const { return _rawID != rhs._rawID; }

            public:
                void            invalidate() { _rawID = 0; }
                const bool      isValid() const { return _rawID != 0; }
                const uint64    getRawID() const { return _rawID; }

            private:
                uint64          _rawID;
            };

            struct ResizingFlags
            {
                void            setAllTrue() { _raw = 0b1111; }
                void            setAllFalse() { _raw = 0; }
                const bool      isAnyTrue() const { return (_raw & 0b1111) != 0; }
                const bool      isAllFalse() const { return (_raw & 0b1111) == 0; }
                void            maskBy(const ResizingFlags& mask) { _raw = _raw & mask._raw; }

                union
                {
                    uint8       _raw{};
                    struct
                    {
                        bool    _top : 1;
                        bool    _bottom : 1;
                        bool    _left : 1;
                        bool    _right : 1;
                    };
                };
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
                void                updateZones();

            public:
                const ControlID&    getID() const { return _ID; }
                const ControlType&  getType() const { return _type; }
                const bool          hasValidType() const { return _type != ControlType::COUNT; }
                const uint64&       getAccessCount() const { return _accessCount; }
                Float2              computeRelativePosition(const ControlData& parentControlData) const;

            private:
                void                computeContentZone();
                void                computeVisibleContentZone();
                void                computeTitleBarZone();

            public:
                Float2              _absolutePosition; // Absolute position in screen space
                Float2              _size;
                Float2              _nextChildSameLinePosition;
                Float2              _nextChildNextLinePosition;

            public:
                enum class InteractionState
                {
                    None,
                    Hovering,
                    Pressing,
                    Clicked,
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
                        float       _titleBarHeight = 0.0f;
                        float       _menuBarHeight = 0.0f;
                    } _windowData{};
                };
                PerTypeData         _perTypeData;

            public:
                ResizingFlags       _resizingMask;
                Float2              _resizableMinSize;
        
            public:
                struct Traits
                {
                    union
                    {
                        uint8   _raw = 0;
                        struct
                        {
                            bool    _isFocusable : 1;
                            bool    _isDraggable : 1;
                        };
                    };
                };
                Traits              _traits;

            private:
                ControlID           _ID;
                ControlType         _type;
                uint64              _accessCount;
            };
        }
    }
}


#endif // !_MINT_GUI_CONTROL_DATA_H_
