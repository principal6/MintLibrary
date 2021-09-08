#pragma once


#ifndef MINT_LANGUAGE_I_PARSER_H
#define MINT_LANGUAGE_I_PARSER_H


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/Tree.h>

#include <MintLanguage/Include/LanguageCommon.h>


namespace mint
{
    namespace Language
    {
        class ILexer;


        template <typename ClassifierType>
        struct SyntaxTreeItem
        {
        public:
                                    SyntaxTreeItem();
                                    ~SyntaxTreeItem();

        //private:
            ClassifierType          _classifier;
            std::string             _identifier;
            std::string             _value;
        };


        template <typename CustomDataType>
        class TypeMetaData final
        {
        public:
                                            TypeMetaData();
                                            ~TypeMetaData() = default;

        public:
            static const TypeMetaData&      getInvalid() noexcept;

        public:
            void                            setBaseData(const std::string& typeName, const bool isBuiltIn);
            void                            setDeclName(const std::string& declName);
            void                            setSize(const uint32 size);
            void                            setByteOffset(const uint32 byteOffset);
            void                            pushMember(const TypeMetaData& member);

        public:
            const bool                      isBuiltIn() const noexcept;

        public:
            const std::string&              getTypeName() const noexcept;
            const std::string&              getDeclName() const noexcept;
            const uint32                    getSize() const noexcept;
            const uint32                    getByteOffset() const noexcept;
            const uint32                    getMemberCount() const noexcept;
            const TypeMetaData&             getMember(const uint32 memberIndex) const noexcept;

        private:
            bool                            _isBuiltIn;
            std::string                     _typeName;      // namespace + name
            std::string                     _declName;
            uint32                          _size;          // Byte count
            uint32                          _byteOffset;
            Vector<TypeMetaData>            _memberArray;   // Member variables

        public:
            CustomDataType                  _customData;
        };


        template <typename TypeCustomDataType, typename SyntaxClassifierType>
        class IParser abstract
        {
        protected:
            using SyntaxTreeNodeData        = SyntaxTreeItem<SyntaxClassifierType>;
            using SyntaxTreeNode            = TreeNodeAccessor<SyntaxTreeNodeData>;
            using SyntaxTree                = Tree<SyntaxTreeNodeData>;

        protected:
            enum class ErrorType
            {
                GrouperMismatch,
                NoMatchingGrouper,
                WrongPredecessor,
                WrongSuccessor,
                LackOfCode,
                RepetitionOfCode,
                SymbolNotFound,
                WrongScope,

                COUNT
            };

        private:
            static constexpr const char* const kErrorTypeStringArray[][2]
            {
                { "Grouper mismatch"                    ,  "���� �ݴ� ��ȣ�� ���� ��ġ���� �ʽ��ϴ�."                                      },
                { "No matching grouper"                 ,  "���� ��ȣ�� �ְ� �ݴ� ��ȣ�� �����ϴ�."                                        },
                { "Wrong predecessor"                   ,  "�տ� �� ���� ���������� ���� �ʽ��ϴ�."                                        },
                { "Wrong successor"                     ,  "�ڿ� �� ���� ���������� ���� �ʽ��ϴ�."                                        },
                { "Lack of code"                        ,  "�� �־�� �� �ڵ尡 �����ϴ�."                                                 },
                { "Repetition of code"                  ,  "�ڵ尡 �ߺ��˴ϴ�."                                                            },
                { "Symbol not found"                    ,  "�ش� �ɺ��� ã�� �� �����ϴ�."                                                 },
                { "Wrong scope"                         ,  "�̰��� ����� �� �����ϴ�."                                                    },
            };

        protected:
            static_assert(static_cast<uint32>(ErrorType::COUNT) == ARRAYSIZE(kErrorTypeStringArray));
            static constexpr const char*                        convertErrorTypeToTypeString(const ErrorType errorType);
            static constexpr const char*                        convertErrorTypeToContentString(const ErrorType errorType);

            class ErrorMessage
            {
            public:
                                                                ErrorMessage();
                                                                ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
                                                                ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);

            private:
                const uint32                                    _sourceAt;
                std::string                                     _message;
            };

        public:
                                                                IParser(ILexer& lexer);
            virtual                                             ~IParser() = default;

        protected:
            void                                                registerTypeInternal(const std::string& typeFullName, const uint32 typeSize, const bool isBuiltIn = false) noexcept;

        public:
            virtual const bool                                  execute() abstract;

        protected:
            void                                                reset();

        protected:
            const bool                                          continueParsing() const noexcept;

        protected:
            void                                                advanceSymbolPositionXXX(const uint32 advanceCount);

        protected:
            const bool                                          hasSymbol(const uint32 symbolPosition) const noexcept;
            const uint32                                        getSymbolPosition() const noexcept;
            SymbolTableItem&                                    getSymbol(const uint32 symbolPosition) const noexcept;

        protected:
            const bool                                          findNextSymbol(const uint32 symbolPosition, const char* const cmp, uint32& outSymbolPosition) const noexcept;
            const bool                                          findNextSymbol(const uint32 symbolPosition, const SymbolClassifier symbolClassifier, uint32& outSymbolPosition) const noexcept;
            const bool                                          findNextSymbolEither(const uint32 symbolPosition, const char* const cmp0, const char* const cmp1, uint32& outSymbolPosition) const noexcept;
            const bool                                          findNextDepthMatchingGrouperCloseSymbol(const uint32 openSymbolPosition, uint32* const outCloseSymbolPosition = nullptr) const noexcept;
        
        protected:
            void                                                reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
            void                                                reportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);
            const bool                                          hasReportedErrors() const noexcept;
            
        public:
            const uint32                                        getTypeMetaDataCount() const noexcept;
            const TypeMetaData<TypeCustomDataType>&             getTypeMetaData(const std::string& typeName) const noexcept;
            const TypeMetaData<TypeCustomDataType>&             getTypeMetaData(const int32 typeIndex) const noexcept;

        protected:
            const bool                                          existsTypeMetaData(const std::string& typeName) const noexcept;
            void                                                pushTypeMetaData(const std::string& typeName, const TypeMetaData<TypeCustomDataType>& typeMetaData) noexcept;
            TypeMetaData<TypeCustomDataType>&                   accessTypeMetaData(const std::string& typeName) noexcept;

        protected:
            ILexer&                                             _lexer;
            Vector<SymbolTableItem>&                            _symbolTable;

        private:
            uint32                                              _symbolAt;
            Vector<ErrorMessage>                                _errorMessageArray;

        protected:
            SyntaxTree                                          _syntaxTree;

        protected:
            Vector<TypeMetaData<TypeCustomDataType>>            _typeMetaDatas;
            HashMap<std::string, uint32>                        _typeMetaDataMap;

        protected:
            HashMap<std::string, uint32>                        _builtInTypeUmap;
        };
    }
}


#include <MintLanguage/Include/IParser.inl>


#endif // !MINT_LANGUAGE_I_PARSER_H
