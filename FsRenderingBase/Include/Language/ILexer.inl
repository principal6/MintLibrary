#pragma once


namespace fs
{
    namespace Language
    {
        FS_INLINE static constexpr const SymbolClassifier getSymbolClassifierFromOperatorClassifier(const OperatorClassifier operatorClassifier)
        {
            return static_cast<SymbolClassifier>(static_cast<uint32>(SymbolClassifier::OPERATOR_BEGINS) + 1 + static_cast<uint32>(operatorClassifier));
        }

        FS_INLINE static constexpr const SymbolClassifier getSymbolClassifierFromGrouperClassifier(const GrouperClassifier grouperClassifier)
        {
            return static_cast<SymbolClassifier>(static_cast<uint32>(SymbolClassifier::GROUPER_BEGINS ) + 1 + static_cast<uint32>(grouperClassifier));
        }
    }
}
