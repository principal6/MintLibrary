#pragma once


#include <File/FileUtil.h>
#include <filesystem>


namespace fs
{
	namespace FileUtil
	{
		inline bool exists(const char* const fileName) noexcept
		{
			auto status{ std::filesystem::status(fileName) };
			auto type{ status.type() };
			if (type == std::filesystem::file_type::not_found)
			{
				return false;
			}
			return true;
		}

		inline bool isReadOnly(const char* const fileName) noexcept
		{
			auto status{ std::filesystem::status(fileName) };
			auto type{ status.type() };
			auto permissions{ status.permissions() };
			if (type == std::filesystem::file_type::not_found)
			{
				// file doesn't exist yet
				return false;
			}
			if (permissions == std::filesystem::perms::_File_attribute_readonly)
			{
				return true;
				// readonly file
				// make writable?
				//std::filesystem::permissions(fileName, permissions | std::filesystem::perms::_All_write);
			}
			return false;
		}

		inline void unsetReadOnly(const char* const fileName) noexcept
		{
			auto status{ std::filesystem::status(fileName) };
			auto type{ status.type() };
			auto permissions{ status.permissions() };
			if (type != std::filesystem::file_type::not_found && permissions == std::filesystem::perms::_File_attribute_readonly)
			{
				// make writable
				std::filesystem::permissions(fileName, permissions | std::filesystem::perms::_All_write);
			}
		}
	}
}
