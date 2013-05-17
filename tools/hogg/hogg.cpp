#include <iostream>
#include <iomanip>
#include <string>

#include "hogg.h"
#include "hogg/Archive.h"

using namespace std;

void PrintHelpMessage() {
	cout << "Extract information from or about hogg archives" << endl << endl;
	cout << "hogg [-ilx] path_to_archive [source_path] [destination_path]" << endl << endl;
	cout << "Options:" << endl;
	cout << " -i | --info : Print information about a file within the archive.  source_path specifies the path of the file within the archive." << endl;
	cout << " -l | --list : Print a list of all the files which are contained within the archive" << endl;
	cout << " -x | --extract : Extract a file from the archive.  source_path specifies the path of the file within the archive.  destination_path specifies the path to where the file data should be saved" << endl;
}

bool ParseCommandLine(int argc, char* argv[], hogg_options::CommandOptions* option, const char** path_to_archive, const char** source_path, const char** destination_path) {
	if (argc < 2) {
		PrintHelpMessage();
		return false;
	}

	*source_path = NULL;
	*destination_path = NULL;

	if (argv[1][0] != '-') {
		(*option) = hogg_options::None;
		(*path_to_archive) = argv[1];
		return true;
	}

	string cmd(argv[1]);
	int reqargc;

	if ((cmd == "-i") || (cmd == "--info")) {
		*option = hogg_options::Info;
		reqargc = 4; // exe cmd archive source
	} else if ((cmd == "-l") || (cmd == "--list")) {
		*option = hogg_options::List;
		reqargc = 3; // exe cmd archive
	} else if ((cmd == "-x") || (cmd == "--extract")) {
		*option = hogg_options::Extract;
		reqargc = 5; // exe cmd archive source dest
	} else {
		PrintHelpMessage();
		return false;
	}

	if (argc < reqargc) {
		PrintHelpMessage();
		return false;
	}

	*path_to_archive = argv[2];

	if (reqargc < 4) { return true; }
	*source_path = argv[3];

	if (reqargc < 5) { return true; }
	*destination_path = argv[4];

	return true;
}

int main(int argc, char* argv[])
{
	hogg_options::CommandOptions op;
	const char* path_to_archive;
	const char* source_path;
	const char* destination_path;

	if (!ParseCommandLine(argc, argv, &op, &path_to_archive, &source_path, &destination_path)) {
		return 1;
	}

	hogg::Archive* archive = hogg::Archive::Open(path_to_archive);
	if (archive == NULL) {
		cerr << "Unable to open hogg archive " << path_to_archive << endl;
		return 1;
	}

	int result;
	switch (op) {
	case hogg_options::None:
		result = doNone(archive);
		break;
	case hogg_options::Info:
		result = doInfo(archive, source_path);
		break;
	case hogg_options::List:
		result = doList(archive);
		break;
	case hogg_options::Extract:
		result = doExtract(archive, source_path, destination_path);
		break;
	}

	delete archive;
	return result;
}

