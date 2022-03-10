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
            union PerTypeData
            {
                struct WindowData
                {
                    float   _titleBarHeight = 0.0f;
                    float   _menuBarHeight = 0.0f;
                } _windowData{};
            };

        public:
            static const ControlID  generateID(const FileLine& fileLine, const ControlType type, const wchar_t* const text);

        public:
                                ControlData() : ControlData(ControlID(), ControlType::COUNT) { __noop; }
                                ControlData(const ControlID& ID, const ControlType type) : _ID{ ID }, _type{ type }, _accessCount{ 0 } { __noop; }
                                ~ControlData() = default;

        public:
            const ControlID&    getID() const { return _ID; }
            const ControlType&  getType() const { return _type; }
            const bool          hasValidType() const { return _type != ControlType::COUNT; }
            const uint64&       getAccessCount() const { return _accessCount; }
            
            Rect                computeTitleBarZone() const;
            Rect                computeContentZone() const;

        public:
            Float2              _position; // Relative to parent control
            Float2              _absolutePosition;
            Float2              _size;
            Float2              _contentZoneSize;
            Float2              _nextChildPosition;
            PerTypeData         _perTypeData;

        private:
            ControlID           _ID;
            ControlType         _type;
            uint64              _accessCount;
        };
    }
}


#endif // !_MINT_GUI_CONTROL_DATA_H_
