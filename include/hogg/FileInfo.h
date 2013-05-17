#ifndef _HOGG_FILEINFO_H_
#define _HOGG_FILEINFO_H_

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace hogg {

class Archive;

class FileInfo {
	friend class hogg::Archive;

public:
	FileInfo(Archive* archive)
		: archive_(archive),
		data_size_(0),
		decompressed_size_(0),
		file_offset_(0),
		file_path_string_id_(0),
		last_updated_(0) { }

	virtual ~FileInfo();

	const Archive* archive() const { return archive_; }

	// Path of the file
	const std::string& path() const { return path_; }

	// Offset within the archive to the file data
	int32_t file_offset() const { return file_offset_; }

	// Retrieve size of file within the archive
	int32_t data_size() const { return data_size_; }

	// Retrieve size of file (possibly after decompression)
	int32_t file_size() const { return is_compressed() ? decompressed_size_ : data_size_; }

	// Whether or not the file is stored in a compressed format
	bool is_compressed() const { return decompressed_size_ > 0; }

	// Size of the decompressed file or 0 if the file is not compressed
	int32_t decompressed_size() const { return decompressed_size_; }

	// Time the file was updated (in seconds since Jan 1 1970)
	__time32_t last_updated() const { return last_updated_; }

	// Whether or not this FileInfo points to valid file data
	bool is_valid() const { return data_size_ > 0; }

	int32_t file_path_string_id() const { return file_path_string_id_; }

	bool ReadData(void* buffer, int32_t buffer_size);

private:
	FileInfo(const FileInfo& n) { }

	void path(std::string val) { path_ = val; }

	void file_offset(int32_t val) { file_offset_ = val; }

	void data_size(int32_t val) { data_size_ = val; }

	void decompressed_size(int32_t val) { decompressed_size_ = val; }

	void last_updated(__time32_t val) { last_updated_ = val; }

	void file_path_string_id(int32_t val) { file_path_string_id_ = val; }

	// The hogg archive to which this node belongs
	Archive* archive_;

	int32_t file_path_string_id_;

	// 32-bit offset within the archive where this node's data resides
	int32_t file_offset_;

	// Size, in bytes, of the file's data within the archive
	int32_t data_size_;

	// Size, in bytes, of the file data after decompression
	int32_t decompressed_size_;

	// Path of the file
	std::string path_;

	// Time the file was last updated
	__time32_t last_updated_;
};

} // namespace ggpk

#endif
