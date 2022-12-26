#pragma once


#ifndef _MINT_CONTAINER_STACK_STRING_H_
#define _MINT_CONTAINER_STACK_STRING_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Array.h>
#include <MintContainer/Include/StringReference.h>


namespace mint
{
	template <typename T, uint32 BufferSize>
	class StackString : public MutableString<T>
	{
	public:
		StackString();
		StackString(const T* const rawString);
		StackString(const StackString& rhs);
		StackString(StackString&& rhs) noexcept;
		~StackString();

	public:
		MutableString<T>& operator=(const StackString& rhs) { return assign(rhs); }
		MutableString<T>& operator=(StackString&& rhs) noexcept { return assign(rhs); }

	public:
		bool operator==(const T* const rawString) const noexcept;
		bool operator==(const StackString& rhs) const noexcept;
		bool operator!=(const T* const rawString) const noexcept;
		bool operator!=(const StackString& rhs) const noexcept;

	public:
		virtual StringType getStringType() const override { return StringType::MutableStackString; }
		virtual uint32 capacity() const override;
		virtual const T* c_str() const override;
		virtual T* data() override;

	private:
		bool canInsert(const uint32 length) const noexcept;

	public:
		virtual void clear();
		virtual MutableString<T>& append(const StringReference<T>& rhs) override;
		virtual MutableString<T>& assign(const StringReference<T>& rhs) override;
		void resize(const uint32 length) noexcept;

	public:
		StackString substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
		bool compare(const T* const rhs) const noexcept;
		bool compare(const StackString& rhs) const noexcept;
		uint64 computeHash() const noexcept;

	private:
		uint32 _length;
		Array<T, BufferSize> _raw;
	};


	template <uint32 BufferSize>
	using StackStringA = StackString<char, BufferSize>;

	template <uint32 BufferSize>
	using StackStringW = StackString<wchar_t, BufferSize>;

	template <uint32 BufferSize>
	using StackStringU8 = StackString<char8_t, BufferSize>;
}


#endif // !_MINT_CONTAINER_STACK_STRING_H_
