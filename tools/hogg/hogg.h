#pragma once

namespace hogg_options {
	enum CommandOptions {
		None,
		Info,
		List,
		Extract
	};
}

namespace hogg {
	class Archive;
}

int doNone(hogg::Archive* archive);
int doInfo(hogg::Archive* archive, const char* path);
int doList(hogg::Archive* archive);
int doExtract(hogg::Archive* archive, const char* path, const char* outPath);