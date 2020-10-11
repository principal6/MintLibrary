#pragma once


#include <File/FileUtil.h>
#include <filesystem>


namespace fs
{
	namespace FileUtil
	{
		const bool isFile(const char* const fileName) noexcept
		{
			return std::filesystem::is_regular_file(fileName);
		}

		const bool isDirectory(const char* const fileName) noexcept
		{
			return std::filesystem::is_directory(fileName);
		}

		inline const bool isReadOnly(const char* const fileName) noexcept
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
				// readonly file
				return true;
			}
			return false;
		}

		inline const bool exists(const char* const fileName) noexcept
		{
			auto status{ std::filesystem::status(fileName) };
			auto type{ status.type() };
			if (type == std::filesystem::file_type::not_found)
			{
				return false;
			}
			return true;
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

		const bool deleteFile(const char* const fileName) noexcept
		{
			if (exists(fileName) == true)
			{
				return std::filesystem::remove(fileName);
			}
			return true;
		}
	}
}
