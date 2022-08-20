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
                bool        operator==(const ControlID& rhs) const { return _rawID == rhs._rawID; }
                bool        operator!=(const ControlID& rhs) const { return _rawID != rhs._rawID; }

            public:
                void        invalidate() { _rawID = 0; }
                bool        isValid() const { return _rawID != 0; }
                uint64      getRawID() const { return _rawID; }

            private:
                uint64      _rawID;
            };


            class ControlData
            {
            public:
                static ControlID    generateID(const FileLine& fileLine, const ControlType type, const wchar_t* const text, const ControlID& parentControlID);

            public:
                                    ControlData() : ControlData(ControlID(), ControlType::COUNT) { __noop; }
                                    ControlData(const ControlID& ID, const ControlType type);
                                    ~ControlData() = default;

            public:
                void                updateZones();

            public:
                const ControlID&    getID() const { return _ID; }
                const ControlType&  getType() const { return _type; }
                bool                hasValidType() const { return _type != ControlType::COUNT; }
                const uint64&       getAccessCount() const { return _accessCount; }
                Float2              computeRelativePosition(const ControlData& parentControlData) const;

            private:
                void                computeTitleBarZone(Rect& titleBarZone);
                void                computeMenuBarZone(Rect& menuBarZone);
                void                computeContentZone(Rect& contentZone);

            public:
                ControlID           _parentID;
                Float2              _absolutePosition; // Absolute position in screen space
                Float2              _size;
                Float2              _nextChildSameLinePosition;
                Float2              _nextChildNextLinePosition;
                const wchar_t*      _text;

            public:
                enum class MouseInteractionState
                {
                    None,
                    Hovering,
                    Pressing,
                    Clicked,
                };
                MouseInteractionState   _mouseInteractionState;

            public:
                struct Zones
                {
                    Rect            _contentZone;
                    Rect            _visibleContentZone;
                    Rect            _titleBarZone;
                    Rect            _menuBarZone;
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
                struct ResizingFlags
                {
                    void            setAllTrue() { _raw = 0b1111; }
                    void            setAllFalse() { _raw = 0; }
                    bool            isAnyTrue() const { return (_raw & 0b1111) != 0; }
                    bool            isAllFalse() const { return (_raw & 0b1111) == 0; }
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
                ResizingFlags       _resizingMask;
                Float2              _resizableMinSize;
        
            public:
                struct GeneralTraits
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
                GeneralTraits       _generalTraits;

            private:
                ControlID           _ID;
                ControlType         _type;
                uint64              _accessCount;
            };
        }
    }
}


#endif // !_MINT_GUI_CONTROL_DATA_H_
