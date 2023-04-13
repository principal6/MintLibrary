#include <MintCommon/Include/CommonDefinitions.h>

#include <Windows.h>

#include <ctime>
#include <cstdarg>

#include <fstream>
#include <filesystem>


namespace mint
{
#pragma region LoggerString
	LoggerString::LoggerString()
		: _capacity{ 0 }
		, _size{ 0 }
		, _rawPointer{ nullptr }
	{
		__noop;
	}

	LoggerString::LoggerString(const uint32 capacity)
		: LoggerString()
	{
		Reserve(capacity);
	}

	LoggerString::LoggerString(const char* const rawString)
		: LoggerString()
	{
		*this = rawString;
	}

	LoggerString::~LoggerString()
	{
		Release();
	}

	LoggerString& LoggerString::operator=(const LoggerString& rhs)
	{
		if (this != &rhs)
		{
			*this = rhs.CString();
		}
		return *this;
	}

	LoggerString& LoggerString::operator=(const char* const rhs)
	{
		const uint32 rhsLength = static_cast<uint32>(::strlen(rhs));
		Reserve(rhsLength);
		::strcpy_s(_rawPointer, _capacity + 1, rhs);
		_size = rhsLength;
		return *this;
	}

	LoggerString& LoggerString::operator+=(const char* const rhs)
	{
		const uint32 rhsLength = static_cast<uint32>(::strlen(rhs));
		Reserve(Max((_capacity + 1) * 2 - 1, _size + rhsLength));
		::strcpy_s(&_rawPointer[_size], rhsLength + 1, rhs);
		_size += rhsLength;
		return *this;
	}

	void LoggerString::Reserve(const uint32 newCapacity) noexcept
	{
		if (newCapacity <= _capacity)
		{
			return;
		}

		char* temp = nullptr;
		if (_size > 0)
		{
			temp = MINT_NEW_ARRAY(char, _size + 1);
			::strcpy_s(temp, _size + 1, _rawPointer);
		}

		MINT_DELETE_ARRAY(_rawPointer);
		_rawPointer = MINT_NEW_ARRAY(char, newCapacity + 1);

		if (_size > 0)
		{
			::strcpy_s(_rawPointer, _size + 1, temp);
			MINT_DELETE_ARRAY(temp);
		}

		_capacity = newCapacity;
	}

	void LoggerString::Release() noexcept
	{
		MINT_DELETE_ARRAY(_rawPointer);
		_capacity = 0;
		_size = 0;
	}
#pragma endregion


#pragma region Logger
	Logger::Logger()
		: _basePathOffset{ 0 }
		, _history{ 2048 }
		, _outputFileName{ 2048 }
	{
		std::filesystem::path currentPath = std::filesystem::current_path();
		//if (currentPath.has_parent_path())
		//{
		//    currentPath = currentPath.parent_path();
		//}
		_basePathOffset = static_cast<uint32>(currentPath.string().length()) + 1;
	}

	Logger::~Logger()
	{
		std::lock_guard<std::mutex> scopeLock{ _mutex };
		if (_outputFileName.IsEmpty() == false)
		{
			std::ofstream ofs;
			ofs.open(_outputFileName.CString());

			ofs.write(_history.CString(), _history.Length());

			ofs.close();
		}
	}

	Logger& Logger::GetInstance() noexcept
	{
		static Logger logger;
		return logger;
	}

	void Logger::SetOutputFileName(const char* const fileName) noexcept
	{
		Logger& logger = GetInstance();
		std::lock_guard<std::mutex> scopeLock{ logger._mutex };
		logger._outputFileName = fileName;
	}

	void Logger::Log(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...)
	{
		static char finalBuffer[kFinalBufferSize]{};
		static char content[kFinalBufferSize]{};

		// variadic arguments
		{
			va_list vl;
			va_start(vl, format);
			vsprintf_s(content, kFinalBufferSize, format, vl);
			va_end(vl);
		}

		LogInternal(logTag, author, content, functionName, fileName, lineNumber, finalBuffer);

		printf(finalBuffer);
	}

	void Logger::LogAlert(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...)
	{
		static char finalBuffer[kFinalBufferSize]{};
		static char content[kFinalBufferSize]{};

		// variadic arguments
		{
			va_list vl;
			va_start(vl, format);
			vsprintf_s(content, kFinalBufferSize, format, vl);
			va_end(vl);
		}

		LogInternal(logTag, author, content, functionName, fileName, lineNumber, finalBuffer);

		printf(finalBuffer);
		::MessageBoxA(nullptr, content, "LOG ALERT", MB_ICONEXCLAMATION);
	}

	void Logger::LogError(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...)
	{
		static char finalBuffer[kFinalBufferSize]{};
		static char content[kFinalBufferSize]{};

		// variadic arguments
		{
			va_list vl;
			va_start(vl, format);
			vsprintf_s(content, kFinalBufferSize, format, vl);
			va_end(vl);
		}

		LogInternal(logTag, author, content, functionName, fileName, lineNumber, finalBuffer);

		printf(finalBuffer);
		::MessageBoxA(nullptr, content, "LOG ERROR", MB_ICONERROR);
	}

	void Logger::LogInternal(const char* const logTag, const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber, char(&outBuffer)[kFinalBufferSize])
	{
		static char timeBuffer[kTimeBufferSize]{};
		{
			std::lock_guard<std::mutex> scopeLock{ _mutex };

			const time_t now = time(nullptr);
			tm localNow;
			localtime_s(&localNow, &now);
			strftime(timeBuffer, kTimeBufferSize, "%Y-%m-%d-%H:%M:%S", &localNow);

			if (logTag == nullptr || functionName == nullptr || fileName == nullptr)
			{
				if (author == nullptr || ::strlen(author) == 0)
				{
					sprintf_s(outBuffer, kFinalBufferSize, "%s\n", content);
				}
				else
				{
					sprintf_s(outBuffer, kFinalBufferSize, "[%s] %s\n", author, content);
				}
			}
			else
			{
				const uint32 fileNameLength = static_cast<uint32>(::strlen(fileName));
				sprintf_s(outBuffer, kFinalBufferSize, "%s(%d): %s() - [%s] %s [%s] %s \n", (_basePathOffset < fileNameLength) ? fileName + _basePathOffset : fileName,
					lineNumber, functionName, logTag, timeBuffer, author, content);
			}

			OutputDebugStringA(outBuffer);

			_history += outBuffer;
		}
	}
#pragma endregion

#pragma region Path
	void Path::SetAssetDirectory(const Path& assetDirectory) noexcept
	{
		GlobalPaths::GetInstance()._assetDirectory = assetDirectory;
	}

	void Path::SetIncludeAssetDirectory(const Path& includeAssetDirectory) noexcept
	{
		GlobalPaths::GetInstance()._includeAssetDirectory = includeAssetDirectory;
	}

	Path Path::MakeAssetPath(const Path& subDirectoryPath) noexcept
	{
		MINT_ASSERT(GlobalPaths::GetInstance()._assetDirectory.IsEmpty() == false, "이 함수를 static 변수 초기화나 static 변수의 멤버 초기화에 사용하면 안 됩니다!!!");
		return Path(GlobalPaths::GetInstance()._assetDirectory, subDirectoryPath);
	}

	Path Path::MakeIncludeAssetPath(const Path& subDirectoryPath) noexcept
	{
		MINT_ASSERT(GlobalPaths::GetInstance()._includeAssetDirectory.IsEmpty() == false, "이 함수를 static 변수 초기화나 static 변수의 멤버 초기화에 사용하면 안 됩니다!!!");
		return Path(GlobalPaths::GetInstance()._includeAssetDirectory, subDirectoryPath);
	}

	Path::Path()
		: _rawString{}
		, _length{ 0 }
	{
		__noop;
	}

	Path::Path(const Path& rhs)
		: Path(rhs.CString())
	{
		__noop;
	}

	Path::Path(const Path& directory, const Path& subDirectoryPath)
	{
		if (directory.IsEmpty())
		{
			MINT_ASSERT(false, "directory 가 비어 있습니다!!!");
			*this = subDirectoryPath;
		}
		else
		{
			Path normalizedDirectory = directory;
			if (normalizedDirectory.EndsWithSlash() == false)
			{
				normalizedDirectory += '/';

				MINT_ASSERT(normalizedDirectory.EndsWithSlash(), "directory 가 비정상적입니다!!!");
			}

			*this = normalizedDirectory;
			*this += subDirectoryPath;
		}
	}

	Path::Path(const char* const rhs)
		: Path()
	{
		*this = rhs;
	}

	Path::~Path()
	{
		__noop;
	}

	Path& Path::operator=(const Path& rhs)
	{
		if (this != &rhs)
		{
			*this = rhs.CString();
		}
		return *this;
	}

	Path& Path::operator=(const char* const rhs)
	{
		::strcpy_s(_rawString, rhs);
		_length = static_cast<uint32>(::strlen(_rawString));
		return *this;
	}

	Path& Path::operator+=(const Path& rhs)
	{
		if (this != &rhs)
		{
			*this += rhs.CString();
		}
		return *this;
	}

	Path& Path::operator+=(const char* const rhs)
	{
		::strcat_s(_rawString, rhs);
		_length = static_cast<uint32>(::strlen(_rawString));
		return *this;
	}

	Path& Path::operator+=(const char rhs)
	{
		if (_length < kMaxPath)
		{
			_rawString[_length] = rhs;
			_rawString[_length + 1] = 0;
			++_length;
		}
		return *this;
	}

	bool Path::EndsWithSlash() const noexcept
	{
		return (_length == 0) ? false : _rawString[_length - 1] == '/';
	}
#pragma endregion

#pragma region GlobalPaths
	GlobalPaths& GlobalPaths::GetInstance() noexcept
	{
		static GlobalPaths instance;
		return instance;
	}

	GlobalPaths::GlobalPaths()
		: _assetDirectory{}
	{
		__noop;
	}

	GlobalPaths::~GlobalPaths()
	{
		__noop;
	}
#pragma endregion
}
