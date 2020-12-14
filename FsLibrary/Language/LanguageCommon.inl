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
			return _symbolIndex == rhs._symbolIndex;
		}

		FS_INLINE const bool SymbolTableItem::operator!=(const SymbolTableItem& rhs) const noexcept
		{
			return _symbolIndex != rhs._symbolIndex;
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
			, _syntaxAdditionalInfo{ 0 }
		{
			__noop;
		}

		inline SyntaxTreeItem::SyntaxTreeItem(const SymbolTableItem& symbolTableItem)
			: _syntaxClassifier{ kUint32Max }
			, _syntaxAdditionalInfo{ 0 }
			, _symbolTableItem{ symbolTableItem }
		{
			__noop;
		}

		inline SyntaxTreeItem::SyntaxTreeItem(const SymbolTableItem& symbolTableItem, const SyntaxClassifierEnumType syntaxClassifier)
			: _syntaxClassifier{ syntaxClassifier }
			, _syntaxAdditionalInfo{ 0 }
			, _symbolTableItem{ symbolTableItem }
		{
			__noop;
		}

		inline SyntaxTreeItem::SyntaxTreeItem(const SymbolTableItem& symbolTableItem, const SyntaxClassifierEnumType syntaxClassifier, const SyntaxAdditionalInfoType syntaxAdditionalInfo)
			: _syntaxClassifier{ syntaxClassifier }
			, _syntaxAdditionalInfo{ syntaxAdditionalInfo }
			, _symbolTableItem{ symbolTableItem }
		{
			__noop;
		}

		FS_INLINE void SyntaxTreeItem::setAdditionalInfo(const SyntaxAdditionalInfoType syntaxAdditionalInfo)
		{
			_syntaxAdditionalInfo = syntaxAdditionalInfo;
		}
		
		FS_INLINE const SyntaxClassifierEnumType SyntaxTreeItem::getSyntaxClassifier() const noexcept
		{
			return _syntaxClassifier;
		}
		
		FS_INLINE const SyntaxAdditionalInfoType SyntaxTreeItem::getAdditionalInfo() const noexcept
		{
			return _syntaxAdditionalInfo;
		}
	}
}

