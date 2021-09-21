#pragma once


#ifndef MINT_REFLECTION_REFLECTION_H
#define MINT_REFLECTION_REFLECTION_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/String.h>
#include <MintContainer/Include/Vector.h>

#include <MintPlatform/Include/BinaryFile.h>

#include <type_traits>


namespace mint
{
    class Serializer;


    class TypeBaseData abstract
    {
    public:
                            TypeBaseData();
        virtual             ~TypeBaseData();
    
    public:
        StringA             _typeName;
        StringA             _declarationName;
        uint32              _size;
        uint32              _alignment;
        uint32              _offset;

    public:
        virtual void        serialize(Serializer& serializer) noexcept abstract;
        virtual void        serializeValue(Serializer& serializer, const void* const memberPointer) noexcept abstract;

    public:
        virtual const bool  deserialize(Serializer& serializer) noexcept abstract;
        virtual void        deserializeValue(Serializer& serializer, void* const memberPointer) noexcept abstract;
    };

    template <typename T>
    class TypeData : public TypeBaseData
    {
    public:
                            TypeData();
        virtual             ~TypeData();

    public:
        virtual void        serialize(Serializer& serializer) noexcept override final;
        virtual void        serializeValue(Serializer& serializer, const void* const memberPointer) noexcept override final;

    public:
        virtual const bool  deserialize(Serializer& serializer) noexcept override final;
        virtual void        deserializeValue(Serializer& serializer, void* const memberPointer) noexcept override final;
    };


    struct ReflectionData
    {
    public:
        ReflectionData() : _typeData{ nullptr } 
        {
            __noop; 
        }
        
        ~ReflectionData()
        {
            const uint32 memberCount = _memberTypeDatas.size();
            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                MINT_DELETE(_memberTypeDatas[memberIndex]);
            }
            _memberTypeDatas.clear();

            MINT_DELETE(_typeData);
        }

    public:
        Vector<TypeBaseData*>   _memberTypeDatas;
        TypeBaseData*           _typeData;
    };


#define REFLECTION_CLASS(className) \
    private: \
        void initializeReflection() noexcept \
        { \
            static bool isReflectionDataBuilt = false; \
            if (isReflectionDataBuilt == false) \
            { \
                __buildMemberReflectionData(); \
                ReflectionData& reflectionData = const_cast<ReflectionData&>(getReflectionData()); \
                reflectionData._typeData = MINT_NEW(TypeData<className>); \
                reflectionData._typeData->_typeName = #className; \
                reflectionData._typeData->_size = sizeof(className); \
                reflectionData._typeData->_alignment = alignof(className); \
                isReflectionDataBuilt = true; \
            } \
        } \
    private: \
        using __classType = className; \
    public: \
        static const ReflectionData& getReflectionData() noexcept \
    { \
        static const ReflectionData kReflectionData; \
        return kReflectionData; \
    }

#define REFLECTION_MEMBER(type, name) \
    type name; \
    __REFLECTION_MEMBER_DEFINE_REGISTRATION(type, name)

#define REFLECTION_MEMBER_INIT(type, name, init) \
    type name{ init }; \
    __REFLECTION_MEMBER_DEFINE_REGISTRATION(type, name)

#define __REFLECTION_MEMBER_DEFINE_REGISTRATION(type, name) \
    void _bind##name()\
    {\
        ReflectionData& reflectionData = const_cast<ReflectionData&>(getReflectionData()); \
        TypeData<type>* newTypeData = MINT_NEW(TypeData<type>);\
        newTypeData->_typeName = #type;\
        newTypeData->_declarationName = #name;\
        newTypeData->_size = sizeof(type);\
        newTypeData->_alignment = alignof(type);\
        newTypeData->_offset = offsetof(__classType, name); \
        reflectionData._memberTypeDatas.push_back(newTypeData);\
    }

#define REFLECTION_BIND_BEGIN private: void __buildMemberReflectionData() {
#define REFLECTION_BIND(name) _bind##name();
#define REFLECTION_BIND_END }

#define REFLECTION_REGISTER(className) const ReflectionData className::getReflectionData(){};


    template <typename T, typename = void>
    class IsReflectionClass : public std::false_type {};
    
    // ### getReflectionData() 가 static 멤버 함수인 경우
    template <typename T>
    class IsReflectionClass<T, std::enable_if_t<std::is_function<decltype(T::getReflectionData)>::value, void>> : public std::true_type {};

    // ### kReflectionData 가 멤버일 경우
    //template <typename T>
    //class IsReflectionClass<T, std::enable_if_t<std::is_same_v<const ReflectionData, decltype(T::kReflectionData)>, void>> : public std::true_type {};


    class ReflectionTesterInner
    {
        REFLECTION_CLASS(ReflectionTesterInner);

    public:
        ReflectionTesterInner() { initializeReflection(); }
        ~ReflectionTesterInner() = default;

    public:
        REFLECTION_MEMBER_INIT(uint32, _ui, 0xDDCCBBAA);
        REFLECTION_MEMBER_INIT(float, _f, 32.0f);
        REFLECTION_MEMBER_INIT(StringA, _str, "abc");

    private:
        REFLECTION_BIND_BEGIN;
            REFLECTION_BIND(_ui);
            REFLECTION_BIND(_f);
            REFLECTION_BIND(_str);
        REFLECTION_BIND_END;
    };

    class ReflectionTesterOuter
    {
        REFLECTION_CLASS(ReflectionTesterOuter);

    public:
        ReflectionTesterOuter() { initializeReflection(); }
        ~ReflectionTesterOuter() = default;

    public:
        REFLECTION_MEMBER_INIT(uint32, _id, 0xFFFFFFFF);
        REFLECTION_MEMBER(ReflectionTesterInner, _inner);
        REFLECTION_MEMBER(Vector<uint32>, _uis);

    private:
        REFLECTION_BIND_BEGIN;
            REFLECTION_BIND(_id);
            REFLECTION_BIND(_inner);
            REFLECTION_BIND(_uis);
        REFLECTION_BIND_END;
    };


    class SerializerScopedDepth
    {
    public:
                        SerializerScopedDepth(Serializer* const serializer);
                        ~SerializerScopedDepth();

    private:
        Serializer*     _serializer;
    };


    class Serializer
    {
        template <typename T>
        friend class TypeData;
        
        friend SerializerScopedDepth;

    public:
                            Serializer() = default;
                            ~Serializer() = default;
    
    public:
        template <typename T>
        const bool          serialize(const T& from, const char* const fileName) noexcept;

    private:
        template <typename T>
        void                _serializeInternal(const T& from, const bool isTypeData) noexcept;
        
        template <typename T>
        void                _serializeInternal(const String<T>& from, const bool isTypeData) noexcept;

        template <typename T>
        void                _serializeInternal(const Vector<T>& from, const bool isTypeData) noexcept;

    public:
        template <typename T>
        const bool          deserialize(const char* const fileName, T& to) noexcept;
        
    private:
        template <typename T>
        const bool          _deserializeInternal(T& to, const bool isTypeData) noexcept;

        template <typename T>
        const bool          _deserializeInternal(String<T>& to, const bool isTypeData) noexcept;

        template <typename T>
        const bool          _deserializeInternal(Vector<T>& to, const bool isTypeData) noexcept;

    private:
        BinaryFileWriter    _writer;
        BinaryFileReader    _reader;
    
    private:
        uint32              _depth = 0;
    };
}


#endif // !MINT_REFLECTION_REFLECTION_H
