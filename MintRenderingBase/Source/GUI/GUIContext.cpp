#include <stdafx.h>
#include <MintRenderingBase/Include/GUI/GUIContext.h>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintReflection/Include/Reflection.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>

#include <MintPlatform/Include/WindowsWindow.h>
#include <MintPlatform/Include/InputContext.h>

#include <MintLibrary/Include/ScopedCPUProfiler.h>


namespace mint
{
    namespace Rendering
    {
        GUIContext::GUIContext(Rendering::GraphicDevice& graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _rendererContext{ graphicDevice }
        {
            __noop;
        }

        GUIContext::~GUIContext()
        {
            __noop;
        }

        void GUIContext::initialize()
        {
            //_caretBlinkIntervalMs = _graphicDevice.getWindow().getCaretBlinkIntervalMs();

            const Rendering::FontData& fontData = _graphicDevice.getShapeRendererContext().getFontData();
            //_fontSize = static_cast<float>(fontData._fontSize);
            if (_rendererContext.initializeFontData(fontData) == false)
            {
                MINT_ASSERT(false, "ShapeRendererContext::initializeFont() 에 실패했습니다!");
            }

            _rendererContext.initializeShaders();
            _rendererContext.setUseMultipleViewports();

            const Float2& windowSize = Float2(_graphicDevice.getWindowSize());
            updateScreenSize(windowSize);
        }

        void GUIContext::updateScreenSize(const Float2& newScreenSize)
        {
            // TODO
        }

        void GUIContext::processEvent() noexcept
        {
            // TODO
        }

        void GUIContext::render() noexcept
        {
            // TODO
        }
    }
}
