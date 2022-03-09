#pragma once


#ifndef _MINT_GUI_CONTROL_DATA_H_
#define _MINT_GUI_CONTROL_DATA_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float2.h>


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
            static const ControlID  generateID(const FileLine& fileLine, const ControlType type, const wchar_t* const text);

        public:
                                ControlData() : ControlData(ControlID(), ControlType::COUNT) { __noop; }
                                ControlData(const ControlID& ID, const ControlType type) : _ID{ ID }, _type{ type } { __noop; }
                                ~ControlData() = default;

        public:
            const ControlID&    getID() const { return _ID; }
            const ControlType&  getType() const { return _type; }

        public:
            Float2              _position;
            Float2              _size;

        private:
            ControlID           _ID;
            ControlType         _type;
        };
    }
}


#endif // !_MINT_GUI_CONTROL_DATA_H_
