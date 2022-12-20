#pragma once


#ifndef MINT_CONTAINER_STRING_H
#define MINT_CONTAINER_STRING_H


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
		virtual MutableString<T>& assign(const StringReference<T>& rhs) override;
		String& assign(const T* const rawString) noexcept;

	private:
		String& assignInternalXXX(const T* const rawString) noexcept;
		String& assignInternalLongXXX(const T* const rawString) noexcept;

	public:
		virtual MutableString<T>& append(const StringReference<T>& rhs) override;
		String& append(const T* const rawString) noexcept;
		String& append(const T ch) noexcept;
		String& append(const String& rhs) noexcept;

	private:
		String& appendInternalSmallXXX(const T* const rhs) noexcept;
		String& appendInternalLongXXX(const T* const rhs) noexcept;

	public:
		void reserve(const uint32 newCapacity) noexcept;
		void resize(const uint32 newSize, const T fillCharacter = 0) noexcept;

	private:
		void _setSize(const uint32 newSize) noexcept;

	public:
		virtual void clear() override;

	public:
		virtual StringType getStringType() const override { return StringType::MutableHeapString; }
		virtual uint32 capacity() const override { return static_cast<uint32>(isSmallString() ? Short::kSmallStringCapacity : _long._capacity); }
		MINT_INLINE uint32 size() const noexcept { return static_cast<uint32>(isSmallString() ? _short._size : _long._size); }
		virtual uint32 length() const override { return size(); }
		virtual const T* c_str() const override;
		T back() const;

	private:
		virtual T* data() override;
		static void __copyString(T* const destination, const T* const source, const uint64 byteCount) noexcept;

	public:
		uint32 find(const T* const target, const uint32 offset = 0) const noexcept;
		uint32 find(const T target, const uint32 offset = 0) const noexcept;
		String substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
		void insert(const uint32 at, const T ch) noexcept;
		void insert(const uint32 at, const T* const rhs) noexcept;
		void insert(const uint32 at, const String& rhs) noexcept;
		void erase(const uint32 at) noexcept;
		void erase(const uint32 at, const uint32 byteCount) noexcept;
		void pop_back();
		bool compare(const T* const rhs) const noexcept;
		bool compare(const String& rhs) const noexcept;
		uint64 computeHash() const noexcept;

	private:
		void release() noexcept;
		void toLongString() noexcept;
		MINT_INLINE bool isSmallString() const noexcept { return _short._size < Short::kSmallStringCapacity; }
		MINT_INLINE bool isNotAllocated() const noexcept { return (isSmallString() == true) ? (_short._size == 0) : (_long._rawPointer == nullptr); }

	private:
		//
		// LONG                     |                capacity               |                  size                 |              rawPointer               |
		// SHORT(1 byte-wide char ) |size| 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 |
		// SHORT(2 bytes-wide char) |   size  |    00   |    01   |    02   |    03   |    04   |    05   |    06   |    07   |    08   |    09   |    10   |
		// SHORT(4 bytes-wide char) |        size       |         00        |         01        |         02        |         03        |         04        |
		//
		struct Long
		{
			uint64 _capacity;      // 8 bytes
			uint64 _size;          // 8 bytes
			T* _rawPointer;    // 8 bytes
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


#endif // !MINT_CONTAINER_STRING_H
