#pragma once

#include <stdafx.h>
#include <Container/StringUtil.h>

#include <Container/Tree.hpp>


namespace fs
{
	namespace StringUtil
	{
		void tokenize(const fs::DynamicStringA& inputString, const char delimiter, fs::Vector<fs::DynamicStringA>& outArray)
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

		void tokenize(const fs::DynamicStringA& inputString, const fs::Vector<char>& delimiterArray, fs::Vector<fs::DynamicStringA>& outArray)
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
					if (inputString.getChar(at) == delimiterArray.get(delimiterIndex))
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
