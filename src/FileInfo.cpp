#include <stdio.h>
#include <locale>
#include <sstream>
#include <cstdlib>
#include <iostream>

#define ZLIB_WINAPI
#include "zlib.h"

#include "defines.h"
#include "FileInfo.h"
#include "Archive.h"

namespace hogg {

FileInfo::~FileInfo() {
}

bool FileInfo::ReadData(void* buffer, int32_t buffer_size) {
	if (buffer_size < file_size()) {
		return false;
	}

	FILE* file = archive_->file();

	if (fseek(file, file_offset_, SEEK_SET) != 0) {
		perror("Seek failed when attempting to read file data");
		return -1;
	}

	if (is_compressed()) {
		char* compressedData = new char[data_size()];
		fread(compressedData, 1, data_size(), file);

		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;
		int ret = inflateInit(&strm);
		if (ret != Z_OK) {
			std::cerr << "zlib init failed" << std::endl;
		}

		strm.avail_in = data_size();
		strm.next_in = (Bytef*)compressedData;

		strm.avail_out = decompressed_size();
		strm.next_out = (Bytef*)buffer;

		ret = inflate(&strm, Z_FINISH);

		if (!((ret == Z_STREAM_END) || (strm.avail_in == 0))) {
			std::cerr << "Some data remaining in compressedBuffer or didn't receive Z_STREAM_END" << std::endl;
		}

		inflateEnd(&strm);
	} else {
		fread(buffer, 1, file_size(), file);
	}
}

} // namespace hogg
