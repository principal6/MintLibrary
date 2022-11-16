#pragma once


#ifndef MINT_CONTAINER_STRING_BASE_H
#define MINT_CONTAINER_STRING_BASE_H


namespace mint
{
	enum class StringType
	{
		StackString,
		HeapString,
		//ConstantString,
		ConstantLiteralString,
		COUNT
	};


	template <typename T>
	class StringBase abstract
	{
	public:
		StringBase(const StringType stringType) : _stringType{ stringType } { __noop; }
		virtual ~StringBase() = default;

	public:
		StringType	getStringType() const { return _stringType; }
		virtual bool isMutable() const { return false; }
		virtual const T* c_str() const abstract;
		virtual uint32 length() const;
		virtual uint32 find(const StringBase<T>& token, const uint32 offset = 0) const;
		virtual bool contains(const StringBase<T>& token, const uint32 offset = 0) const;
		virtual uint64 computeHash() const;

	protected:
		StringType	_stringType;
	};


	template <typename T>
	class MutableString abstract : public StringBase<T>
	{
	public:
		MutableString(const StringType stringType) : StringBase<T>(stringType) { __noop; }
		virtual ~MutableString() = default;

	public:
		virtual bool	isMutable() const override final { return true; }
		virtual uint32	capacity() const { return 0; }

	public:
		virtual void	clear() abstract;
		virtual MutableString<T>& append(const StringBase<T>& rhs) abstract;
		virtual MutableString<T>& assign(const StringBase<T>& rhs) abstract;
	};


	template <typename T>
	class ConstantString abstract : public StringBase<T>
	{
	public:
		ConstantString(const StringType stringType, const T* string) : StringBase<T>(stringType), _string{ string } { __noop; }
		virtual ~ConstantString() = default;

	public:
		virtual const T* c_str() const override final { return _string; }

	protected:
		const T* _string;
	};


	// ==========
	// 구체적 구현. LiteralString.h 로 따로 빼야하나?
	template <typename T>
	class LiteralString : public ConstantString<T>
	{
	public:
		LiteralString(const T* string) : ConstantString<T>(StringType::ConstantLiteralString, string) { __noop; }
		virtual ~LiteralString() = default;

	public:

	};


	template <typename T>
	bool operator==(const StringBase<T>& lhs, const StringBase<T>& rhs);
}


#endif // !MINT_CONTAINER_STRING_BASE_H
