#pragma once


#ifndef MINT_RENDERING_MATH_EXPRESSION_RENDERER
#define MINT_RENDERING_MATH_EXPRESSION_RENDERER


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/StringUtil.h>

#include <MintRenderingBase/Include/ShapeRendererContext.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;
        struct LatexParser;
        class MathExpressionRenderer;
        class LatexLexer;


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
            void                        evaluate() const noexcept;
            MINT_INLINE const bool      isEvaluated() const noexcept { return _isEvaluated; }

        public:
            const wchar_t* const        getPlainString() const noexcept;
            const uint32                getPlainStringLength() const noexcept;

        private:
            std::wstring                _latexExpression;
            mutable bool                _isEvaluated;

        private:
            Vector<Modifier>            _modifiers;
            std::wstring                _plainString;
        };


        class MathExpressionRenderer
        {
        public:
                        MathExpressionRenderer(GraphicDevice& graphicDevice);
                        ~MathExpressionRenderer();

        public:
            void        drawMathExpression(const MathExpression& mathExpression, const Float2& screenPosition) noexcept;

        public:
            void        render() noexcept;

        private:
            ShapeRendererContext    _shapeFontRendererContexts[MathExpression::getModifierTypeCount()];
            BitVector                   _bitFlagsArray[MathExpression::getModifierTypeCount()];
        };
    }
}


#endif // !MINT_RENDERING_MATH_EXPRESSION_RENDERER
