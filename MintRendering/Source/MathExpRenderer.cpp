#include <MintRendering/Include/MathExpRenderer.h>

#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/StringUtil.hpp>

#include <MintLanguage/Include/ILexer.h>
#include <MintLanguage/Include/IParser.h>


namespace mint
{
	namespace Rendering
	{
		using Language::ILexer;
		using Language::IParser;


		class LatexLexer : public ILexer
		{
		public:
			LatexLexer() : ILexer()
			{
				SetEscaper('\\');
				SetParsePlainEscaper(true);
				SetDefaultSymbolClassifier(Language::SymbolClassifier::StringLiteral);

				RegisterDelimiter(' ');
				RegisterDelimiter('\t');
				RegisterDelimiter('\r');
				RegisterDelimiter('\n');

				RegisterGrouper('(', ')');
				RegisterGrouper('{', '}');
				RegisterGrouper('[', ']');

				RegisterPunctuator(",");

				RegisterKeyword("bold");
				RegisterKeyword("begin");
				RegisterKeyword("end");
			}

			virtual bool Execute() noexcept override final
			{
				return __super::ExecuteDefault();
			}
		};


		// TODO: IParser 상속해서 제대로 만들어 보기...
		struct LatexParser
		{
			using StringType = StringW;
			static constexpr uint32 StringTypeNpos = kStringNPos;

			static constexpr wchar_t kEscapeCh = L'\\';
			static constexpr wchar_t kEscapeOpenCh = L'{';
			static constexpr wchar_t kEscapeCloseCh = L'}';
			static constexpr uint32 kEscapeLengthBase = 2; // \\ + {

			static void parse(MathExpression& mathExpression)
			{
				const StringType& latexString = mathExpression._latexExpression;

				uint32 nextModifierBeginAt = 0;
				uint32 at = 0;
				while (at < latexString.Size())
				{
					const uint32 plainStringLength = mathExpression._plainString.Length();
					const wchar_t ch = latexString.At(at);
					if (ch == kEscapeCh)
					{
						const uint32 escapeOpenAt = latexString.Find(kEscapeOpenCh, at + 1);
						if (escapeOpenAt == StringTypeNpos)
						{
							MINT_LOG_ERROR("%d 이후 잘못된 형식이 나왔습니다!", at);
							break;
						}

						const uint32 escapeCloseAt = latexString.Find(kEscapeCloseCh, escapeOpenAt + 1);
						if (escapeCloseAt == StringTypeNpos)
						{
							MINT_LOG_ERROR("%d 이후 잘못된 형식이 나왔습니다!", escapeOpenAt);
							break;
						}

						if (nextModifierBeginAt < plainStringLength)
						{
							MathExpression::Modifier italicModifier;
							italicModifier._type = MathExpression::ModifierType::Italic;
							italicModifier._range = StringRange(nextModifierBeginAt, plainStringLength - nextModifierBeginAt);
							mathExpression._modifiers.PushBack(italicModifier);
						}

						const StringRange modifierStringRange{ at + 1, escapeOpenAt - at - 1 };
						StringType modifierString = latexString.Substring(modifierStringRange._offset, modifierStringRange._length);
						const uint32 modifierStringLength = static_cast<uint32>(modifierString.Size());
						if (modifierString == L"bold")
						{
							const StringRange innerStringRange{ escapeOpenAt + 1, escapeCloseAt - escapeOpenAt - 1 };

							mathExpression._plainString += latexString.Substring(innerStringRange._offset, innerStringRange._length);

							MathExpression::Modifier modifier;
							modifier._type = MathExpression::ModifierType::Bold;
							modifier._range = StringRange(plainStringLength, innerStringRange._length);
							mathExpression._modifiers.PushBack(modifier);

							nextModifierBeginAt = modifier._range._offset + modifier._range._length;

							at += (escapeCloseAt - at + 1);
						}
						else
						{
							StringA modifierStr;
							StringUtil::ConvertWideStringToString(modifierString, modifierStr);
							MINT_LOG_ERROR("아직 지원되지 않는 modifier 입니다! [%s]", modifierStr.CString());
						}
					}
					else
					{
						mathExpression._plainString += ch;

						++at;
					}
				}

				if (nextModifierBeginAt < mathExpression._plainString.Length() - 1)
				{
					MathExpression::Modifier italicModifier;
					italicModifier._type = MathExpression::ModifierType::Italic;
					italicModifier._range = StringRange(nextModifierBeginAt, mathExpression._plainString.Length() - nextModifierBeginAt);
					mathExpression._modifiers.PushBack(italicModifier);
				}
			}
		};

		MathExpression::MathExpression(StringW latexString)
			: _latexExpression{ latexString }
			, _isEvaluated{ false }
		{
			__noop;
		}

		void MathExpression::Evaluate() const noexcept
		{
			if (IsEvaluated() == true)
			{
				return;
			}

			LatexLexer lexer;
			StringA latexExpressionStr;
			StringUtil::ConvertWideStringToString(_latexExpression, latexExpressionStr);
			lexer.SetSource(latexExpressionStr.CString());
			lexer.Execute();

			LatexParser::parse(const_cast<MathExpression&>(*this));

			_isEvaluated = true;
		}

		const wchar_t* const MathExpression::GetPlainString() const noexcept
		{
			return _plainString.CString();
		}

		uint32 MathExpression::GetPlainStringLength() const noexcept
		{
			return _plainString.Length();
		}


		MathExpressionRenderer::MathExpressionRenderer(GraphicsDevice& graphicsDevice)
			: _shapeRenderers{ graphicsDevice, graphicsDevice, graphicsDevice, graphicsDevice }
		{
			const char* const kFontFileNames[MathExpression::GetModifierTypeCount()] =
			{ Path::MakeAssetPath("cmu_s_italic"), Path::MakeAssetPath("cmu_s_bold"), Path::MakeAssetPath("cmu_s_bold_italic"), Path::MakeAssetPath("cmu_s_roman") };

			FontLoader fontLoader;
			for (uint32 modifierTypeIndex = 0; modifierTypeIndex < MathExpression::GetModifierTypeCount(); ++modifierTypeIndex)
			{
				ShapeRenderer& shapeRenderer = _shapeRenderers[modifierTypeIndex];
				const char* const kFontFileName = kFontFileNames[modifierTypeIndex];
				if (FontLoader::ExistsFont(kFontFileName) == false)
				{
					fontLoader.PushGlyphRange(GlyphRange(0, 0x33DD));
					fontLoader.BakeFontData(kFontFileName, 32, kFontFileName, 2048, 1, 1);
				}
				fontLoader.LoadFont(kFontFileName, graphicsDevice);
				shapeRenderer.InitializeFontData(fontLoader.GetFontData());
				shapeRenderer.InitializeShaders();
				shapeRenderer.SetTextColor(Color::kBlack);
			}
		}

		MathExpressionRenderer::~MathExpressionRenderer()
		{
			__noop;
		}

		void MathExpressionRenderer::DrawMathExpression(const MathExpression& mathExpression, const Float2& screenPosition) noexcept
		{
			mathExpression.Evaluate();

			const uint32 plainStringLength = mathExpression.GetPlainStringLength();
			for (uint32 modifierTypeIndex = 0; modifierTypeIndex < MathExpression::GetModifierTypeCount(); ++modifierTypeIndex)
			{
				BitVector& bitFlags = _bitFlagsArray[modifierTypeIndex];
				bitFlags.ResizeBitCount(plainStringLength);
				bitFlags.Fill(false);
			}

			const uint32 modifierCount = mathExpression._modifiers.Size();
			for (uint32 modifierIndex = 0; modifierIndex < modifierCount; modifierIndex++)
			{
				const MathExpression::Modifier& modifier = mathExpression._modifiers[modifierIndex];
				const uint32 modifierTypeIndex = static_cast<uint32>(modifier._type);

				BitVector& bitFlags = _bitFlagsArray[modifierTypeIndex];
				for (uint32 iter = 0; iter < modifier._range._length; iter++)
				{
					const uint32 at = modifier._range._offset + iter;
					bitFlags.Set(at, true);
				}
			}

			for (uint32 modifierTypeIndex = 0; modifierTypeIndex < MathExpression::GetModifierTypeCount(); ++modifierTypeIndex)
			{
				ShapeRenderer& shapeRenderer = _shapeRenderers[modifierTypeIndex];
				shapeRenderer.DrawDynamicTextBitFlagged(mathExpression.GetPlainString(), Float3(screenPosition._x, screenPosition._y, 0.0f),
					FontRenderingOption(TextRenderDirectionHorz::Rightward, TextRenderDirectionVert::Downward), _bitFlagsArray[modifierTypeIndex]);
			}
		}

		void MathExpressionRenderer::Render() noexcept
		{
			for (uint32 modifierTypeIndex = 0; modifierTypeIndex < MathExpression::GetModifierTypeCount(); ++modifierTypeIndex)
			{
				ShapeRenderer& shapeRenderer = _shapeRenderers[modifierTypeIndex];
				shapeRenderer.Render();
			}
		}
	}
}
