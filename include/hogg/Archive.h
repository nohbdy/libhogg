#ifndef _HOGG_ARCHIVE_H_
#define _HOGG_ARCHIVE_H_

#pragma once

#include <cstdint>
#include <map>
#include <vector>

namespace hogg {

class FileInfo;

typedef std::map<std::string, FileInfo*> FilePathToFileInfoMap;

// Encapsulates a hogg Archive
class Archive
{
public:
	~Archive();

	// Create an Archive object for the hogg archive located at the provided file path
	static Archive* Open(const char* filename);

	// Retrieve a file from the archive with the given path
	// If no file exist with the given path, FindFile will return NULL.
	FileInfo* FindFile(const std::string& path);

	// Get the archive file handle
	// Using the file handle is not thread safe
	FILE* file() const { return file_; }

	// Returns the path to the archive file
	const char* filename() const { return filename_; }

	const std::vector<FileInfo*>& all_files() const { return file_infos_; }

private:
	explicit Archive(const char* filename) : filename_(filename), string_table_data_(NULL), datalist_data_(NULL), file_(NULL) { };

	Archive(const Archive& a) { }

	// Initialize the Archive object by reading in the file header and some initial data
	bool Init();

	bool InitHeader();
	bool InitFiles();
	bool InitFilePaths();

	// String Table
	char* string_table_data_;

	// Data List
	char* datalist_data_;

	// Vector of FileInfo pointers
	std::vector<FileInfo*> file_infos_;

	// Map path to FileInfo
	std::map<std::string, FileInfo*> file_info_path_map_;

	// How many files are contained in this archive
	int32_t num_files_;

	// File handle for the open archive file
	FILE* file_;

	// Filepath of the hogg archive
	const char* filename_;
};

}	// namespace ggpk

#endif
