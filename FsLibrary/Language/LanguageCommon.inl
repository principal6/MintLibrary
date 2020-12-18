#include "LanguageCommon.h"
#pragma once


namespace fs
{
	namespace Language
	{
		inline SymbolTableItem::SymbolTableItem()
			: _symbolClassifier{ SymbolClassifier::Identifier }
			, _symbolIndex{ kUint64Max }
			, _sourceAt{ kUint64Max }
		{
			__noop;
		}

		inline SymbolTableItem::SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString, const uint64 sourceAt)
			: _symbolClassifier{ symbolClassifier }
			, _symbolString{ symbolString }
			, _symbolIndex{ kUint64Max }
			, _sourceAt{ sourceAt }
		{
			__noop;
		}

		inline SymbolTableItem::SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString)
			: _symbolClassifier{ symbolClassifier }
			, _symbolString{ symbolString }
			, _symbolIndex{ kUint64Max }
			, _sourceAt{ kUint64Max }
		{
			__noop;
		}

		FS_INLINE const bool SymbolTableItem::operator==(const SymbolTableItem& rhs) const noexcept
		{
			return (_symbolIndex == rhs._symbolIndex) && (_symbolString == rhs._symbolString);
		}

		FS_INLINE const bool SymbolTableItem::operator!=(const SymbolTableItem& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		FS_INLINE void SymbolTableItem::clearData()
		{
			_symbolClassifier = SymbolClassifier::POST_CLEARED;
			_symbolString.clear();
			_symbolIndex = kUint64Max;
			// _sourceAt 은 그냥 남겨둔다.
		}

		FS_INLINE const uint64 SymbolTableItem::index() const noexcept
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

		FS_INLINE const SyntaxClassifierEnumType SyntaxTreeItem::getSyntaxClassifier() const noexcept
		{
			return _syntaxClassifier;
		}

		FS_INLINE void SyntaxTreeItem::setMainInfo(const SyntaxMainInfoType syntaxMainInfo)
		{
			_syntaxMainInfo = syntaxMainInfo;
		}

		FS_INLINE void SyntaxTreeItem::setSubInfo(const SyntaxSubInfoType syntaxSubInfo)
		{
			_syntaxSubInfo = syntaxSubInfo;
		}
		
		FS_INLINE const SyntaxMainInfoType SyntaxTreeItem::getMainInfo() const noexcept
		{
			return _syntaxMainInfo;
		}

		FS_INLINE const SyntaxSubInfoType SyntaxTreeItem::getSubInfo() const noexcept
		{
			return _syntaxSubInfo;
		}
	}
}

