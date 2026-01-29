#include "types.h"
#include <stdio.h>

/* Goes to nth byte, assumes a chunk there, retrieves it */
chunk *yoink_chunk(u32 offset, char *path)
{
	FILE *f = fopen(path, "rb");
	if (f == NULL) {
		fprintf(stderr, "ERROR: couldn't read file %s\n", path);
		return NULL;
	}

	if (fseek(f, offset, SEEK_SET) != 0) {
		fprintf(stderr, "ERROR: couldn't go to line %d in file %s\n",
			__LINE__ - 2, __FILE__);
		return NULL;
	}
	u32 header[2];
	size_t head_read_ret = fread(header, sizeof(u32), 2, f);
	if (head_read_ret != 2) {
		if (feof(f)) {
			fprintf(stderr, "ERROR: unexpected EOF in %s\n", path);
			return NULL;
		} else if (ferror(f)) {
			perror("ERROR");
			return NULL;
		}
	}
	u32 id = header[0];
	u32 size = header[1];

	if (fseek(f, offset + sizeof(header), SEEK_SET) != 0) {
		fprintf(stderr, "ERROR: couldn't go to line %d in file %s\n",
			__LINE__ - 2, __FILE__);
		return NULL;
	}
	u8 data[size];
	size_t body_read_ret = fread(data, sizeof(u8), size, f);
	if (body_read_ret != size) {
		if (feof(f)) {
			fprintf(stderr, "ERROR: unexpected EOF in %s\n", path);
			return NULL;
		} else if (ferror(f)) {
			perror("ERROR");
			return NULL;
		}
	}
	/*
	 * TODO: put data on heap
	 */
}
