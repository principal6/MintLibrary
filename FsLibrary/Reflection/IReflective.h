#pragma once


#ifndef FS_IREFLECTIVE_H
#define FS_IREFLECTIVE_H


#include <CommonDefinitions.h>


namespace fs
{
	class ReflectionPool;


	struct ReflectionTypeId
	{
		ReflectionTypeId() : _typeSize{ 0 }, _hashCode{ 0 }, _isReflective{ false }{}
		ReflectionTypeId(const std::type_info& type)
			: ReflectionTypeId()
		{
			_typeName	= type.name();
			_hashCode	= type.hash_code();
		}

		const bool operator==(const std::type_info& type) const noexcept
		{
			return _hashCode == type.hash_code();
		}

		std::string		_typeName;
		size_t			_typeSize;
		size_t			_hashCode;
		bool			_isReflective;
	};

	struct ReflectionTypeData
	{
		friend ReflectionPool;

		ReflectionTypeData() : _isRegisterDone{ false } {}
		explicit ReflectionTypeData(const std::type_info& type) : ReflectionTypeData() { _type = type; }

		ReflectionTypeId				_type;
		std::string						_declarationName;
		std::vector<ReflectionTypeData>	_memberArray;

	private:
		bool							_isRegisterDone;
	};

	class IReflective;
	class ReflectionPool final
	{
		friend IReflective;

	private:
											ReflectionPool() = default;
											~ReflectionPool() = default;

	private:
		static uint32						registerType(const std::type_info& type, const size_t typeSize, const std::type_info& memberType, const std::string memberName, const size_t memberSize);
		static void							registerTypeDone(const uint32 typeIndex);
		static const ReflectionTypeData&	getTypeData(const uint32 typeIndex);

	private:
		static ReflectionPool&				getInstance();

	private:
		std::vector<ReflectionTypeData>		_typeArray;
	};

#define FS_DECLARE_REFLECTIVE(type, name)			type name; name.setDeclarationName(#name);

#define FS_REFLECTIVE_CTOR(className)				public: className() { registerTypeInfoXXX(); }
#define FS_REFLECTIVE_CTOR_INIT(className, init)	public: className() : init { registerTypeInfoXXX(); }

#define FS_DECLARE_MEMBER(type, name)				public: type name;

#define FS_REGISTER_BEGIN()							protected: virtual void registerTypeInfoXXX() override {
#define FS_REGISTER_MEMBER(name)						__super::registerType(typeid(*this), sizeof(*this), typeid(name), #name, sizeof(name));
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

		const ReflectionTypeData&	getType()
		{
			return fs::ReflectionPool::getTypeData(_myTypeIndex);
		}

		const uint32				getMemberCount() const noexcept
		{
			return _memberCount;
		}

		const ReflectionTypeData&	getMemberType(const uint32 memberIndex)
		{
			return fs::ReflectionPool::getTypeData(_myTypeIndex)._memberArray[memberIndex];
		}

	protected:
		void						registerType(const std::type_info& type, const size_t typeSize, const std::type_info& memberType, const std::string memberName, const size_t memberSize)
		{
			_myTypeIndex = fs::ReflectionPool::registerType(type, typeSize, memberType, memberName, memberSize);
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
