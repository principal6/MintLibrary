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
                setEscaper('\\');
                setParsePlainEscaper(true);
                setDefaultSymbolClassifier(Language::SymbolClassifier::StringLiteral);

                registerDelimiter(' ');
                registerDelimiter('\t');
                registerDelimiter('\r');
                registerDelimiter('\n');

                registerGrouper('(', ')');
                registerGrouper('{', '}');
                registerGrouper('[', ']');

                registerPunctuator(",");

                registerKeyword("bold");
                registerKeyword("begin");
                registerKeyword("end");
            }

            virtual const bool execute() noexcept override final
            {
                return __super::executeDefault();
            }
        };


        // TODO: IParser 상속해서 제대로 만들어 보기...
        struct LatexParser
        {
            using StringType = std::wstring;
            static constexpr size_t StringTypeNpos = std::wstring::npos;

            static constexpr wchar_t    kEscapeCh = L'\\';
            static constexpr wchar_t    kEscapeOpenCh = L'{';
            static constexpr wchar_t    kEscapeCloseCh = L'}';
            static constexpr uint32     kEscapeLengthBase = 2; // \\ + {

            static void parse(MathExpression& mathExpression)
            {
                const StringType& latexString = mathExpression._latexExpression;
                
                size_t nextModifierBeginAt = 0;
                size_t at = 0;
                while (at < latexString.size())
                {
                    const size_t plainStringLength = mathExpression._plainString.length();
                    const wchar_t ch = latexString.at(at);
                    if (ch == kEscapeCh)
                    {
                        const size_t escapeOpenAt = latexString.find(kEscapeOpenCh, at + 1);
                        if (escapeOpenAt == StringTypeNpos)
                        {
                            MINT_LOG_ERROR("%d 이후 잘못된 형식이 나왔습니다!", at);
                            break;
                        }

                        const size_t escapeCloseAt = latexString.find(kEscapeCloseCh, escapeOpenAt + 1);
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
                            mathExpression._modifiers.push_back(italicModifier);
                        }
                        
                        const StringRange modifierStringRange{ at + 1, escapeOpenAt - at - 1 };
                        StringType modifierString = latexString.substr(modifierStringRange._offset, modifierStringRange._length);
                        const uint32 modifierStringLength = static_cast<uint32>(modifierString.size());
                        if (modifierString == L"bold")
                        {
                            const StringRange innerStringRange{ escapeOpenAt + 1, escapeCloseAt - escapeOpenAt - 1 };

                            mathExpression._plainString += latexString.substr(innerStringRange._offset, innerStringRange._length);

                            MathExpression::Modifier modifier;
                            modifier._type = MathExpression::ModifierType::Bold;
                            modifier._range = StringRange(plainStringLength, innerStringRange._length);
                            mathExpression._modifiers.push_back(modifier);

                            nextModifierBeginAt = modifier._range._offset + modifier._range._length;

                            at += (escapeCloseAt - at + 1);
                        }
                        else
                        {
                            std::string modifierStr;
                            StringUtil::convertWideStringToString(modifierString, modifierStr);
                            MINT_LOG_ERROR("아직 지원되지 않는 modifier 입니다! [%s]", modifierStr.c_str());
                        }
                    }
                    else
                    {
                        mathExpression._plainString += ch;

                        ++at;
                    }
                }

                if (nextModifierBeginAt < mathExpression._plainString.length() - 1)
                {
                    MathExpression::Modifier italicModifier;
                    italicModifier._type = MathExpression::ModifierType::Italic;
                    italicModifier._range = StringRange(nextModifierBeginAt, mathExpression._plainString.length() - nextModifierBeginAt);
                    mathExpression._modifiers.push_back(italicModifier);
                }
            }
        };

        MathExpression::MathExpression(std::wstring latexString)
            : _latexExpression{ latexString }
            , _isEvaluated{ false }
        {
            __noop;
        }

        void MathExpression::evaluate() const noexcept
        {
            if (isEvaluated() == true)
            {
                return;
            }

            LatexLexer lexer;
            std::string latexExpressionStr;
            StringUtil::convertWideStringToString(_latexExpression, latexExpressionStr);
            lexer.setSource(latexExpressionStr);
            lexer.execute();

            LatexParser::parse(const_cast<MathExpression&>(*this));

            _isEvaluated = true;
        }

        const wchar_t* const MathExpression::getPlainString() const noexcept
        {
            return _plainString.c_str();
        }

        const uint32 MathExpression::getPlainStringLength() const noexcept
        {
            return static_cast<uint32>(_plainString.length());
        }


        MathExpressionRenderer::MathExpressionRenderer(GraphicDevice& graphicDevice)
            : _shapeFontRendererContexts{ graphicDevice, graphicDevice, graphicDevice, graphicDevice }
        {
            const char* const kFontFileNames[MathExpression::getModifierTypeCount()] =
                { Path::makeAssetPath("cmu_s_italic"), Path::makeAssetPath("cmu_s_bold"), Path::makeAssetPath("cmu_s_bold_italic"), Path::makeAssetPath("cmu_s_roman") };

            FontLoader fontLoader;
            for (uint32 modifierTypeIndex = 0; modifierTypeIndex < MathExpression::getModifierTypeCount(); ++modifierTypeIndex)
            {
                ShapeRendererContext& rendererContext = _shapeFontRendererContexts[modifierTypeIndex];
                const char* const kFontFileName = kFontFileNames[modifierTypeIndex];
                if (FontLoader::doesExistFont(kFontFileName) == false)
                {
                    fontLoader.pushGlyphRange(GlyphRange(0, 0x33DD));
                    fontLoader.bakeFontData(kFontFileName, 32, kFontFileName, 2048, 1, 1);
                }
                fontLoader.loadFont(kFontFileName, graphicDevice);

                rendererContext.initializeFontData(fontLoader.getFontData());
                rendererContext.initializeShaders();
                rendererContext.setTextColor(Color::kBlack);
            }
        }

        MathExpressionRenderer::~MathExpressionRenderer()
        {
            __noop;
        }

        void MathExpressionRenderer::drawMathExpression(const MathExpression& mathExpression, const Float2& screenPosition) noexcept
        {
            mathExpression.evaluate();

            const uint32 plainStringLength = mathExpression.getPlainStringLength();
            for (uint32 modifierTypeIndex = 0; modifierTypeIndex < MathExpression::getModifierTypeCount(); ++modifierTypeIndex)
            {
                BitVector& bitFlags = _bitFlagsArray[modifierTypeIndex];
                bitFlags.resizeBitCount(plainStringLength);
                bitFlags.fill(false);
            }

            const uint32 modifierCount = mathExpression._modifiers.size();
            for (uint32 modifierIndex = 0; modifierIndex < modifierCount; modifierIndex++)
            {
                const MathExpression::Modifier& modifier = mathExpression._modifiers[modifierIndex];
                const uint32 modifierTypeIndex = static_cast<uint32>(modifier._type);
                
                BitVector& bitFlags = _bitFlagsArray[modifierTypeIndex];
                for (uint32 iter = 0; iter < modifier._range._length; iter++)
                {
                    const uint32 at = modifier._range._offset + iter;
                    bitFlags.set(at, true);
                }
            }
            
            for (uint32 modifierTypeIndex = 0; modifierTypeIndex < MathExpression::getModifierTypeCount(); ++modifierTypeIndex)
            {
                ShapeRendererContext& rendererContext = _shapeFontRendererContexts[modifierTypeIndex];

                rendererContext.drawDynamicTextBitFlagged(mathExpression.getPlainString(), Float4(screenPosition._x, screenPosition._y, 0.0f, 1.0f),
                    FontRenderingOption(TextRenderDirectionHorz::Rightward, TextRenderDirectionVert::Downward), _bitFlagsArray[modifierTypeIndex]);
            }            
        }

        void MathExpressionRenderer::render() noexcept
        {
            for (uint32 modifierTypeIndex = 0; modifierTypeIndex < MathExpression::getModifierTypeCount(); ++modifierTypeIndex)
            {
                ShapeRendererContext& rendererContext = _shapeFontRendererContexts[modifierTypeIndex];
                rendererContext.render();
                rendererContext.flush();
            }
        }
    }
}
