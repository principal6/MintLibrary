#pragma once

#include <stdafx.h>
#include <Container\StringUtil.h>


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
					outArray.push_back(inputString.substr(prevAt, at - prevAt));

					prevAt = at + 1;
				}
			}

			if (prevAt < length)
			{
				outArray.push_back(inputString.substr(prevAt, length - prevAt));
			}
		}

	}
}
