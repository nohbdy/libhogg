#include <iostream>

#include "hogg.h"
#include "hogg/Archive.h"

using namespace std;

//hogg::Node* TestFile(hogg::Archive* archive, const char* path) {
//	hogg::Node* node = archive->FindNode(path);
//	if (node != NULL) {
//		cout << "Found " << path << " at 0x" << hex << node->file_offset() << endl;
//		cout << " Data Size: " << dec << node->data_size() << " bytes" << endl;
//		return node;
//	} else {
//		cout << "Unable to find " << path << endl;
//		return NULL;
//	}
//}

int doNone(hogg::Archive* archive) {
	/*hogg::Node* root = archive->root();
	cout << "Archive " << archive->filename() << endl;
	cout << "Root Offset: 0x" << hex << root->file_offset() << endl;

	for (vector<hogg::Node*>::const_iterator it = root->child_nodes().begin(); it < root->child_nodes().end(); it++) {
		hogg::Node* c = *it;

		if (c->node_type() == hogg::Node::File) {
			cout << "FILE ";
		} else {
			cout << "PDIR ";
		}

		cout << c->name() << endl;
	}*/

	return 0;
}