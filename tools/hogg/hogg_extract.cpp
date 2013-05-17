#include <iostream>
#include <fstream>

#include "hogg.h"
#include "hogg/Archive.h"
#include "hogg/FileInfo.h"

using namespace std;

// Extract a file from the archive and save it to disk
int doExtract(hogg::Archive* archive, const char* path, const char* outPath) {
	// Find the FileInfo if it exists
	auto file = archive->FindFile(path);
	if (file == NULL) {
		cerr << "Failed to extract " << path << ": File not found" << endl;
		return 2;
	}

	// Create/truncate the destination file
	ofstream outFile;
	outFile.open(outPath, ios::out | ios::binary | ios::trunc);

	if (!outFile.is_open()) {
		cerr << "Failed to extract " << path << ": Could not open " << outPath << " for writing" << endl;
		return 3;
	}

	// Read the data from the archive into a buffer
	int32_t dataSize = file->file_size();
	char* buffer = new char[dataSize];
	file->ReadData(buffer, dataSize);

	// Write the buffer to the output
	outFile.write(buffer, dataSize);
	outFile.close();

	cout << path << " saved to " << outPath << endl;

	delete [] buffer;

	return 0;
}