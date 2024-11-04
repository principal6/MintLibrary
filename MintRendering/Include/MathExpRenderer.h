#pragma once


#ifndef _MINT_RENDERING_MATH__EXPRESSION_RENDERER
#define _MINT_RENDERING_MATH__EXPRESSION_RENDERER


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/StringUtil.h>
#include <MintContainer/Include/BitVector.h>

#include <MintRenderingBase/Include/ShapeRenderer.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicsDevice;
		class MathExpressionRenderer;
		class LatexLexer;
		struct LatexParser;
	}
}

namespace mint
{
	namespace Rendering
	{
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

			MINT_INLINE static constexpr uint32 GetModifierTypeCount() { return static_cast<uint32>(MathExpression::ModifierType::COUNT); }

			struct Modifier
			{
				ModifierType _type = ModifierType::Italic;
				StringRange _range;
			};

		public:
			MathExpression(StringW latexString);
			~MathExpression() = default;

		public:
			void Evaluate() const noexcept;
			MINT_INLINE bool IsEvaluated() const noexcept { return _isEvaluated; }

		public:
			const wchar_t* const GetPlainString() const noexcept;
			uint32 GetPlainStringLength() const noexcept;

		private:
			StringW _latexExpression;
			mutable bool _isEvaluated;

		private:
			Vector<Modifier> _modifiers;
			StringW _plainString;
		};


		class MathExpressionRenderer
		{
		public:
			MathExpressionRenderer(GraphicsDevice& graphicsDevice);
			~MathExpressionRenderer();

		public:
			void drawMathExpression(const MathExpression& mathExpression, const Float2& screenPosition) noexcept;

		public:
			void Render() noexcept;

		private:
			ShapeRenderer _shapeRenderers[MathExpression::GetModifierTypeCount()];
			BitVector _bitFlagsArray[MathExpression::GetModifierTypeCount()];
		};
	}
}


#endif // !_MINT_RENDERING_MATH__EXPRESSION_RENDERER
