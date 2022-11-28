#pragma once


#ifndef MINT_CONTAINER_STRING_REFERENCE_H
#define MINT_CONTAINER_STRING_REFERENCE_H


namespace mint
{
	enum class StringType
	{
		ConstantLiteralString,
		MutableStackString,
		MutableHeapString,
		COUNT
	};


	template <typename T>
	class StringReference
	{
	public:
		StringReference() : _literalString{ nullptr } { __noop; }
		StringReference(const T* const rhs) : _literalString{ rhs } { __noop; }
		virtual ~StringReference() = default;

	public:
		bool		operator==(const StringReference<T>& rhs) const;
		const T&	operator[](const uint32 index) const { return at(index); }

	public:
		virtual StringType	getStringType() const { return StringType::ConstantLiteralString; }
		bool				isLiteral() const { return true; }
		virtual bool		isMutable() const { return false; }
		virtual uint32		length() const;
		virtual uint32		countBytes() const;
		virtual uint32		countChars() const;
		virtual const T&	at(const uint32 index) const { return c_str()[index]; }
		virtual const T*	c_str() const { return _literalString; }
		virtual uint32		find(const StringReference<T>& token, const uint32 offset = 0) const;
		uint32              rfind(const StringReference<T>& token, const uint32 offset = 0) const;
		virtual bool		contains(const StringReference<T>& token, const uint32 offset = 0) const;
		virtual uint64		computeHash() const;

	private:
		const T*			_literalString;
	};


	template <typename T>
	class MutableString abstract : public StringReference<T>
	{
	public:
		MutableString() : StringReference<T>() { __noop; }
		virtual ~MutableString() = default;

	public:
		MutableString<T>&	operator=(const StringReference<T>& rhs);
		// TODO: 구현 최적화
		MutableString<T>&	operator=(StringReference<T>&& rhs);
		MutableString<T>&	operator+=(const StringReference<T>& rhs);

		T&					operator[](const uint32 index) { return at(index); }

	public:
		virtual StringType	getStringType() const abstract;
		virtual bool		isMutable() const override final { return true; }
		virtual uint32		capacity() const { return 0; }
		virtual T&			at(const uint32 index) { return data()[index]; }
		virtual const T*	c_str() const abstract;
		virtual T*			data() abstract;

	public:
		virtual void		clear() abstract;
		virtual MutableString<T>& append(const StringReference<T>& rhs) abstract;
		virtual MutableString<T>& assign(const StringReference<T>& rhs) abstract;
	};
}


#endif // !MINT_CONTAINER_STRING_REFERENCE_H
