#ifndef __LIBHOGG_DEFINES_H_
#define __LIBHOGG_DEFINES_H_

#include <cstdint>

namespace hogg {

	const int32_t HOGG_MAGIC = 0xDEADF00D;
	const int32_t HOGG_STRINGTABLE_OFFSET = 0x41C;
	const int32_t HOGG_DATAENTRIES_OFFSET = 0x10018;

	// Private struct to simplify reading the archive header
	// We must pack the struct so the read bytes align properly with the struct members
#ifdef __GNUC__
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif
	struct ArchiveHeader {
		int32_t magic_number;					// DEADF00D
		int32_t unknown;						// flags?  == 0x0400000A
		int32_t data_entry_section_size;		// Size (in bytes) of the data entry section
		int32_t compression_info_section_size;	// Size (in bytes) of the compression info section size
	};

	struct DataTableEntry {
		uint8_t unknown_0;		// Always 1?
		int32_t id;				// Data ID
		int32_t data_length;	// Length, in bytes, of the data in this entry
		char data;
	};

	struct DataEntry {
		int64_t file_offset;	// offset to the file's data
		int32_t size_on_disk;	// size of the filedata within the archive
		__time32_t timestamp;	// 32-bit timestamp (seconds since 1970)
		uint32_t unknown_10;	// hash?
		int32_t unknown_14;		// 0
		int32_t unknown_18;		// 0xFFFE
		uint32_t index;
	};

	struct CompressionInfo {
		int32_t file_path_id;	// Data ID of the file's path
		int32_t excerpt_id;		// Data ID of a data excerpt, or -1 if there is none
		int32_t uncompressed_size;	// Size of the data after decompression, or 0 if the file is not compressed
		int32_t unknown_C; // 0
	};

	// Reset the struct packing to default
#ifdef __GNUC__
#pragma pack()
#else
#pragma pack(pop)
#endif

}

#endif