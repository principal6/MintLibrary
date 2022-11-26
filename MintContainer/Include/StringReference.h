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
		bool	operator==(const StringReference<T>& rhs) const;

	public:
		virtual StringType	getStringType() const { return StringType::ConstantLiteralString; }
		bool				isLiteral() const { return true; }
		virtual bool		isMutable() const { return false; }
		virtual const T*	c_str() const { return _literalString; }
		virtual uint32		length() const;
		virtual uint32		countBytes() const;
		virtual uint32		countChars() const;
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
		virtual StringType	getStringType() const abstract;
		virtual bool		isMutable() const override final { return true; }
		virtual const T*	c_str() const abstract;
		virtual uint32		capacity() const { return 0; }

	public:
		virtual void		clear() abstract;
		virtual MutableString<T>& append(const StringReference<T>& rhs) abstract;
		virtual MutableString<T>& assign(const StringReference<T>& rhs) abstract;
	};
}


#endif // !MINT_CONTAINER_STRING_REFERENCE_H
