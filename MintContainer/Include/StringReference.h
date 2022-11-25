#pragma once


#ifndef MINT_CONTAINER_STRING_REFERENCE_H
#define MINT_CONTAINER_STRING_REFERENCE_H


namespace mint
{
	enum class StringType
	{
		ConstantLiteralString,
		StackString,
		HeapString,
		COUNT
	};


	template <typename T>
	class StringReference
	{
	public:
		StringReference(const T* const rhs) : _stringType{ StringType::ConstantLiteralString }, _capacity{ reinterpret_cast<uint64>(rhs) } { __noop; }
		StringReference(const StringType stringType) : _stringType{ stringType }, _capacity{ 0 } { __noop; }
		virtual ~StringReference() = default;

	public:
		StringType			getStringType() const { return _stringType; }
		bool				isLiteral() const { return true; }
		virtual bool		isMutable() const { return false; }
		virtual const T*	c_str() const { return reinterpret_cast<const T*>(_capacity); }
		virtual uint32		length() const;
		virtual uint32		find(const StringReference<T>& token, const uint32 offset = 0) const;
		virtual bool		contains(const StringReference<T>& token, const uint32 offset = 0) const;
		virtual uint64		computeHash() const;

	protected:
		StringType	_stringType;
		uint64		_capacity;
	};


	template <typename T>
	class MutableString abstract : public StringReference<T>
	{
	public:
		MutableString(const StringType stringType) : StringReference<T>(stringType) { __noop; }
		virtual ~MutableString() = default;

	public:
		virtual bool	isMutable() const override final { return true; }
		virtual uint32	capacity() const { return 0; }

	public:
		virtual void	clear() abstract;
		virtual MutableString<T>& append(const StringReference<T>& rhs) abstract;
		virtual MutableString<T>& assign(const StringReference<T>& rhs) abstract;
	};


	template <typename T>
	bool operator==(const StringReference<T>& lhs, const StringReference<T>& rhs);
}


#endif // !MINT_CONTAINER_STRING_REFERENCE_H
