#pragma once


#ifndef _MINT_LANGUAGE_I_PARSER_H_
#define _MINT_LANGUAGE_I_PARSER_H_


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
			ClassifierType _classifier;
			StringA _Identifier;
			StringA _value;
		};


		template <typename CustomDataType>
		class TypeMetaData final
		{
		public:
			TypeMetaData();
			~TypeMetaData() = default;

		public:
			static const TypeMetaData& GetInvalid() noexcept;

		public:
			void SetBaseData(const StringA& typeName, const bool isBuiltIn);
			void SetDeclName(const StringA& declName);
			void SetSize(const uint32 size);
			void SetByteOffset(const uint32 byteOffset);
			void PushMember(const TypeMetaData& member);

		public:
			bool IsBuiltIn() const noexcept;

		public:
			const StringA& GetTypeName() const noexcept;
			const StringA& GetDeclName() const noexcept;
			uint32 GetSize() const noexcept;
			uint32 GetByteOffset() const noexcept;
			uint32 GetMemberCount() const noexcept;
			const TypeMetaData& GetMember(const uint32 memberIndex) const noexcept;

		private:
			bool _isBuiltIn;
			StringA _typeName; // namespace + name
			StringA _declName;
			uint32 _size; // Byte count
			uint32 _byteOffset;
			Vector<TypeMetaData> _memberArray; // Member variables

		public:
			CustomDataType _customData;
		};


		template <typename TypeCustomDataType, typename SyntaxClassifierType>
		class IParser abstract
		{
		protected:
			using SyntaxTreeNodeData = SyntaxTreeItem<SyntaxClassifierType>;
			using SyntaxTreeNode = TreeNodeAccessor<SyntaxTreeNodeData>;
			using SyntaxTree = Tree<SyntaxTreeNodeData>;

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
				{ "Grouper mismatch" , "열고 닫는 기호가 서로 일치하지 않습니다." },
				{ "No matching grouper" , "여는 기호만 있고 닫는 기호가 없습니다." },
				{ "Wrong predecessor" , "앞에 온 것이 문법적으로 맞지 않습니다." },
				{ "Wrong successor" , "뒤에 온 것이 문법적으로 맞지 않습니다." },
				{ "Lack of code" , "더 있어야 할 코드가 없습니다." },
				{ "Repetition of code" , "코드가 중복됩니다." },
				{ "Symbol not found" , "해당 심볼을 찾을 수 없습니다." },
				{ "Wrong scope" , "이곳에 사용할 수 없습니다." },
			};

		protected:
			static_assert(static_cast<uint32>(ErrorType::COUNT) == ARRAYSIZE(kErrorTypeStringArray));
			static constexpr const char* ConvertErrorTypeToTypeString(const ErrorType errorType);
			static constexpr const char* ConvertErrorTypeToContentString(const ErrorType errorType);

			class ErrorMessage
			{
			public:
				ErrorMessage();
				ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
				ErrorMessage(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);

			private:
				const uint32 _sourceAt;
				StringA _message;
			};

		public:
			IParser(ILexer& lexer);
			virtual ~IParser() = default;

		protected:
			void RegisterTypeInternal(const StringA& typeFullName, const uint32 typeSize, const bool IsBuiltIn = false) noexcept;

		public:
			virtual bool Execute() abstract;

		protected:
			void Reset();

		protected:
			bool ContinuesParsing() const noexcept;

		protected:
			void AdvanceSymbolPositionXXX(const uint32 advanceCount);

		protected:
			bool HasSymbol(const uint32 symbolPosition) const noexcept;
			uint32 GetSymbolPosition() const noexcept;
			SymbolTableItem& GetSymbol(const uint32 symbolPosition) const noexcept;

		protected:
			bool FindNextSymbol(const uint32 symbolPosition, const char* const cmp, uint32& outSymbolPosition) const noexcept;
			bool FindNextSymbol(const uint32 symbolPosition, const SymbolClassifier symbolClassifier, uint32& outSymbolPosition) const noexcept;
			bool FindNextSymbolEither(const uint32 symbolPosition, const char* const cmp0, const char* const cmp1, uint32& outSymbolPosition) const noexcept;
			bool FindNextDepthMatchingGrouperCloseSymbol(const uint32 openSymbolPosition, uint32* const outCloseSymbolPosition = nullptr) const noexcept;

		protected:
			void ReportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType);
			void ReportError(const SymbolTableItem& symbolTableItem, const ErrorType errorType, const char* const additionalExplanation);
			bool HasReportedErrors() const noexcept;

		public:
			uint32 GetTypeMetaDataCount() const noexcept;
			const TypeMetaData<TypeCustomDataType>& GetTypeMetaData(const StringA& typeName) const noexcept;
			const TypeMetaData<TypeCustomDataType>& GetTypeMetaData(const int32 typeIndex) const noexcept;

		protected:
			bool ExistsTypeMetaData(const StringA& typeName) const noexcept;
			void PushTypeMetaData(const StringA& typeName, const TypeMetaData<TypeCustomDataType>& typeMetaData) noexcept;
			TypeMetaData<TypeCustomDataType>& AccessTypeMetaData(const StringA& typeName) noexcept;

		protected:
			ILexer& _lexer;
			Vector<SymbolTableItem>& _symbolTable;

		private:
			uint32 _symbolAt;
			Vector<ErrorMessage> _errorMessageArray;

		protected:
			SyntaxTree _syntaxTree;

		protected:
			Vector<TypeMetaData<TypeCustomDataType>> _typeMetaDatas;
			HashMap<StringA, uint32> _typeMetaDataMap;

		protected:
			HashMap<StringA, uint32> _builtInTypeUmap;
		};
	}
}


#include <MintLanguage/Include/IParser.inl>


#endif // !_MINT_LANGUAGE_I_PARSER_H_
