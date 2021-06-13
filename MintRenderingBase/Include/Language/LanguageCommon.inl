#pragma once


namespace mint
{
    namespace Language
    {
        inline SymbolTableItem::SymbolTableItem()
            : _symbolClassifier{ SymbolClassifier::Identifier }
            , _symbolIndex{ kUint32Max }
            , _sourceAt{ kUint32Max }
        {
            __noop;
        }

        inline SymbolTableItem::SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString, const uint32 sourceAt)
            : _symbolClassifier{ symbolClassifier }
            , _symbolString{ symbolString }
            , _symbolIndex{ kUint32Max }
            , _sourceAt{ sourceAt }
        {
            __noop;
        }

        inline SymbolTableItem::SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString)
            : _symbolClassifier{ symbolClassifier }
            , _symbolString{ symbolString }
            , _symbolIndex{ kUint32Max }
            , _sourceAt{ kUint32Max }
        {
            __noop;
        }

        MINT_INLINE const bool SymbolTableItem::operator==(const SymbolTableItem& rhs) const noexcept
        {
            return (_symbolIndex == rhs._symbolIndex) && (_symbolString == rhs._symbolString);
        }

        MINT_INLINE const bool SymbolTableItem::operator!=(const SymbolTableItem& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        MINT_INLINE void SymbolTableItem::clearData()
        {
            _symbolClassifier = SymbolClassifier::POST_CLEARED;
            _symbolString.clear();
            _symbolIndex = kUint32Max;
            // _sourceAt 은 그냥 남겨둔다.
        }

        MINT_INLINE const uint32 SymbolTableItem::index() const noexcept
        {
            return _symbolIndex;
        }


        inline SyntaxTreeItem::SyntaxTreeItem()
            : _syntaxClassifier{ kUint32Max }
            , _syntaxMainInfo{ 0 }
            , _syntaxSubInfo{ 0 }
        {
            __noop;
        }

        inline SyntaxTreeItem::SyntaxTreeItem(const SymbolTableItem& symbolTableItem)
            : SyntaxTreeItem()
        {
            _symbolTableItem = symbolTableItem;
        }

        inline SyntaxTreeItem::SyntaxTreeItem(const SymbolTableItem& symbolTableItem, const SyntaxClassifierEnumType syntaxClassifier)
            : SyntaxTreeItem()
        {
            _syntaxClassifier = syntaxClassifier;
            _symbolTableItem = symbolTableItem;
        }

        MINT_INLINE const SyntaxClassifierEnumType SyntaxTreeItem::getSyntaxClassifier() const noexcept
        {
            return _syntaxClassifier;
        }

        MINT_INLINE void SyntaxTreeItem::setMainInfo(const SyntaxMainInfoType syntaxMainInfo)
        {
            _syntaxMainInfo = syntaxMainInfo;
        }

        MINT_INLINE void SyntaxTreeItem::setSubInfo(const SyntaxSubInfoType syntaxSubInfo)
        {
            _syntaxSubInfo = syntaxSubInfo;
        }
        
        MINT_INLINE const SyntaxMainInfoType SyntaxTreeItem::getMainInfo() const noexcept
        {
            return _syntaxMainInfo;
        }

        MINT_INLINE const SyntaxSubInfoType SyntaxTreeItem::getSubInfo() const noexcept
        {
            return _syntaxSubInfo;
        }
    }
}

