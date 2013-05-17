// This is the main DLL file.

#include <cstdio>
#include <cassert>
#include <iostream>

#include "defines.h"
#include "Archive.h"
#include "FileInfo.h"

namespace hogg {

Archive::~Archive() {
	for (int i = 0; i < file_infos_.size(); i++) {
		if (file_infos_[i] != NULL) {
			delete file_infos_[i];
			file_infos_[i] = NULL;
		}
	}
	if (file_ != NULL) {
		fclose(file_);
		file_ = NULL;
	}

	if (string_table_data_ != NULL) {
		delete [] string_table_data_;
		string_table_data_ = NULL;
	}

	if (datalist_data_ != NULL) {
		delete [] datalist_data_;
		datalist_data_ = NULL;
	}
}

bool Archive::InitHeader() {
	ArchiveHeader header;

	// Open file for reading in binary mode
	file_ = fopen(filename_, "rb");
	if (file_ == NULL) {
		perror("Failed to open archive");
		return false;
	}

	// Go to beginning of the file
	if (fseek(file_, 0, SEEK_SET) != 0) {
		perror("Failed to seek to the beginning of an archive");
		return false;
	}

	// Read in Archive header
	fread(&header, sizeof(header), 1, file_);

	// Validate header
	if (header.magic_number != hogg::HOGG_MAGIC) {
		std::cerr << "hogg magic number mismatch" << std::endl;
		return false;
	}

	if (header.data_entry_section_size != (2 * header.compression_info_section_size)) {
		std::cerr << "data entry / compression info section size mismatch" << std::endl;
		return false;
	}

	num_files_ = header.compression_info_section_size >> 4; // compression_info_section_size = 16 * num_files_
	file_infos_.resize(num_files_);

	return true;
}

bool Archive::InitFilePaths() {
	std::vector<const char*> data_list;

	// Read DataList file into memory
	auto dataListFile = file_infos_[0];
	datalist_data_ = new char[dataListFile->file_size()];
	dataListFile->ReadData(datalist_data_, dataListFile->file_size());

	// Iterate over DataList path names and assign them to the corresponding FileInfo (Nth string belongs to Nth FileInfo)
	int numDataListStrings = *(int*)&datalist_data_[4];
	data_list.resize(numDataListStrings);
	int dataListOffset = 8;

	int i;
	for (i = 0; i < numDataListStrings; i++) {		
		int strLen = *(int*)&datalist_data_[dataListOffset];
		data_list[i] = &datalist_data_[dataListOffset+4];

		// auto fi = file_infos_[i];
		// fi->path(std::string(&datalist_data_[dataListOffset+4]));
		// file_info_path_map_.insert(std::make_pair(std::string(&datalist_data_[dataListOffset+4]), fi));

		dataListOffset += (4 + strLen);
	}

	// Read File Paths from the hogg file's string table
	if (fseek(file_, hogg::HOGG_STRINGTABLE_OFFSET, SEEK_SET) != 0) {
		perror("Failed to seek to the string table section");
		return false;
	}

	int string_table_size;
	fread(&string_table_size, sizeof(int), 1, file_);

	if (fseek(file_, 4, SEEK_CUR) != 0) {
		perror("Failed to skip second string table length");
		return false;
	}

	// Assign paths to the appropriate FileInfos
	string_table_data_ = new char[string_table_size];
	fread(string_table_data_, sizeof(char), string_table_size, file_);

	hogg::DataTableEntry* entry;
	unsigned int stringTablePosition = 0;
	while (stringTablePosition < string_table_size) {
		entry = (hogg::DataTableEntry*)(string_table_data_ + stringTablePosition);
		data_list.push_back(&entry->data);

		// auto fi = file_infos_[entry->index];
		// fi->path(std::string(&entry->data));
		// file_info_path_map_.insert(std::make_pair(std::string(&entry->data), fi));

		if (entry->unknown_0 != 1) { // Either an invalid entry.. or the last entry.. entry->data_length is invalid it seems
			stringTablePosition += 9;	// offset to next entry
		} else {
			stringTablePosition += (9 + entry->data_length);	// offset to next entry
		}
	}

	// Assign all the file's paths and fill in the path->FileInfo map
	for (i = 0; i < file_infos_.size(); i++) {
		auto fi = file_infos_[i];
		auto filePathStringId = fi->file_path_string_id();
		auto filePath = data_list[filePathStringId];
		fi->path(std::string(filePath));
		file_info_path_map_.insert(std::make_pair(std::string(filePath), fi));
	}

	return true;
}

bool Archive::InitFiles() {
	// Create FileInfo structures from the data entries within the hogg
	if (fseek(file_, hogg::HOGG_DATAENTRIES_OFFSET, SEEK_SET) != 0) {
		perror("Failed to seek to the data entries section");
		return false;
	}

	char* dataEntryData = new char[sizeof(hogg::DataEntry) * num_files_];
	fread(dataEntryData, sizeof(hogg::DataEntry), num_files_, file_);

	hogg::DataEntry* dataEntry = (hogg::DataEntry*)dataEntryData;

	int i;
	for (i = 0; i < num_files_; i++) {
		auto fi = new FileInfo(this);
		fi->data_size(dataEntry->size_on_disk);
		fi->file_offset(dataEntry->file_offset);
		fi->last_updated(dataEntry->timestamp);

		file_infos_[i] = fi;
		dataEntry++;
	}

	delete [] dataEntryData;

	// Set the decompressed size of FileInfos which are compressed
	char* compressionData = new char[sizeof(hogg::CompressionInfo) * num_files_];
	fread(compressionData, sizeof(hogg::CompressionInfo), num_files_, file_);

	hogg::CompressionInfo* compressionInfo = (hogg::CompressionInfo*)compressionData;

	for (i = 0; i < num_files_; i++) {
		auto fi = file_infos_[i];
		fi->file_path_string_id(compressionInfo->file_path_id);
		fi->decompressed_size(compressionInfo->uncompressed_size);

		compressionInfo++;
	}

	delete [] compressionData;

	return true;
}

bool Archive::Init() {
	if (!InitHeader()) {
		std::cerr << "InitHeader failed" << std::endl;
		goto on_error;
	}

	if (!InitFiles()) {
		std::cerr << "InitFiles failed" << std::endl;
		goto on_error;
	}

	if (!InitFilePaths()) {
		std::cerr << "InitFilePaths failed" << std::endl;
		goto on_error;
	}

	return true;

on_error:
	if (file_ != NULL) {
		fclose(file_);
		file_ = NULL;
	}

	return false;
}

Archive* Archive::Open(const char* filename) {
	if (filename == NULL) {
		return NULL;
	}

	Archive* result = new Archive(filename);
	if (!result->Init()) {
		delete result;
		return NULL;
	}

	return result;
}

FileInfo* Archive::FindFile(const std::string& path) {
	auto it = file_info_path_map_.find(path);
	if (it != file_info_path_map_.end()) {
		return it->second;
	}

	return NULL;
}

} // namespace hogg
