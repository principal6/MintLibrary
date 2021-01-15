#pragma once

#include <stdafx.h>
#include <FsLibrary/Container/StringUtil.h>

#include <FsLibrary/Container/Tree.hpp>


namespace fs
{
	namespace StringUtil
	{
		FS_INLINE void convertWideStringToString(const std::wstring& source, std::string& destination)
		{
			destination.resize(source.length());
			::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()), nullptr, nullptr);
		}

		FS_INLINE void excludeExtension(std::string& inoutText)
		{
			const size_t found = inoutText.find('.');
			if (found != std::string::npos)
			{
				inoutText = inoutText.substr(0, found);
			}
		}

		void tokenize(const fs::ContiguousStringA& inputString, const char delimiter, fs::ContiguousVector<fs::ContiguousStringA>& outArray)
		{
			if (inputString.empty() == true)
			{
				return;
			}

			outArray.clear();

			uint32 prevAt = 0;
			const uint32 length = inputString.length();
			for (uint32 at = 0; at < length; ++at)
			{
				if (inputString.getChar(at) == delimiter)
				{
					if (prevAt < at)
					{
						outArray.push_back(inputString.substr(prevAt, at - prevAt));
					}

					prevAt = at + 1;
				}
			}

			if (prevAt < length)
			{
				outArray.push_back(inputString.substr(prevAt, length - prevAt));
			}
		}

		void tokenize(const fs::ContiguousStringA& inputString, const fs::ContiguousVector<char>& delimiterArray, fs::ContiguousVector<fs::ContiguousStringA>& outArray)
		{
			if (inputString.empty() == true)
			{
				return;
			}

			outArray.clear();

			uint32 prevAt = 0;
			const uint32 delimiterCount = delimiterArray.size();
			const uint32 length = inputString.length();
			for (uint32 at = 0; at < length; ++at)
			{
				for (uint32 delimiterIndex = 0; delimiterIndex < delimiterCount; ++delimiterIndex)
				{
					if (inputString.getChar(at) == delimiterArray.at(delimiterIndex))
					{
						if (prevAt < at)
						{
							outArray.push_back(inputString.substr(prevAt, at - prevAt));
						}

						prevAt = at + 1;
					}
				}
			}

			if (prevAt < length)
			{
				outArray.push_back(inputString.substr(prevAt, length - prevAt));
			}
		}

		void tokenize(const std::string& inputString, const std::string& delimiterString, std::vector<std::string>& outArray)
		{
			if (inputString.empty() == true || delimiterString.empty() == true)
			{
				return;
			}

			outArray.clear();

			const uint64 length = inputString.length();
			const uint64 delimiterLength = delimiterString.length();
			uint64 at = 0;
			uint64 prevAt = 0;
			while (at < length)
			{
				if (0 == inputString.compare(at, delimiterLength, delimiterString))
				{
					if (prevAt < at)
					{
						outArray.push_back(inputString.substr(prevAt, at - prevAt));
					}

					at += delimiterLength;
					prevAt = at;
				}
				else
				{
					++at;
				}
			}

			if (prevAt < length)
			{
				outArray.push_back(inputString.substr(prevAt, length - prevAt));
			}
		}
	}
}
