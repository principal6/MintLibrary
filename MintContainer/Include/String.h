#pragma once


#ifndef _MINT_CONTAINER_STRING_H_
#define _MINT_CONTAINER_STRING_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/StringReference.h>


namespace mint
{
	template <typename T>
	class String : public MutableString<T>
	{
		static constexpr uint32 kTypeSize = sizeof(T);

	public:
		String();
		String(const uint32 size, const T ch);
		String(const T* const rhs);
		String(const String& rhs);
		String(String&& rhs) noexcept;
		~String();

	public:
		String& operator=(const T* const rhs) noexcept;
		String& operator=(const String& rhs) noexcept;
		String& operator=(String&& rhs) noexcept;

	public:
		String operator+(const T* const rhs) const noexcept;
		String operator+(const String& rhs) const noexcept;
		String& operator+=(const T* const rhs) noexcept;
		String& operator+=(const String& rhs) noexcept;

	public:
		bool operator==(const T* const rhs) const noexcept;
		bool operator==(const String& rhs) const noexcept;
		bool operator!=(const T* const rhs) const noexcept;
		bool operator!=(const String& rhs) const noexcept;

	public:
		virtual MutableString<T>& Assign(const StringReference<T>& rhs) override;
		String& Assign(const T* const rawString) noexcept;

	private:
		String& AssignInternalXXX(const T* const rawString) noexcept;
		String& AssignInternalLongXXX(const T* const rawString) noexcept;

	public:
		virtual MutableString<T>& Append(const StringReference<T>& rhs) override;
		String& Append(const T* const rawString) noexcept;
		String& Append(const T ch) noexcept;
		String& Append(const String& rhs) noexcept;

	private:
		String& AppendInternalSmallXXX(const T* const rhs) noexcept;
		String& AppendInternalLongXXX(const T* const rhs) noexcept;

	public:
		void Reserve(const uint32 newCapacity) noexcept;
		void Resize(const uint32 newSize, const T fillCharacter = 0) noexcept;

	private:
		void _SetSize(const uint32 newSize) noexcept;

	public:
		virtual void Clear() override;

	public:
		virtual StringType GetStringType() const override { return StringType::MutableHeapString; }
		virtual uint32 Capacity() const override { return static_cast<uint32>(IsSmallString() ? Short::kSmallStringCapacity : _long._capacity); }
		MINT_INLINE uint32 Size() const noexcept { return static_cast<uint32>(IsSmallString() ? _short._size : _long._size); }
		virtual uint32 Length() const override { return Size(); }
		virtual const T* CString() const override;
		T Back() const;

	private:
		virtual T* Data() override;
		static void __CopyString(T* const destination, const T* const source, const uint64 byteCount) noexcept;

	public:
		uint32 Find(const T* const target, const uint32 offset = 0) const noexcept;
		uint32 Find(const T target, const uint32 offset = 0) const noexcept;
		String Substring(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
		void Insert(const uint32 at, const T ch) noexcept;
		void Insert(const uint32 at, const T* const rhs) noexcept;
		void Insert(const uint32 at, const String& rhs) noexcept;
		void Erase(const uint32 at) noexcept;
		void Erase(const uint32 at, const uint32 byteCount) noexcept;
		void PopBack();
		bool Equals(const T* const rhs) const noexcept;
		bool Equals(const String& rhs) const noexcept;
		uint64 ComputeHash() const noexcept;

	private:
		void Release() noexcept;
		void ToLongString() noexcept;
		MINT_INLINE bool IsSmallString() const noexcept { return _short._size < Short::kSmallStringCapacity; }
		MINT_INLINE bool IsNotAllocated() const noexcept { return (IsSmallString() == true) ? (_short._size == 0) : (_long._rawPointer == nullptr); }

	private:
		//
		// LONG                     |                capacity               |                  size                 |              rawPointer               |
		// SHORT(1 byte-wide char ) |size| 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 |
		// SHORT(2 bytes-wide char) |   size  |    00   |    01   |    02   |    03   |    04   |    05   |    06   |    07   |    08   |    09   |    10   |
		// SHORT(4 bytes-wide char) |        size       |         00        |         01        |         02        |         03        |         04        |
		//
		struct Long
		{
			uint64 _capacity; // 8 bytes
			uint64 _size;     // 8 bytes
			T* _rawPointer;   // 8 bytes
		};
		struct Short
		{
			static constexpr uint32 kSmallStringCapacity = (24 / kTypeSize) - 1;

			T _size;                              //  1 (char) or  2 (wchar_t)
			T _smallString[kSmallStringCapacity]; // 23 (char) or 22 (wchar_t)
		};
		union
		{
			Long _long{};
			Short _short;
		};
	};


	using StringA = String<char>;
	using StringW = String<wchar_t>;
	using StringU8 = String<char8_t>;
}


#endif // !_MINT_CONTAINER_STRING_H_
