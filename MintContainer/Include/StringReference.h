#pragma once


#ifndef _MINT_CONTAINER_STRING_REFERENCE_H_
#define _MINT_CONTAINER_STRING_REFERENCE_H_


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
		bool operator==(const StringReference<T>& rhs) const;
		const T& operator[](const uint32 index) const { return At(index); }

	public:
		virtual StringType GetStringType() const { return StringType::ConstantLiteralString; }
		virtual bool IsLiteral() const { return true; }
		virtual bool IsMutable() const { return false; }
		bool IsEmpty() const { return Length() == 0; }
		virtual uint32 Length() const;
		virtual uint32 CountChars() const;
		virtual const T& At(const uint32 index) const { return CString()[index]; }
		virtual const T* CString() const { return _literalString; }
		virtual uint32 Find(const T token, const uint32 offset = 0) const;
		virtual uint32 Find(const StringReference<T>& token, const uint32 offset = 0) const;
		uint32 RFind(const StringReference<T>& token, const uint32 offset = 0) const;
		virtual bool Contains(const StringReference<T>& token, const uint32 offset = 0) const;
		bool StartsWith(const StringReference<T>& token) const;
		bool EndsWith(const StringReference<T>& token) const;
		virtual uint64 ComputeHash() const;

	private:
		const T* _literalString;
	};


	using StringReferenceA = StringReference<char>;
	using StringReferenceW = StringReference<wchar_t>;
	using StringReferenceU8 = StringReference<char8_t>;


	template <typename T>
	class MutableString abstract : public StringReference<T>
	{
	public:
		MutableString() : StringReference<T>() { __noop; }
		virtual ~MutableString() = default;

	public:
		MutableString<T>& operator=(const StringReference<T>& rhs);
		// TODO: 구현 최적화
		MutableString<T>& operator=(StringReference<T>&& rhs);
		MutableString<T>& operator+=(const StringReference<T>& rhs);
		MutableString<T>& operator+=(const T rhs);

		T& operator[](const uint32 index) { return At(index); }
		const T& operator[](const uint32 index) const { return At(index); }

	public:
		virtual StringType GetStringType() const abstract;
		virtual bool IsLiteral() const override final { return false; }
		virtual bool IsMutable() const override final { return true; }
		virtual uint32 Capacity() const { return 0; }
		virtual T& At(const uint32 index) { return Data()[index]; }
		virtual const T& At(const uint32 index) const { return CString()[index]; }
		virtual const T* CString() const abstract;
		virtual T* Data() abstract;

	public:
		virtual void Clear() abstract;
		virtual MutableString<T>& Append(const StringReference<T>& rhs) abstract;
		virtual MutableString<T>& Append(const T rhs) abstract;
		virtual MutableString<T>& Assign(const StringReference<T>& rhs) abstract;
	};
}


#endif // !_MINT_CONTAINER_STRING_REFERENCE_H_
