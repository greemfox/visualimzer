#include "types.h"
#include <stdio.h>
#include <stdlib.h>

void fpos_log(FILE *f)
{
	printf("current file position: %lu\n", ftell(f));
}

bool parse_wav(char *path)
{
	FILE *f = fopen(path, "rb");
	u32 header_size;
	u32 format_size;
	u16 format_tag;
	u16 channels;
	u32 samples_per_sec;
	u32 avg_bytes_per_sec;
	u16 block_align;

	if (fgetc(f) != 'R' || fgetc(f) != 'I' || fgetc(f) != 'F' ||
	    fgetc(f) != 'F') {
		fprintf(stderr, "ERROR: %s is not a RIFF file\n", path);
		return false;
	}

	fread(&header_size, 4, 1, f);

	if (fgetc(f) != 'W' || fgetc(f) != 'A' || fgetc(f) != 'V' ||
	    fgetc(f) != 'E') {
		fprintf(stderr, "ERROR: %s is not a WAVE file\n", path);
		return false;
	}

	if (fgetc(f) != 'f' || fgetc(f) != 'm' || fgetc(f) != 't' ||
	    fgetc(f) != ' ') {
		fprintf(stderr, "ERROR: %s is missing the fmt chunk\n", path);
		return false;
	}

	fread(&format_size, 4, 1, f);
	fread(&format_tag, 2, 1, f);
	fread(&channels, 2, 1, f);
	fread(&samples_per_sec, 4, 1, f);
	fread(&avg_bytes_per_sec, 4, 1, f);
	fread(&block_align, 2, 1, f);

	printf("Header size: %i bytes\n", header_size);
	printf("Format size: %i bytes\n", format_size);
	printf("Format tag: %04x\n", format_tag);
	printf("Channels: %i\n", channels);
	printf("Samples per sec: %i Hz\n", samples_per_sec);
	printf("Avg bytes per sec: %i bytes\n", avg_bytes_per_sec);
	printf("Block align: %i bytes\n", block_align);

	return true;
}

int main(void)
{
	parse_wav("test.wav");
	return EXIT_SUCCESS;
}
