#pragma once


#include <MintPlatform/Include/FileUtil.h>

#include <filesystem>


namespace mint
{
	namespace FileUtil
	{
		MINT_INLINE bool IsFile(const char* const fileName) noexcept
		{
			return std::filesystem::is_regular_file(fileName);
		}

		MINT_INLINE bool IsDirectory(const char* const fileName) noexcept
		{
			return std::filesystem::is_directory(fileName);
		}

		MINT_INLINE bool IsReadOnly(const char* const fileName) noexcept
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

		MINT_INLINE void UnsetReadOnly(const char* const fileName) noexcept
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

		MINT_INLINE bool Exists(const char* const path) noexcept
		{
			auto status{ std::filesystem::status(path) };
			auto type{ status.type() };
			if (type == std::filesystem::file_type::not_found)
			{
				return false;
			}
			return true;
		}

		MINT_INLINE bool CreateDirectory_(const char* const directoryName) noexcept
		{
			if (Exists(directoryName) == true)
			{
				return true;
			}
			return std::filesystem::create_directory(directoryName);
		}

		MINT_INLINE bool DeleteFile_(const char* const fileName) noexcept
		{
			if (Exists(fileName) == true)
			{
				return std::filesystem::remove(fileName);
			}
			return true;
		}
	}
}
