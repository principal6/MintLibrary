#pragma once


#ifndef FS_COMMON_DEFINITIONS_H
#define FS_COMMON_DEFINITIONS_H


#if defined DEBUG || _DEBUG
	#define FS_DEBUG
#endif


#define FS_INLINE __forceinline


#pragma region Integer definitions
	using				  int8						=   int8_t;
	using				 int16						=  int16_t;
	using				 int32						=  int32_t;
	using				 int64						=  int64_t;

	using				 uint8						=  uint8_t;
	using				uint16						= uint16_t;
	using				uint32						= uint32_t;
	using				uint64						= uint64_t;


	static constexpr	  int8		kInt8Max		= (  int8) 0x7F;
	static constexpr	  int8		kInt8Min		= (  int8)~0x7F;
	static constexpr	 int16		kInt16Max		= ( int16) 0x7FFF;
	static constexpr	 int16		kInt16Min		= ( int16)~0x7FFF;
	static constexpr	 int32		kInt32Max		= ( int32) 0x7FFFFFFF;
	static constexpr	 int32		kInt32Min		= ( int32)~0x7FFFFFFF;
	static constexpr	 int64		kInt64Max		= ( int64) 0x7FFFFFFFFFFFFFFF;
	static constexpr	 int64		kInt64Min		= ( int64)~0x7FFFFFFFFFFFFFFF;

	static constexpr	 uint8		kUint8Max		= ( uint8) 0xFF;
	static constexpr	uint16		kUint16Max		= (uint16) 0xFFFF;
	static constexpr	uint32		kUint32Max		= (uint32) 0xFFFFFFFF;
	static constexpr	uint64		kUint64Max		= (uint64) 0xFFFFFFFFFFFFFFFF;

	static constexpr	uint32		kBitsPerByte	= 8;
	static_assert(kBitsPerByte == 8, "Number of Bits per Byte must be 8!!!");

	static constexpr	uint32		kStackSizeLimit	= 1 << 14;
#pragma endregion


#pragma region String
	static constexpr	uint32		kStringNPos		= kUint32Max;
#pragma endregion


#pragma region File
	static constexpr	uint32		kMaxPath		= 260;
#pragma endregion


#ifdef FS_CHECK_HEAP_ALLOCATION
	__noop;
#else
	#define FS_NEW(type) new type{}
	#define FS_NEW_ARRAY(type, size) new type[size]{}
	#define FS_DELETE(obj) if (obj != nullptr) { delete obj; obj = nullptr; }
	#define FS_DELETE_ARRAY(obj) if (obj != nullptr) { delete[] obj; obj = nullptr; }
#endif


#endif // !FS_COMMON_DEFINITIONS_H
