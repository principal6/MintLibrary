#pragma once


#ifndef MINT_LANGUAGE_I_LEXER_H
#define MINT_LANGUAGE_I_LEXER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>

#include <MintLanguage/Include/LanguageCommon.h>


namespace mint
{
    namespace Language
    {
        class ILexer;

        template <typename CustomDataType>
        class IParser;


        enum class GrouperClassifier : uint8
        {
            Open,                   // ( [ {
            Close,                  // ) ] }

            COUNT
        };

        enum class OperatorClassifier : uint8
        {
            AssignmentOperator,     // = += -= *= /= %= &= |= ^=
            RelationalOperator,     // < <= > >= == !=
            ArithmeticOperator,     // + - * / % ++() ()++ --() ()--
            LogicalOperator,        // && || !
            BitwiseOperator,        // & | ^ ~
            MemberAccessOperator,   // .
            OperatorCandiate,       // ? :

            COUNT
        };

        enum class LineSkipperClassifier : uint8
        {
            SingleMarker,
            OpenMarker,
            CloseMarker,
            OpenCloseMarker,
            
            COUNT,
        };

        enum class LineSkipperSemantic : uint8
        {
            Comment,
            Preprocessor,

            COUNT,
        };

        static_assert(static_cast<uint32>(SymbolClassifier::OPERATOR_ENDS) - (static_cast<uint32>(SymbolClassifier::OPERATOR_BEGINS) + 1) == static_cast<uint32>(OperatorClassifier::COUNT));
        static_assert(static_cast<uint32>(SymbolClassifier::GROUPER_ENDS ) - (static_cast<uint32>(SymbolClassifier::GROUPER_BEGINS ) + 1) == static_cast<uint32>(GrouperClassifier::COUNT));
        static constexpr const SymbolClassifier getSymbolClassifierFromOperatorClassifier(const OperatorClassifier operatorClassifier);
        static constexpr const SymbolClassifier getSymbolClassifierFromGrouperClassifier(const GrouperClassifier grouperClassifier);

        struct GrouperTableItem
        {
        public:
                                                    GrouperTableItem();
                                                    GrouperTableItem(const char input, const GrouperClassifier grouperClassifier);

        public:
            MINT_INLINE const GrouperClassifier     getClassifier() const noexcept { return _grouperClassifier; }

        private:
            char                                    _input;
            GrouperClassifier                       _grouperClassifier;
        };

        class OperatorTableItem
        {
        public:
                                                    OperatorTableItem();
                                                    OperatorTableItem(const char* const string, const OperatorClassifier operatorClassifier);

        public:
            MINT_INLINE const uint32                getLength() const noexcept { return _length; }
            MINT_INLINE const OperatorClassifier    getClassifier() const noexcept { return _operatorClassifier; }

        private:
            std::string                             _string;
            uint32                                  _length;
            OperatorClassifier                      _operatorClassifier;
        };

        class LineSkipperTableItem
        {
        public:
                                                    LineSkipperTableItem();
                                                    LineSkipperTableItem(const char* const string, const LineSkipperSemantic lineSkipperSemantic, const LineSkipperClassifier lineSkipperClassifier, const uint16 groupId);

        public:
            MINT_INLINE const bool                  isSameString(const LineSkipperTableItem& rhs) const noexcept { return _string == rhs._string; }
            MINT_INLINE const bool                  isSameGroup(const LineSkipperTableItem& rhs) const noexcept { return _groupId == rhs._groupId; }
            MINT_INLINE const bool                  checkSemantic(const LineSkipperSemantic lineSkipperSemantic) const noexcept { return _lineSkipperSemantic == lineSkipperSemantic; }
            MINT_INLINE const bool                  checkClassifier(const LineSkipperClassifier lineSkipperClassifier) const noexcept { return _lineSkipperClassifier == lineSkipperClassifier; }
            MINT_INLINE const LineSkipperClassifier getClassifier() const noexcept { return _lineSkipperClassifier; }

        private:
            std::string                             _string;
            uint16                                  _groupId;
            LineSkipperSemantic                     _lineSkipperSemantic;
            LineSkipperClassifier                   _lineSkipperClassifier;
        };


        // Lexical Analyzer
        // Tokens of lexeme
        // Also known as Scanner
        class ILexer
        {
            template <typename CustomDataType>
            friend class IParser;

        public:
                                                    ILexer();
            virtual                                 ~ILexer() = default;

        public:
            void                                    setSource(const std::string& source);

        protected:
            void                                    setEscaper(const char escaper);
            void                                    setStatementTerminator(const char statementTerminator);
            void                                    registerDelimiter(const char delimiter);
            void                                    registerLineSkipper(const char* const lineSkipperOpen, const char* const lineSkipperClose, const LineSkipperSemantic lineSkipperSemantic);
            void                                    registerLineSkipper(const char* const lineSkipper, const LineSkipperSemantic lineSkipperSemantic);
            void                                    registerKeyword(const char* const keyword);
            void                                    registerGrouper(const char grouperOpen, const char grouperClose);
            void                                    registerStringQuote(const char stringQuote);
            void                                    registerPunctuator(const char* const punctuator);
            void                                    registerOperator(const char* const operator_, const OperatorClassifier operatorClassifier);

        private:
            const uint16                            getLineSkipperNextGroupId() noexcept;

        protected:
            virtual const bool                      execute() noexcept abstract;
            const bool                              executeDefault() noexcept;
        
        protected:
            const bool                              continueExecution(const uint32 sourceAt) const noexcept;
            const char                              getCh0(const uint32 sourceAt) const noexcept;
            const char                              getCh1(const uint32 sourceAt) const noexcept;
            const char                              getCh2(const uint32 sourceAt) const noexcept;
            void                                    executeInternalScanning(uint32& prevSourceAt, uint32& sourceAt);
            
        private:
            void                                    advanceExecution(const SymbolClassifier symbolClassifier, const uint32 advance, uint32& prevSourceAt, uint32& sourceAt);
        
        protected:
            void                                    endExecution();
        
        private:
            void                                    setStringLiterals();
            void                                    setSymbolIndices();

        protected:
            const bool                              isDelimiter(const char input) const noexcept;
            const bool                              isLineSkipper(const char ch0, const char ch1, LineSkipperTableItem& out) const noexcept;
            const bool                              isStatementTerminator(const char input) const noexcept;
            const bool                              isGrouper(const char input, GrouperTableItem& out) const noexcept;
            const bool                              isStringQuote(const char input) const noexcept;
            const bool                              isPunctuator(const char ch0, const char ch1, const char ch2, uint32& outAdvance) const noexcept;
            const bool                              isOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept;
            const bool                              isNumber(const std::string& input) const noexcept;
            const bool                              isKeyword(const std::string& input) const noexcept;
            const bool                              isEscaper(const char input) const noexcept;
            MINT_INLINE void                        setParsePlainEscaper(const bool value) noexcept { _parsePlainEscaper = value; }
            MINT_INLINE const bool                  parsePlainEscaper() const noexcept { return _parsePlainEscaper; }
            MINT_INLINE void                        setDefaultSymbolClassifier(const SymbolClassifier defaultSymbolClassifier) noexcept { _defaultSymbolClassifier = defaultSymbolClassifier; }

        public:
            const mint::Vector<SymbolTableItem>&    getSymbolTable() const noexcept;
            const uint32                            getSymbolCount() const noexcept;
            const SymbolTableItem&                  getSymbol(const uint32 symbolIndex) const noexcept;
            const char                              getGrouperClose(const char grouperOpen) const noexcept;

        protected:
            std::string                             _source;
            uint32                                  _totalTimeMs;

        private:
            char                                    _escaper;
            char                                    _statementTerminator;
            bool                                    _parsePlainEscaper;
            SymbolClassifier                        _defaultSymbolClassifier;

        protected:
            mint::HashMap<char, int8>               _delimiterUmap;

        protected:
            mint::Vector<LineSkipperTableItem>      _lineSkipperTable;
            mint::HashMap<uint64, uint32>           _lineSkipperUmap;
            uint16                                  _lineSkipperNextGroupId;

        protected:
            mint::Vector<std::string>               _keywordTable;
            mint::HashMap<uint64, uint32>           _keywordUmap;

        protected:
            mint::Vector<GrouperTableItem>          _grouperTable;
            mint::HashMap<char, uint32>             _grouperUmap;
            mint::HashMap<char, char>               _grouperOpenToCloseMap;

        protected:
            mint::HashMap<char, int8>               _stringQuoteUmap;

        protected:
            mint::Vector<std::string>               _punctuatorTable;
            mint::HashMap<uint64, uint32>           _punctuatorUmap;

        protected:
            mint::Vector<OperatorTableItem>         _operatorTable;
            mint::HashMap<uint64, uint32>           _operatorUmap;

        protected:
            mint::Vector<SymbolTableItem>           _symbolTable;
        };
    }
}


#include <MintLanguage/Include/ILexer.inl>


#endif // !MINT_LANGUAGE_I_LEXER_H
