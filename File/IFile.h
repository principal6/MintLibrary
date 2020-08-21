#pragma once


#ifndef FS_IFILE_H
#define FS_IFILE_H


namespace fs
{
	class IFileReader abstract
	{
	public:
						IFileReader() = default;
		virtual			~IFileReader() = default;

	public:
		virtual bool	load(const char* const fileName) abstract;
	};


	class IFileWriter abstract
	{
	public:
						IFileWriter() = default;
		virtual			~IFileWriter() = default;

	public:
		virtual bool	save(const char* const fileName) abstract;
	};
}


#endif // !FS_IFILE_H
