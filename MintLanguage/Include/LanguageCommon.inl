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

		MINT_INLINE bool SymbolTableItem::operator==(const SymbolTableItem& rhs) const noexcept
		{
			return (_symbolIndex == rhs._symbolIndex) && (_symbolString == rhs._symbolString);
		}

		MINT_INLINE bool SymbolTableItem::operator!=(const SymbolTableItem& rhs) const noexcept
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

		MINT_INLINE uint32 SymbolTableItem::index() const noexcept
		{
			return _symbolIndex;
		}
	}
}
