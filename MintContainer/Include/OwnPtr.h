﻿#pragma once


#ifndef _MINT_CONTAINER_OWN_PTR
#define _MINT_CONTAINER_OWN_PTR


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	// TODO: OwnPtr SharedPtr SharedPtrViewer
	template<typename T>
	class OwnPtr
	{
	public:
		OwnPtr() : _rawPointer{ nullptr } { __noop; }
		OwnPtr(T* const rawPointer) : _rawPointer{ rawPointer } { __noop; }
		OwnPtr(const OwnPtr& rhs) = delete;
		OwnPtr(OwnPtr&& rhs) : _rawPointer{ rhs._rawPointer } { rhs._rawPointer = nullptr; }
		~OwnPtr() { release(); }

	public:
		OwnPtr& operator=(const OwnPtr& rhs) noexcept = delete;
		OwnPtr& operator=(OwnPtr&& rhs) noexcept
		{
			if (this != &rhs)
			{
				_rawPointer = rhs._rawPointer;
				rhs._rawPointer = nullptr;
			}
			return *this;
		}

	public:
		// this is really dangerous ...
		//T* operator&() noexcept { return _rawPointer; }
		T& operator*() noexcept { return *_rawPointer; }
		const T& operator*() const noexcept { return *_rawPointer; }
		T* operator->() noexcept { return _rawPointer; }
		const T* operator->() const noexcept { return _rawPointer; }

	public:
		void assign(T*&& rawPointer) noexcept { release(); _rawPointer = rawPointer; rawPointer = nullptr; }
		void release() noexcept { MINT_DELETE(_rawPointer); }
		T* get() noexcept { return _rawPointer; }
		constexpr uint64 size() { return sizeof(T); }

	private:
		T* _rawPointer;
	};
}


#endif // !_MINT_CONTAINER_OWN_PTR
