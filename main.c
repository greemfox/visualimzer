#include "types.h"
#include <stdio.h>
#include <stdlib.h>

bool parse_wav(char *path)
{
	FILE *f = fopen(path, "rb");
	u32 file_size;
	u32 format_size;
	u16 format_tag;
	u16 channels;
	u32 samples_per_sec;
	u32 avg_bytes_per_sec;
	u16 block_align;
	u16 bits_per_sample;
	u32 info_size;

	if (fgetc(f) != 'R' || fgetc(f) != 'I' || fgetc(f) != 'F' ||
	    fgetc(f) != 'F') {
		fprintf(stderr, "ERROR: %s is not a RIFF file\n", path);
		return false;
	}

	fread(&file_size, 4, 1, f);
	printf("File size: %i bytes\n", file_size);

	if (fgetc(f) != 'W' || fgetc(f) != 'A' || fgetc(f) != 'V' ||
	    fgetc(f) != 'E') {
		fprintf(stderr, "ERROR: %s is not a WAVE file\n", path);
		return false;
	}

	if (fgetc(f) != 'f' || fgetc(f) != 'm' || fgetc(f) != 't' ||
	    fgetc(f) != ' ') {
		fprintf(stderr, "ERROR: couldn't find the fmt chunk in %s\n",
			path);
		return false;
	}

	fread(&format_size, 4, 1, f);
	printf("Format size: %i bytes\n", format_size);

	fread(&format_tag, 2, 1, f);
	printf("Format tag: %04x\n", format_tag);
	if (format_tag != 1) {
		fprintf(stderr, "SORRY! IBM formats are not supported\n");
		return false;
	}

	fread(&channels, 2, 1, f);
	printf("Channels: %i\n", channels);

	fread(&samples_per_sec, 4, 1, f);
	printf("Samples per sec: %i Hz\n", samples_per_sec);

	fread(&avg_bytes_per_sec, 4, 1, f);
	printf("Avg bytes per sec: %i bytes\n", avg_bytes_per_sec);

	fread(&block_align, 2, 1, f);
	printf("Block alignment: %i bytes\n", block_align);

	fread(&bits_per_sample, 2, 1, f);
	printf("Bits per sample: %i\n", bits_per_sample);

	fseek(f, 4, SEEK_CUR);
	fread(&info_size, 4, 1, f);
	printf("Info size: %i bytes\n", info_size);

	fseek(f, info_size, SEEK_CUR);
	printf("Skipping info...\n");

	if (fgetc(f) != 'd' || fgetc(f) != 'a' || fgetc(f) != 't' ||
	    fgetc(f) != 'a') {
		fprintf(stderr, "ERROR: couldn't find the data chunk in %s\n",
			path);
		return false;
	}

	return true;
}

int main(void)
{
	parse_wav("test.wav");
	return EXIT_SUCCESS;
}
