#pragma once


#ifndef FS_I_LEXER_H
#define FS_I_LEXER_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsContainer/Include/Vector.h>

#include <FsRenderingBase/Include/Language/LanguageCommon.h>


namespace fs
{
    namespace Language
    {
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

        static_assert(static_cast<uint32>(SymbolClassifier::OPERATOR_ENDS) - (static_cast<uint32>(SymbolClassifier::OPERATOR_BEGINS) + 1) == static_cast<uint32>(OperatorClassifier::COUNT));
        static_assert(static_cast<uint32>(SymbolClassifier::GROUPER_ENDS ) - (static_cast<uint32>(SymbolClassifier::GROUPER_BEGINS ) + 1) == static_cast<uint32>(GrouperClassifier::COUNT));
        static constexpr const SymbolClassifier getSymbolClassifierFromOperatorClassifier(const OperatorClassifier operatorClassifier);
        static constexpr const SymbolClassifier getSymbolClassifierFromGrouperClassifier(const GrouperClassifier grouperClassifier);

        struct GrouperTableItem
        {
                                    GrouperTableItem()
                                        : _input{ '\0' }
                                        , _grouperClassifier{ GrouperClassifier::COUNT }
                                    {
                                        __noop;
                                    }
                                    GrouperTableItem(const char input, const GrouperClassifier grouperClassifier)
                                        : _input{ input }
                                        , _grouperClassifier { grouperClassifier }
                                    {
                                        __noop;
                                    }

            char                    _input;
            GrouperClassifier       _grouperClassifier;
        };

        struct OperatorTableItem
        {
                                    OperatorTableItem()
                                        : _length{ 0 }
                                        , _operatorClassifier{ OperatorClassifier::COUNT }
                                    {
                                        __noop;
                                    }
                                    OperatorTableItem(const char* const string, const OperatorClassifier operatorClassifier) 
                                        : _string{ string }
                                        , _operatorClassifier{ operatorClassifier }
                                    {
                                        _length = static_cast<uint32>(_string.length());
                                    }

            std::string             _string;
            uint32                  _length;
            OperatorClassifier      _operatorClassifier;
        };

        struct LineSkipperTableItem
        {
                                    LineSkipperTableItem()
                                        : _length{ 0 }
                                        , _lineSkipperGroupId{ kUint16Max }
                                        , _lineSkipperClassifier{ LineSkipperClassifier::COUNT }
                                    {
                                        __noop;
                                    }

                                    LineSkipperTableItem(const char* const string, const LineSkipperClassifier lineSkipperClassifier, const uint16 lineSkipperGroupId)
                                        : _string{ string }
                                        , _lineSkipperGroupId{ lineSkipperGroupId }
                                        , _lineSkipperClassifier{ lineSkipperClassifier }
                                    {
                                        _length = static_cast<uint32>(_string.length());
                                    }

            static const uint16     getNextGroupId()
            {
                const uint16 result = _lineSkipperNextGroupId;
                ++_lineSkipperNextGroupId;
                return result;
            }

            std::string             _string;
            uint32                  _length;
            uint16                  _lineSkipperGroupId;
            LineSkipperClassifier   _lineSkipperClassifier;

        private:
            static uint16           _lineSkipperNextGroupId;
        };


        // Lexical Analyzer
        // Tokens of lexeme
        // Also known as Scanner
        // Uses STL (vector, string, unordered_map)
        class ILexer
        {
            friend IParser;

        public:
                                                    ILexer();
            virtual                                 ~ILexer() = default;

        public:
            void                                    setSource(const std::string& source);

        protected:
            void                                    setEscaper(const char escaper);
            void                                    setStatementTerminator(const char statementTerminator);
            void                                    registerDelimiter(const char delimiter);
            void                                    registerLineSkipper(const char* const lineSkipperOpen, const char* const lineSkipperClose);
            void                                    registerLineSkipper(const char* const lineSkipper);
            void                                    registerKeyword(const char* const keyword);
            void                                    registerGrouper(const char grouper, const GrouperClassifier grouperClassifier);
            void                                    registerStringQuote(const char stringQuote);
            void                                    registerPunctuator(const char* const punctuator);
            void                                    registerOperator(const char* const operator_, const OperatorClassifier operatorClassifier);

        protected:
            virtual const bool                      execute() abstract;
        
        protected:
            const bool                              continueExecution(const uint32 sourceAt) const noexcept;
            const char                              getCh0(const uint32 sourceAt) const noexcept;
            const char                              getCh1(const uint32 sourceAt) const noexcept;
            const char                              getCh2(const uint32 sourceAt) const noexcept;
            void                                    executeDefault(uint32& prevSourceAt, uint32& sourceAt);
            
        private:
            void                                    advanceExecution(const SymbolClassifier symbolClassifier, const uint32 advance, uint32& prevSourceAt, uint32& sourceAt);
        
        protected:
            void                                    endExecution();
        
        protected:
            void                                    updateSymbolIndex();

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

        public:
            const fs::Vector<SymbolTableItem>&      getSymbolTable() const noexcept;
            const uint32                            getSymbolCount() const noexcept;
            const SymbolTableItem&                  getSymbol(const uint32 symbolIndex) const noexcept;

        protected:
            std::string                             _source;
            uint32                                  _totalTimeMs;

        protected:
            char                                    _escaper;
            char                                    _statementTerminator;

        protected:
            std::unordered_map<char, int8>          _delimiterUmap;

        protected:
            fs::Vector<LineSkipperTableItem>        _lineSkipperTable;
            std::unordered_map<uint64, uint32>      _lineSkipperUmap;

        protected:
            fs::Vector<std::string>                 _keywordTable;
            std::unordered_map<uint64, uint32>      _keywordUmap;

        protected:
            fs::Vector<GrouperTableItem>            _grouperTable;
            std::unordered_map<char, uint32>        _grouperUmap;

        protected:
            std::unordered_map<char, int8>          _stringQuoteUmap;

        protected:
            fs::Vector<std::string>                 _punctuatorTable;
            std::unordered_map<uint64, uint32>      _punctuatorUmap;

        protected:
            fs::Vector<OperatorTableItem>           _operatorTable;
            std::unordered_map<uint64, uint32>      _operatorUmap;

        protected:
            fs::Vector<SymbolTableItem>             _symbolTable;
        };
    }
}


#include <FsRenderingBase/Include/Language/ILexer.inl>


#endif // !FS_I_LEXER_H
