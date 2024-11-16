#pragma once


#ifndef _MINT_REFLECTION_REFLECTION_H_
#define _MINT_REFLECTION_REFLECTION_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/String.h>
#include <MintContainer/Include/Vector.h>

#include <MintPlatform/Include/BinaryFile.h>

#include <type_traits>


namespace mint
{
	class BinarySerializer;
	class JSONSerializer;


	class TypeBaseData abstract
	{
	public:
		TypeBaseData();
		virtual ~TypeBaseData() = default;

	public:
		StringA _typeName;
		StringA _declarationName;
		uint32 _size;
		uint32 _alignment;
		uint32 _offset;
		uint32 _arrayItemCount;

	public:
		virtual void Serialize(BinarySerializer& serializer) const noexcept abstract;
		virtual void SerializeValue(BinarySerializer& serializer, const void* const memberPointer, const uint32 ArrayItemCount) const noexcept abstract;
		virtual void SerializeValue(JSONSerializer& serializer, const uint32 depth, const void* const memberPointer, const uint32 ArrayItemCount) const noexcept abstract;

	public:
		virtual bool Deserialize(BinarySerializer& serializer) noexcept abstract;
		virtual void DeserializeValue(BinarySerializer& serializer, void* const memberPointer, const uint32 ArrayItemCount) noexcept abstract;
	};

	template <typename T>
	class TypeData : public TypeBaseData
	{
	public:
		TypeData() = default;
		virtual ~TypeData() = default;

	public:
		virtual void Serialize(BinarySerializer& serializer) const noexcept override final;
		virtual void SerializeValue(BinarySerializer& serializer, const void* const memberPointer, const uint32 ArrayItemCount) const noexcept override final;
		virtual void SerializeValue(JSONSerializer& serializer, const uint32 depth, const void* const memberPointer, const uint32 ArrayItemCount) const noexcept override final;

	public:
		virtual bool Deserialize(BinarySerializer& serializer) noexcept override final;
		virtual void DeserializeValue(BinarySerializer& serializer, void* const memberPointer, const uint32 ArrayItemCount) noexcept override final;
	};


	struct ReflectionData
	{
		using BindFunction = void(*)();

	public:
		ReflectionData() : _typeData{ nullptr }
		{
			__noop;
		}

		~ReflectionData()
		{
			const uint32 memberCount = _memberTypeDatas.Size();
			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				MINT_DELETE(_memberTypeDatas[memberIndex]);
			}
			_memberTypeDatas.Clear();

			MINT_DELETE(_typeData);
		}

	public:
		Vector<TypeBaseData*> _memberTypeDatas;
		TypeBaseData* _typeData;
		Vector<BindFunction> _bindFunctions;
	};


#define REFLECTION_CLASS(ClassName)\
	private:\
		void InitializeReflection() noexcept\
		{\
			static bool sIsReflectionDataBuilt = false;\
			if (sIsReflectionDataBuilt == true)\
			{\
				return;\
			}\
			ReflectionData& reflectionData = const_cast<ReflectionData&>(GetReflectionDataStatic());\
			reflectionData._typeData = MINT_NEW(TypeData<ClassName>);\
			reflectionData._typeData->_typeName = #ClassName;\
			reflectionData._typeData->_size = sizeof(ClassName);\
			reflectionData._typeData->_alignment = alignof(ClassName);\
			for (const ReflectionData::BindFunction& bindFunction : reflectionData._bindFunctions )\
			{\
				bindFunction();\
			}\
			reflectionData._bindFunctions.Clear();\
			reflectionData._bindFunctions.ShrinkToFit();\
			sIsReflectionDataBuilt = true;\
		}\
	private:\
		using __ClassType = ClassName;\
	public:\
		virtual const ReflectionData& GetReflectionData() const noexcept\
		{\
			return GetReflectionDataStatic();\
		}\
		static const ReflectionData& GetReflectionDataStatic() noexcept\
		{\
			static const ReflectionData kReflectionData;\
			return kReflectionData;\
		}\
		template<typename T>\
		bool IsTypeOf() const\
		{\
			return GetReflectionData()._typeData->_typeName == T::GetReflectionDataStatic()._typeData->_typeName;\
		}

#define REFLECTION_MEMBER(Type, Name)\
	Type Name;\
	__REFLECTION_MEMBER_DEFINE_REGISTRATION(Type, Name, 0)

#define REFLECTION_MEMBER_ARRAY(Type, Name, ArrayItemCount)\
	Type Name[ArrayItemCount];\
	__REFLECTION_MEMBER_DEFINE_REGISTRATION(Type, Name, ArrayItemCount)

#define REFLECTION_MEMBER_INIT(Type, Name, init)\
	Type Name{ init };\
	__REFLECTION_MEMBER_DEFINE_REGISTRATION(Type, Name, 0)

#define __REFLECTION_MEMBER_DEFINE_REGISTRATION(Type, Name, ArrayItemCount)\
	static void __Bind##Name()\
	{\
		static bool sIsBound = false;\
		if (sIsBound == true)\
		{\
			return;\
		}\
		ReflectionData& reflectionData = const_cast<ReflectionData&>(GetReflectionDataStatic());\
		TypeData<Type>* newTypeData = MINT_NEW(TypeData<Type>);\
		newTypeData->_typeName = #Type;\
		newTypeData->_declarationName = #Name;\
		newTypeData->_size = sizeof(Type);\
		newTypeData->_alignment = alignof(Type);\
		newTypeData->_offset = offsetof(__ClassType, Name);\
		newTypeData->_arrayItemCount = ArrayItemCount;\
		reflectionData._memberTypeDatas.PushBack(newTypeData);\
		reflectionData._bindFunctions.PushBack(__Bind##Name);\
		sIsBound = true;\
	}

	template <typename T, typename = void>
	class IsReflectionClass : public std::false_type {};

	// ### GetReflectionDataStatic() 가 static 멤버 함수인 경우
	template <typename T>
	class IsReflectionClass<T, std::enable_if_t<std::is_function<decltype(T::GetReflectionDataStatic)>::value, void>> : public std::true_type {};

	// ### kReflectionData 가 멤버일 경우
	//template <typename T>
	//class IsReflectionClass<T, std::enable_if_t<std::is_same_v<const ReflectionData, decltype(T::kReflectionData)>, void>> : public std::true_type {};


	class ReflectionTesterInner
	{
		REFLECTION_CLASS(ReflectionTesterInner);

	public:
		ReflectionTesterInner() { InitializeReflection(); }
		~ReflectionTesterInner() = default;

	public:
		REFLECTION_MEMBER_INIT(uint32, _ui, 0xDDCCBBAA);
		REFLECTION_MEMBER_INIT(float, _f, 32.0f);
		REFLECTION_MEMBER_INIT(StringA, _str, "abc");
	};

	class ReflectionTesterOuter
	{
		REFLECTION_CLASS(ReflectionTesterOuter);

	public:
		ReflectionTesterOuter() { InitializeReflection(); }
		~ReflectionTesterOuter() = default;

	public:
		REFLECTION_MEMBER_INIT(uint32, _id, 0xFFFFFFFF);
		REFLECTION_MEMBER(ReflectionTesterInner, _inner);
		REFLECTION_MEMBER(Vector<uint32>, _uis);
	};


	class SerializerScopedDepth
	{
	public:
		SerializerScopedDepth(BinarySerializer* const serializer);
		~SerializerScopedDepth();

	private:
		BinarySerializer* _serializer;
	};


	class BinarySerializer
	{
		template <typename T>
		friend class TypeData;

		friend SerializerScopedDepth;

	public:
		BinarySerializer() = default;
		~BinarySerializer() = default;

	public:
		template <typename T>
		bool Serialize(const T& from, const char* const fileName) noexcept;

	private:
		template <typename T>
		void SerializeInternal(const T& from) noexcept;

		template <typename T>
		void SerializeInternal(const String<T>& from) noexcept;

		template <typename T>
		void SerializeInternal(const Vector<T>& from) noexcept;

	public:
		template <typename T>
		bool Deserialize(const char* const fileName, T& to) noexcept;

	private:
		template <typename T>
		bool DeserializeInternal(T& to) noexcept;

		template <typename T>
		bool DeserializeInternal(String<T>& to) noexcept;

		template <typename T>
		bool DeserializeInternal(Vector<T>& to) noexcept;

	private:
		BinaryFileWriter _writer;
		BinaryFileReader _reader;

	private:
		uint32 _depth = 0;
	};
}


#endif // !_MINT_REFLECTION_REFLECTION_H_
