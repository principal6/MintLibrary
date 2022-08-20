#pragma once


#ifndef MINT_LANGUAGE_I_LEXER_H
#define MINT_LANGUAGE_I_LEXER_H


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>

#include <MintLanguage/Include/LanguageCommon.h>


namespace mint
{
    namespace Language
    {
        class ILexer;

        template <typename TypeCustomDataType, typename SyntaxClassifierType>
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
        static constexpr SymbolClassifier           getSymbolClassifierFromOperatorClassifier(const OperatorClassifier operatorClassifier);
        static constexpr SymbolClassifier           getSymbolClassifierFromGrouperClassifier(const GrouperClassifier grouperClassifier);

        struct GrouperTableItem
        {
        public:
                                            GrouperTableItem();
                                            GrouperTableItem(const char input, const GrouperClassifier grouperClassifier);

        public:
            MINT_INLINE GrouperClassifier   getClassifier() const noexcept { return _grouperClassifier; }

        private:
            char                            _input;
            GrouperClassifier               _grouperClassifier;
        };

        class OperatorTableItem
        {
        public:
                                            OperatorTableItem();
                                            OperatorTableItem(const char* const string, const OperatorClassifier operatorClassifier);

        public:
            MINT_INLINE uint32              getLength() const noexcept { return _length; }
            MINT_INLINE OperatorClassifier  getClassifier() const noexcept { return _operatorClassifier; }

        private:
            std::string                     _string;
            uint32                          _length;
            OperatorClassifier              _operatorClassifier;
        };

        class LineSkipperTableItem
        {
        public:
                                                LineSkipperTableItem();
                                                LineSkipperTableItem(const char* const string, const LineSkipperSemantic lineSkipperSemantic, const LineSkipperClassifier lineSkipperClassifier, const uint16 groupID);

        public:
            MINT_INLINE bool                    isSameString(const LineSkipperTableItem& rhs) const noexcept { return _string == rhs._string; }
            MINT_INLINE bool                    isSameGroup(const LineSkipperTableItem& rhs) const noexcept { return _groupID == rhs._groupID; }
            MINT_INLINE bool                    checkSemantic(const LineSkipperSemantic lineSkipperSemantic) const noexcept { return _lineSkipperSemantic == lineSkipperSemantic; }
            MINT_INLINE bool                    checkClassifier(const LineSkipperClassifier lineSkipperClassifier) const noexcept { return _lineSkipperClassifier == lineSkipperClassifier; }
            MINT_INLINE LineSkipperClassifier   getClassifier() const noexcept { return _lineSkipperClassifier; }

        private:
            std::string                         _string;
            uint16                              _groupID;
            LineSkipperSemantic                 _lineSkipperSemantic;
            LineSkipperClassifier               _lineSkipperClassifier;
        };


        // Lexical Analyzer
        // Tokens of lexeme
        // Also known as Scanner
        class ILexer
        {
            template <typename TypeCustomDataType, typename SyntaxClassifierType>
            friend class IParser;

        public:
                                            ILexer();
            virtual                         ~ILexer() = default;

        public:
            void                            setSource(const std::string& source);

        protected:
            void                            setEscaper(const char escaper);
            void                            setStatementTerminator(const char statementTerminator);
            void                            registerDelimiter(const char delimiter);
            void                            registerLineSkipper(const char* const lineSkipperOpen, const char* const lineSkipperClose, const LineSkipperSemantic lineSkipperSemantic);
            void                            registerLineSkipper(const char* const lineSkipper, const LineSkipperSemantic lineSkipperSemantic);
            void                            registerKeyword(const char* const keyword);
            void                            registerGrouper(const char grouperOpen, const char grouperClose);
            void                            registerStringQuote(const char stringQuote);
            void                            registerPunctuator(const char* const punctuator);
            void                            registerOperator(const char* const operator_, const OperatorClassifier operatorClassifier);

        private:
            uint16                          getLineSkipperNextGroupID() noexcept;

        protected:
            virtual bool                    execute() noexcept abstract;
            bool                            executeDefault() noexcept;

        protected:
            bool                            continueExecution(const uint32 sourceAt) const noexcept;
            char                            getCh0(const uint32 sourceAt) const noexcept;
            char                            getCh1(const uint32 sourceAt) const noexcept;
            char                            getCh2(const uint32 sourceAt) const noexcept;
            void                            executeInternalScanning(uint32& prevSourceAt, uint32& sourceAt);
    
        private:
            void                            advanceExecution(const SymbolClassifier symbolClassifier, const uint32 advance, uint32& prevSourceAt, uint32& sourceAt);

        protected:
            void                            endExecution();

        private:
            void                            setStringLiterals();
            void                            setSymbolIndices();

        protected:
            bool                            isDelimiter(const char input) const noexcept;
            bool                            isLineSkipper(const char ch0, const char ch1, LineSkipperTableItem& out) const noexcept;
            bool                            isStatementTerminator(const char input) const noexcept;
            bool                            isGrouper(const char input, GrouperTableItem& out) const noexcept;
            bool                            isStringQuote(const char input) const noexcept;
            bool                            isPunctuator(const char ch0, const char ch1, const char ch2, uint32& outAdvance) const noexcept;
            bool                            isOperator(const char ch0, const char ch1, OperatorTableItem& out) const noexcept;
            bool                            isNumber(const std::string& input) const noexcept;
            bool                            isKeyword(const std::string& input) const noexcept;
            bool                            isEscaper(const char input) const noexcept;
            MINT_INLINE void                setParsePlainEscaper(const bool value) noexcept { _parsePlainEscaper = value; }
            MINT_INLINE bool                parsePlainEscaper() const noexcept { return _parsePlainEscaper; }
            MINT_INLINE void                setDefaultSymbolClassifier(const SymbolClassifier defaultSymbolClassifier) noexcept { _defaultSymbolClassifier = defaultSymbolClassifier; }

        public:
            const Vector<SymbolTableItem>&  getSymbolTable() const noexcept;
            uint32                          getSymbolCount() const noexcept;
            const SymbolTableItem&          getSymbol(const uint32 symbolIndex) const noexcept;
            char                            getGrouperClose(const char grouperOpen) const noexcept;

        protected:
            std::string                     _source;
            uint32                          _totalTimeMs;

        private:
            char                            _escaper;
            char                            _statementTerminator;
            bool                            _parsePlainEscaper;
            SymbolClassifier                _defaultSymbolClassifier;

        protected:
            HashMap<char, int8>             _delimiterUmap;

        protected:
            Vector<LineSkipperTableItem>    _lineSkipperTable;
            HashMap<uint64, uint32>         _lineSkipperUmap;
            uint16                          _lineSkipperNextGroupID;

        protected:
            Vector<std::string>             _keywordTable;
            HashMap<uint64, uint32>         _keywordUmap;

        protected:
            Vector<GrouperTableItem>        _grouperTable;
            HashMap<char, uint32>           _grouperUmap;
            HashMap<char, char>             _grouperOpenToCloseMap;

        protected:
            HashMap<char, int8>             _stringQuoteUmap;

        protected:
            Vector<std::string>             _punctuatorTable;
            HashMap<uint64, uint32>         _punctuatorUmap;

        protected:
            Vector<OperatorTableItem>       _operatorTable;
            HashMap<uint64, uint32>         _operatorUmap;

        protected:
            Vector<SymbolTableItem>         _symbolTable;
        };
    }
}


#include <MintLanguage/Include/ILexer.inl>


#endif // !MINT_LANGUAGE_I_LEXER_H
