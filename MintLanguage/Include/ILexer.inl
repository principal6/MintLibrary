﻿#pragma once


namespace mint
{
	namespace Language
	{
		MINT_INLINE static constexpr SymbolClassifier GetSymbolClassifierFromOperatorClassifier(const OperatorClassifier operatorClassifier)
		{
			return static_cast<SymbolClassifier>(static_cast<uint32>(SymbolClassifier::OPERATOR_BEGINS) + 1 + static_cast<uint32>(operatorClassifier));
		}

		MINT_INLINE static constexpr SymbolClassifier GetSymbolClassifierFromGrouperClassifier(const GrouperClassifier grouperClassifier)
		{
			return static_cast<SymbolClassifier>(static_cast<uint32>(SymbolClassifier::GROUPER_BEGINS) + 1 + static_cast<uint32>(grouperClassifier));
		}
	}
}
