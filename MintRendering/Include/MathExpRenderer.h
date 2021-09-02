#pragma once


#ifndef MINT_RENDERING_MATH_EXPRESSION_RENDERER
#define MINT_RENDERING_MATH_EXPRESSION_RENDERER


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/StringUtil.h>

#include <MintRenderingBase/Include/ShapeFontRendererContext.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;
        struct LatexParser;
        class MathExpressionRenderer;


        class MathExpression
        {
            friend LatexParser;
            friend MathExpressionRenderer;

        public:
            enum class ModifierType
            {
                Italic, // default
                Bold,
                BoldItalic,
                Plain,
                COUNT
            };

            MINT_INLINE static constexpr uint32 getModifierTypeCount() { return static_cast<uint32>(MathExpression::ModifierType::COUNT); }

            struct Modifier
            {
                ModifierType _type = ModifierType::Italic;
                StringRange _range;
            };

        public:
                                        MathExpression(std::wstring latexString);
                                        ~MathExpression() = default;

        public:
            const wchar_t* const        getPlainString() const noexcept;
            const uint32                getPlainStringLength() const noexcept;

        private:
            std::wstring                _latexExpression;

        private:
            mint::Vector<Modifier>      _modifiers;
            std::wstring                _plainString;
        };


        class MathExpressionRenderer
        {
        public:
                                MathExpressionRenderer(mint::Rendering::GraphicDevice* const graphicDevice);
                                ~MathExpressionRenderer() = default;

        public:
            void                drawMathExpression(const MathExpression& mathExpression, const mint::Float2& screenPosition) noexcept;

        public:
            void                render() noexcept;

        private:
            static constexpr const char* const              kFontFileNames[MathExpression::getModifierTypeCount()] =
                { "Assets/cmu_s_italic", "Assets/cmu_s_bold", "Assets/cmu_s_bold_italic", "Assets/cmu_s_roman" };

        private:
            mint::Rendering::ShapeFontRendererContext       _shapeFontRendererContexts[MathExpression::getModifierTypeCount()];
            mint::BitVector                                 _bitFlagsArray[MathExpression::getModifierTypeCount()];
        };
    }
}


#endif // !MINT_RENDERING_MATH_EXPRESSION_RENDERER
