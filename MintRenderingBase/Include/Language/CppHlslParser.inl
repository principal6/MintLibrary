#pragma once


namespace mint
{
    namespace Language
    {
        MINT_INLINE const CppHlslSubInfo_AccessModifier convertStringToCppClassStructAccessModifier(const std::string& input)
        {
            if (input == "public")
            {
                return CppHlslSubInfo_AccessModifier::Public;
            }
            else if (input == "protected")
            {
                return CppHlslSubInfo_AccessModifier::Protected;
            }
            return CppHlslSubInfo_AccessModifier::Private;
        }


        inline CppHlslTypeInfo::CppHlslTypeInfo()
            : _isBuiltIn{ false }
            , _registerIndex{ kInvalidRegisterIndex }
            , _size{ 0 }
            , _byteOffset{ 0 }
        {
            __noop;
        }

        MINT_INLINE void CppHlslTypeInfo::setDefaultInfo(const bool isBuiltIn, const std::string& typeName)
        {
            _isBuiltIn = isBuiltIn;
            _typeName = typeName;
        }

        MINT_INLINE void CppHlslTypeInfo::setDeclName(const std::string& declName)
        {
            _declName = declName;
        }

        MINT_INLINE void CppHlslTypeInfo::setSize(const uint32 size)
        {
            _size = size;
        }

        MINT_INLINE void CppHlslTypeInfo::setByteOffset(const uint32 byteOffset)
        {
            _byteOffset = byteOffset;
        }

        MINT_INLINE void CppHlslTypeInfo::setSemanticName(const std::string& semanticName)
        {
            _semanticName = semanticName;
        }

        MINT_INLINE void CppHlslTypeInfo::setRegisterIndex(const uint32 registerIndex)
        {
            _registerIndex = registerIndex;
        }

        MINT_INLINE void CppHlslTypeInfo::pushMember(const CppHlslTypeInfo& member)
        {
            _memberArray.push_back(member);
        }

        MINT_INLINE const bool CppHlslTypeInfo::isRegisterIndexValid() const noexcept
        {
            return (_registerIndex != kInvalidRegisterIndex);
        }

        MINT_INLINE const std::string& CppHlslTypeInfo::getTypeName() const noexcept
        {
            return _typeName;
        }

        MINT_INLINE const std::string& CppHlslTypeInfo::getDeclName() const noexcept
        {
            return _declName;
        }

        MINT_INLINE const uint32 CppHlslTypeInfo::getSize() const noexcept
        {
            return _size;
        }

        inline const uint32 CppHlslTypeInfo::getByteOffset() const noexcept
        {
            return _byteOffset;
        }

        MINT_INLINE const std::string& CppHlslTypeInfo::getSemanticName() const noexcept
        {
            return _semanticName;
        }

        MINT_INLINE const uint32 CppHlslTypeInfo::getRegisterIndex() const noexcept
        {
            return _registerIndex;
        }

        MINT_INLINE const uint32 CppHlslTypeInfo::getMemberCount() const noexcept
        {
            return static_cast<uint32>(_memberArray.size());
        }

        MINT_INLINE const CppHlslTypeInfo& CppHlslTypeInfo::getMember(const uint32 memberIndex) const noexcept
        {
            return (memberIndex < getMemberCount()) ? _memberArray[memberIndex] : kInvalidTypeInfo;
        }


        inline CppHlslTypeTableItem::CppHlslTypeTableItem(const SymbolTableItem& typeSymbol, const CppHlslMainInfo_TypeModifierFlags& typeModifierFlags)
            : _typeSymbol{ typeSymbol }
            , _typeModifierFlags{ typeModifierFlags }
            , _userDefinedTypeInfo{ 0 }
            , _typeSize{ 0 }
        {
            __noop;
        }

        inline CppHlslTypeTableItem::CppHlslTypeTableItem(const SymbolTableItem& typeSymbol, const CppHlslUserDefinedTypeInfo& userDefinedTypeInfo)
            : _typeSymbol{ typeSymbol }
            , _typeModifierFlags{ 0 }
            , _userDefinedTypeInfo{ userDefinedTypeInfo }
            , _typeSize{ 0 }
        {
            __noop;
        }

        inline CppHlslTypeTableItem::CppHlslTypeTableItem(const SyntaxTreeItem& typeSyntax)
            : CppHlslTypeTableItem(typeSyntax._symbolTableItem, static_cast<CppHlslMainInfo_TypeModifierFlags>(typeSyntax.getMainInfo()))
        {
            __noop;
        }

        MINT_INLINE const std::string& CppHlslTypeTableItem::getTypeName() const noexcept
        {
            return _typeSymbol._symbolString;
        }

        MINT_INLINE void CppHlslTypeTableItem::setTypeSize(const uint32 typeSize) noexcept
        {
            _typeSize = typeSize;
        }

        MINT_INLINE const uint32 CppHlslTypeTableItem::getTypeSize() const noexcept
        {
            return _typeSize;
        }


        MINT_INLINE const CppHlslMainInfo_TypeModifierFlags CppHlslTypeModifierSet::getTypeModifierFlags() const noexcept
        {
            const CppHlslMainInfo_TypeModifierFlags longFlags =
                (0 == _longState)
                ? CppHlslMainInfo_TypeModifierFlags::NONE
                : (1 == _longState)
                  ? CppHlslMainInfo_TypeModifierFlags::Long
                  : CppHlslMainInfo_TypeModifierFlags::LongLong;

            const CppHlslMainInfo_TypeModifierFlags signFlags
                = static_cast<CppHlslMainInfo_TypeModifierFlags>(static_cast<int32>(_signState / 2) * static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Unsigned));

            return static_cast<CppHlslMainInfo_TypeModifierFlags>(
                static_cast<int>(_isConst)          * static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Const       ) |
                static_cast<int>(_isConstexpr)      * static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Constexpr   ) |
                static_cast<int>(_isMutable)        * static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Mutable     ) |
                static_cast<int>(_isStatic)         * static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Static      ) |
                static_cast<int>(_isThreadLocal)    * static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::ThreadLocal ) |
                static_cast<int>(_isShort)          * static_cast<int32>(CppHlslMainInfo_TypeModifierFlags::Short       ) |
                static_cast<int32>(longFlags) | static_cast<int32>(signFlags)
                );
        }


        inline const CppHlslSyntaxClassifier CppHlslParser::convertSymbolToAccessModifierSyntax(const SymbolTableItem& symbol) noexcept
        {
            if (symbol._symbolString == "public" || symbol._symbolString == "protected" || symbol._symbolString == "private")
            {
                return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_ClassStruct_AccessModifier;
            }

            return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_INVALID;
        }

        inline const CppHlslSyntaxClassifier CppHlslParser::convertLiteralSymbolToSyntax(const SymbolTableItem& symbol) noexcept
        {
            if (symbol._symbolClassifier == SymbolClassifier::NumberLiteral)
            {
                return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Literal_Number;
            }
            else if (symbol._symbolClassifier == SymbolClassifier::Keyword)
            {
                if (symbol._symbolString == "true" || symbol._symbolString == "false")
                {
                    return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Literal_TrueFalse;
                }
                else if (symbol._symbolString == "nullptr")
                {
                    return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Literal_Nullptr;
                }
            }
            else if (symbol._symbolClassifier == SymbolClassifier::StringLiteral)
            {
                return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_Literal_String;
            }

            return CppHlslSyntaxClassifier::CppHlslSyntaxClassifier_INVALID;
        }
    }
}
