#pragma once


#ifndef _MINT_COMMON_COMMON_DEFINITIONS_H_
#define _MINT_COMMON_COMMON_DEFINITIONS_H_


#define MINT_INLINE __forceinline
#define MINT_VARNAME(var) #var

#define NOMINMAX


#if defined(DEBUG) | defined(_DEBUG)
#define MINT_DEBUG
#endif


#include <cstdint>

#include <mutex>


namespace mint
{
	template<typename T>
	MINT_INLINE constexpr const T& Max(const T& a, const T& b)
	{
		return (a < b) ? b : a;
	}

	template<typename T>
	MINT_INLINE constexpr const T& Max(T&& a, T&& b)
	{
		return (a < b) ? b : a;
	}

	template<typename T>
	MINT_INLINE constexpr const T& Min(const T& a, const T& b)
	{
		return (a > b) ? b : a;
	}

	template<typename T>
	MINT_INLINE constexpr const T& Min(T&& a, T&& b)
	{
		return (a > b) ? b : a;
	}

	template<typename T>
	MINT_INLINE constexpr const T& Clamp(const T& value, const T& lowerBound, const T& upperBound)
	{
		return Min(Max(value, lowerBound), upperBound);
	}

	template<typename T>
	MINT_INLINE constexpr const T& Clamp(T&& value, T&& lowerBound, T&& upperBound)
	{
		return Min(Max(value, lowerBound), upperBound);
	}
}


#pragma region Integer definitions
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using byte = uint8;

static constexpr int8 kInt8Max = (int8)0x7F;
static constexpr int8 kInt8Min = (int8)~0x7F;
static constexpr int16 kInt16Max = (int16)0x7FFF;
static constexpr int16 kInt16Min = (int16)~0x7FFF;
static constexpr int32 kInt32Max = (int32)0x7FFFFFFF;
static constexpr int32 kInt32Min = (int32)~0x7FFFFFFF;
static constexpr int64 kInt64Max = (int64)0x7FFFFFFFFFFFFFFF;
static constexpr int64 kInt64Min = (int64)~0x7FFFFFFFFFFFFFFF;

static constexpr uint8 kUint8Max = (uint8)0xFF;
static constexpr uint16 kUint16Max = (uint16)0xFFFF;
static constexpr uint32 kUint32Max = (uint32)0xFFFFFFFF;
static constexpr uint64 kUint64Max = (uint64)0xFFFFFFFFFFFFFFFF;

static constexpr uint32 kBitsPerByte = 8;
static_assert(kBitsPerByte == 8, "Number of Bits per Byte must be 8!!!");

static constexpr uint32 kStackSizeLimit = 1 << 14;

// TODO: Index 를 class 로 만들 것
static constexpr int32 kInvalidIndexInt32 = -1;
constexpr bool IsValidIndex(int32 index) { return index >= 0; }
#pragma endregion


#pragma region String
static constexpr uint32 kStringNPos = kUint32Max;
using U8CharCode = uint32;
#pragma endregion


#pragma region File
static constexpr uint32 kMaxPath = 260;
#pragma endregion


#ifdef MINT_CHECK_HEAP_ALLOCATION
__noop;
#else
#define MINT_NEW(type, ...) new type{ __VA_ARGS__ }
#define MINT_PLACEMNT_NEW(pointer, ctor) new (pointer) ctor
#define MINT_NEW_ARRAY(type, size) new type[size]{}
#define MINT_NEW_ARRAY_USING_BYTE(type, size) reinterpret_cast<type*>(new byte[sizeof(type) * size]{})
#define MINT_DELETE(obj) if (obj != nullptr) { delete obj; obj = nullptr; }
#define MINT_DELETE_ARRAY(obj) if (obj != nullptr) { delete[] obj; obj = nullptr; }
#define MINT_MALLOC(type, count) reinterpret_cast<type*>(::malloc(sizeof(type) * count))
#define MINT_FREE(pointer) ::free(pointer); pointer = nullptr
#endif


static constexpr int32 kErrorExitCode = -1;

#pragma region Logging
#if defined MINT_DEBUG
#define _MINT_LOG_ERROR_ACTION __debugbreak()
#else
#define _MINT_LOG_ERROR_ACTION exit(kErrorExitCode)
#endif

#define MINT_LOG_UNTAGGED(format, ...) mint::Logger::GetInstance().Log(nullptr, nullptr, nullptr, nullptr, 0, format, __VA_ARGS__)
#define MINT_LOG(format, ...) mint::Logger::GetInstance().Log(" _LOG_ ", "MINT", __func__, __FILE__, __LINE__, format, __VA_ARGS__)
#define MINT_LOG_ALERT(format, ...) mint::Logger::GetInstance().LogAlert(" ALERT ", "MINT", __func__, __FILE__, __LINE__, format, __VA_ARGS__)
#define MINT_LOG_ERROR(format, ...) mint::Logger::GetInstance().LogError(" ERROR ", "MINT", __func__, __FILE__, __LINE__, format, __VA_ARGS__); _MINT_LOG_ERROR_ACTION
#pragma endregion


#pragma region Assertion
#define MINT_NEVER { mint::Logger::GetInstance().LogError(" ASSUR ", "MINT", __func__, __FILE__, __LINE__, "THIS BRANCH IS NOT ALLOWED!"); _MINT_LOG_ERROR_ACTION; }
// [DESCRIPTION]
// Return false if expression is false!
#define MINT_ASSURE(expression) if (!(expression)) { mint::Logger::GetInstance().LogError(" ASSUR ", "MINT", __func__, __FILE__, __LINE__, "NOT ASSURED. RETURN FALSE!"); _MINT_LOG_ERROR_ACTION; return false; }

#if defined MINT_LOG_FOR_ASSURE_SILENT
#define MINT_ASSURE_SILENT(expression) if (!(expression)) { mint::Logger::GetInstance().log(" ASSUR ", "MINT", __func__, __FILE__, __LINE__, "NOT ASSURED. RETURN FALSE!"); return false; }
#else
#define MINT_ASSURE_SILENT(expression) if (!(expression)) { return false; }
#endif

#if defined MINT_DEBUG
#define MINT_ASSERT(expression, format, ...) if (!(expression)) { mint::Logger::GetInstance().LogError(" ASSRT ", "MINT", __func__, __FILE__, __LINE__, format, __VA_ARGS__); _MINT_LOG_ERROR_ACTION; }
#else
#define MINT_ASSERT(expression, format, ...)
#endif
#define MINT_RETURN_FALSE_IF_NOT(expression) if (!(expression)) return false
#pragma endregion


namespace mint
{
	class LoggerString
	{
	private:
		LoggerString();

	public:
		LoggerString(const uint32 capacity);
		LoggerString(const char* const rawString);
		~LoggerString();

	public:
		LoggerString& operator=(const LoggerString& rhs);
		LoggerString& operator=(const char* const rhs);
		LoggerString& operator+=(const char* const rhs);

	public:
		MINT_INLINE bool IsEmpty() const noexcept { return _size == 0; }
		MINT_INLINE const char* CString() const noexcept { return _rawPointer; }
		MINT_INLINE uint32 Length() const noexcept { return _size; }

	private:
		void Reserve(const uint32 newCapacity) noexcept;
		void Release() noexcept;

	private:
		uint32 _capacity;
		uint32 _size;
		char* _rawPointer;
	};


	class Logger
	{
	private:
		static constexpr uint32 kTimeBufferSize = 100;
		static constexpr uint32 kFinalBufferSize = 1024;

	private:
		Logger();

	public:
		~Logger();

	public:
		static Logger& GetInstance() noexcept;
		static void SetOutputFileName(const char* const fileName) noexcept;

	public:
		void Log(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);
		void LogAlert(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);
		void LogError(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);

	private:
		void LogInternal(const char* const logTag, const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber, char(&outBuffer)[kFinalBufferSize]);

	private:
		uint32 _basePathOffset;
		std::mutex _mutex;
		LoggerString _history;
		LoggerString _outputFileName;
	};


	class Path
	{
	public:
		static void SetAssetDirectory(const Path& assetDirectory) noexcept;
		static void SetIncludeAssetDirectory(const Path& includeAssetDirectory) noexcept;

		// WARNING!!! static 변수 초기화나 static 변수의 멤버 초기화에 사용하면 안 된다!!!
		static Path MakeAssetPath(const Path& subDirectoryPath) noexcept;
		// WARNING!!! static 변수 초기화나 static 변수의 멤버 초기화에 사용하면 안 된다!!!
		static Path MakeIncludeAssetPath(const Path& subDirectoryPath) noexcept;

	public:
		Path();
		Path(const Path& rhs);
		Path(const Path& directory, const Path& subDirectoryPath);
		Path(const char* const rhs);
		~Path();

	public:
		Path& operator=(const Path& rhs);
		Path& operator=(const char* const rhs);
		Path& operator+=(const Path& rhs);
		Path& operator+=(const char* const rhs);
		Path& operator+=(const char rhs);

		operator const char* () const { return CString(); }

	public:
		void Clear() noexcept { _rawString[0] = 0; _length = 0; }
		char* Data() noexcept { return _rawString; }
		bool IsEmpty() const noexcept { return _length == 0; }
		const char* CString() const noexcept { return _rawString; }

	private:
		bool EndsWithSlash() const noexcept;

	private:
		char _rawString[kMaxPath + 1];
		uint32 _length;
	};

	class GlobalPaths
	{
		friend Path;

	private:
		static GlobalPaths& GetInstance() noexcept;
		GlobalPaths();

	public:
		~GlobalPaths();

	private:
		Path _assetDirectory;
		Path _includeAssetDirectory;
	};
}


#endif // !_MINT_COMMON_COMMON_DEFINITIONS_H_
