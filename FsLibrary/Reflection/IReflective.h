#pragma once


#ifndef FS_IREFLECTIVE_H
#define FS_IREFLECTIVE_H


#include <CommonDefinitions.h>


namespace fs
{
	class ReflectionPool;


	struct ReflectionTypeData
	{
		friend ReflectionPool;

	public:
												ReflectionTypeData() : _byteOffset{ 0 }, _typeSize{ 0 }, _compactTypeSize{ 0 }, _hashCode{ 0 }, _isReflective{ false }, _isRegisterDone{ false } {}
		explicit								ReflectionTypeData(const std::type_info& type) : ReflectionTypeData()
		{
			_typeName = _fullTypeName = type.name();

			const uint32 found = _typeName.rfind("::");
			if (kStringNPos != found)
			{
				_typeName = _typeName.substr(found + 2);
			}

			_hashCode = type.hash_code();
		}

	public:
		const bool								operator==(const std::type_info& type) const noexcept
		{
			return _fullTypeName == type.name();
		}

	public:
		FS_INLINE const uint32					byteOffset() const noexcept { return _byteOffset; }
		FS_INLINE const uint32					typeSize() const noexcept { return _typeSize; }
		FS_INLINE const uint32					compactTypeSize() const noexcept { return _compactTypeSize; }
		FS_INLINE const size_t					hashCode() const noexcept { return _hashCode; }
		FS_INLINE const fs::DynamicStringA&		typeName() const noexcept { return _typeName; }
		FS_INLINE const fs::DynamicStringA&		declarationName() const noexcept { return _declarationName; }
		FS_INLINE const ReflectionTypeData&		member(const uint32 index) const noexcept { return _memberArray[index]; }

	private:
		uint32									_byteOffset;
		uint32									_typeSize;
		uint32									_compactTypeSize;
		size_t									_hashCode;

	private:
		bool									_isReflective; // 멤버 중엔 Reflective 하지 않은 자료형이 있을 수 있다.
		bool									_isRegisterDone;
		fs::DynamicStringA						_fullTypeName;

	private:
		fs::DynamicStringA						_typeName;
		fs::DynamicStringA						_declarationName;
		std::vector<ReflectionTypeData>			_memberArray;
	};


	class IReflective;
	class ReflectionPool final
	{
		friend IReflective;

	private:
												ReflectionPool() = default;
												~ReflectionPool() = default;

	private:
		static uint32							registerType(const std::type_info& type, const size_t byteOffset, const size_t typeSize, const std::type_info& memberType, const fs::DynamicStringA& memberName, const size_t memberSize);
		static void								registerTypeDone(const uint32 typeIndex);
		static const ReflectionTypeData&		getTypeData(const uint32 typeIndex);

	private:
		static ReflectionPool&					getInstance();

	private:
		std::vector<ReflectionTypeData>			_typeArray;
	};


#define FS_DECLARE_REFLECTIVE(type, name)			type name; name.setDeclarationName(#name);

#define FS_REFLECTIVE_CTOR(className)				public: className() { registerTypeInfoXXX(); }
#define FS_REFLECTIVE_CTOR_INIT(className, init)	public: className() : init { registerTypeInfoXXX(); }

#define FS_DECLARE_MEMBER(type, name)				public: type name;

#define FS_REGISTER_BEGIN()							protected: virtual void registerTypeInfoXXX() override {
#define FS_REGISTER_MEMBER(name)						__super::registerType(typeid(*this), reinterpret_cast<size_t>(&(reinterpret_cast<decltype(this)>(0))->name), sizeof(*this), typeid(name), #name, sizeof(name));
#define FS_REGISTER_END()							registerTypeDone(); }


	class IReflective abstract
	{
	public:
									IReflective() = default;
		virtual						~IReflective() = default;

	public:
		void						setDeclarationName(const char* const declarationName)
		{
			_declarationName = declarationName;
		}

		const ReflectionTypeData&	getType() const noexcept
		{
			return fs::ReflectionPool::getTypeData(_myTypeIndex);
		}

		byte*						compactOffsetPtr() noexcept
		{
			return (reinterpret_cast<byte*>(this) + compactOffset());
		}

		const uint32				compactOffset() const noexcept
		{
			return fs::ReflectionPool::getTypeData(_myTypeIndex).member(0).byteOffset();
		}

		const uint32				compactSize() const noexcept
		{
			return fs::ReflectionPool::getTypeData(_myTypeIndex).compactTypeSize();
		}

		const uint32				getMemberCount() const noexcept
		{
			return _memberCount;
		}

		const ReflectionTypeData&	getMemberType(const uint32 memberIndex) const noexcept
		{
			return fs::ReflectionPool::getTypeData(_myTypeIndex).member(memberIndex);
		}

	protected:
		void						registerType(const std::type_info& type, const size_t byteOffset, const size_t typeSize, const std::type_info& memberType, const fs::DynamicStringA& memberName, const size_t memberSize)
		{
			_myTypeIndex = fs::ReflectionPool::registerType(type, byteOffset, typeSize, memberType, memberName, memberSize);
			++_memberCount;
		}

		void						registerTypeDone()
		{
			fs::ReflectionPool::registerTypeDone(_myTypeIndex);
		}

	protected:
		virtual void				registerTypeInfoXXX() abstract;

	protected:
		uint32						_myTypeIndex{ 0 };
		uint32						_memberCount{ 0 };
		const char*					_declarationName{ nullptr };
	};
}


#endif // !FS_IREFLECTIVE_H
