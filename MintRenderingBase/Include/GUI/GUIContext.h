#pragma once


#ifndef _MINT_GUI_CONTEXT_H_
#define _MINT_GUI_CONTEXT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/BitVector.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/HashMap.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Rect.h>


namespace mint
{
    // Use MINT_FILE_LINE macro!
    struct FileLine
    {
        explicit FileLine(const char* const file, const int line) : _file{ file }, _line{ line } { __noop; }
        const char* const _file;
        const int         _line;
    };
    #define MINT_FILE_LINE() FileLine(__FILE__, __LINE__)


    namespace Rendering
    {
        class GraphicDevice;
    }


    namespace GUI
    {
        class GUIContext final
        {
            friend Rendering::GraphicDevice;

        private:
                                                GUIContext(Rendering::GraphicDevice& graphicDevice);

        public:
                                                ~GUIContext();

        public:
            void                                initialize();
            void                                updateScreenSize(const Float2& newScreenSize);

        public:
            void                                processEvent() noexcept;
            void                                render() noexcept;

        private:
            Rendering::GraphicDevice&           _graphicDevice;
            Rendering::ShapeRendererContext     _rendererContext;
        };
    }
}


#endif // !_MINT_GUI_CONTEXT_H_
