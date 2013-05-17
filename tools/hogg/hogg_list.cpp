#include <vector>
#include <iostream>

#include "hogg.h"
#include "hogg/Archive.h"
#include "hogg/FileInfo.h"

using namespace std;

// Print a list of all files that exist within the ggpk Archive
int doList(hogg::Archive* archive) {
	auto all_files = archive->all_files();
	for (auto it = all_files.begin(); it < all_files.end(); it++) {
		auto fi = *it;
		if (fi->is_valid()) {
			std::cout << fi->path() << std::endl;
		}
	}

	return 0;
}