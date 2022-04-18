#pragma once


#ifndef MINT_LANGUAGE_COMMON_H
#define MINT_LANGUAGE_COMMON_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    namespace Language
    {
        class ILexer;


        enum class SymbolClassifier : uint32
        {
            Delimiter,

            Escaper,

            // ==========

            NumberLiteral,              // [0-9]+[.]*[0-9]*

            Keyword,                    // if, else, class, ...

            GROUPER_BEGINS,
            //
            Grouper_Open,               // () {} []   �� <> �� ����!!!
            Grouper_Close,              // () {} []   �� <> �� ����!!!
            //
            GROUPER_ENDS,

            StringQuote,                // ' "

            StatementTerminator,        // ;

            Punctuator,                 // ,

            OPERATOR_BEGINS,            // �� ��¥�� operator �� ù ���ڵ� �ݵ�� operator ��!?
            //
            AssignmentOperator,
            RelationalOperator,
            ArithmeticOperator,
            LogicalOperator,
            BitwiseOperator,
            MemberAccessOperator,
            OperatorCandiate,
            //
            OPERATOR_ENDS,

            Identifier,                 // [a-zA-Z_][a-zA-Z_0-9]+
            
            StringLiteral,

            // ==========

            SPECIAL_USE,
            POST_CLEARED,

            RESERVED
        };
        static_assert(SymbolClassifier::Delimiter           <    SymbolClassifier::NumberLiteral    );
        static_assert(SymbolClassifier::NumberLiteral       <    SymbolClassifier::Keyword          );
        static_assert(SymbolClassifier::Keyword             <    SymbolClassifier::GROUPER_BEGINS   );
        static_assert(SymbolClassifier::GROUPER_BEGINS      <    SymbolClassifier::GROUPER_ENDS     );
        static_assert(SymbolClassifier::OPERATOR_BEGINS     <    SymbolClassifier::OPERATOR_ENDS    );
        static_assert(SymbolClassifier::OPERATOR_ENDS       <    SymbolClassifier::Identifier       );
        

        class SymbolTableItem
        {
            friend ILexer;

        public:
                                SymbolTableItem();
                                SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString, const uint32 sourceAt);
                                SymbolTableItem(const SymbolClassifier symbolClassifier, const std::string& symbolString);

        public:
            bool                operator==(const SymbolTableItem& rhs) const noexcept;
            bool                operator!=(const SymbolTableItem& rhs) const noexcept;

        public:
            void                clearData();
            uint32              index() const noexcept;

        public:
            SymbolClassifier    _symbolClassifier;
            std::string         _symbolString;

        private:
            uint32              _symbolIndex;
    
        public:
            uint32              _sourceAt;
        };
    }
}


#include <MintLanguage/Include/LanguageCommon.inl>


#endif // !MINT_LANGUAGE_COMMON_H
