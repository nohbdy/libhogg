#include <iostream>
#include <iomanip>

#include "hogg.h"
#include "hogg/Archive.h"
#include "hogg/FileInfo.h"

using namespace std;

// Write information about a given file within the archive
int doInfo(hogg::Archive* archive, const char* path) {
	// Find the FileInfo if it exists
	auto file = archive->FindFile(path);
	if (file == NULL) {
		cerr << "File not found" << endl;
		return 2;
	}

	// Print the following information:
	// <path>
	// offset <file_offset>
	// size on disk <data_size>
	// compressed <is_compressed>
	// file size <file_size>
	// last update <last_updated>
	cout << file->path() << endl;
	cout << "offset 0x" << hex << file->file_offset() << dec << endl;
	cout << "size on disk " << file->data_size() << endl;
	cout << "compressed " << file->is_compressed() << endl;
	cout << "file size " << file->file_size() << endl;
	auto update_time = file->last_updated();
	cout << "last update " << _ctime32(&update_time) << endl;

	return 0;
}