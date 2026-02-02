#include "types.h"
#include <stdio.h>
#include <stdlib.h>

bool extract_metadata(char *path, wav_metadata *md)
{
	u32 file_size;
	u32 format_size;
	u16 format_tag;
	u16 channels;
	u32 samples_per_sec;
	u32 avg_bytes_per_sec;
	u16 block_align;
	u16 bits_per_sample;
	u32 info_size;
	long where_data_at;

	FILE *f = fopen(path, "rb");
	if (fgetc(f) != 'R' || fgetc(f) != 'I' || fgetc(f) != 'F' ||
	    fgetc(f) != 'F') {
		char what[5];
		what[4] = '\0';
		fseek(f, 0, SEEK_SET);
		fread(what, 4, 1, f);
		fprintf(stderr, "ERROR in %s: expected RIFF, found %s\n", path,
			what);
		return false;
	}
	fread(&file_size, 4, 1, f);
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
	fread(&format_tag, 2, 1, f);
	if (format_tag != 1) {
		fprintf(stderr,
			"Can't parse %s: IBM formats are not supported\n",
			path);
		return false;
	}
	fread(&channels, 2, 1, f);
	fread(&samples_per_sec, 4, 1, f);
	fread(&avg_bytes_per_sec, 4, 1, f);
	fread(&block_align, 2, 1, f);
	fread(&bits_per_sample, 2, 1, f);
	fseek(f, 4, SEEK_CUR); // Skipping LIST fourcc
	fread(&info_size, 4, 1, f);
	fseek(f, info_size, SEEK_CUR); // Skipping info
	if (fgetc(f) != 'd' || fgetc(f) != 'a' || fgetc(f) != 't' ||
	    fgetc(f) != 'a') {
		fprintf(stderr, "ERROR: couldn't find the data chunk in %s\n",
			path);
		return false;
	}
	where_data_at = ftell(f);

	*md = (wav_metadata){
	    file_size,	 format_size,	  format_tag,
	    channels,	 samples_per_sec, avg_bytes_per_sec,
	    block_align, bits_per_sample, where_data_at,
	};
	return true;
}

void printmd(wav_metadata *md)
{
	printf("File size: %i bytes\n", md->fileSize);
	printf("Format size: %i bytes\n", md->formatSize);
	printf("Format tag: %04x\n", md->formatTag);
	printf("Channels: %i\n", md->channels);
	printf("Samples per sec: %i Hz\n", md->samplesPerSec);
	printf("Avg bytes per sec: %i bytes\n", md->avgBytesPerSec);
	printf("Block alignment: %i bytes\n", md->blockAlign);
	printf("Bits per sample: %i\n", md->bitsPerSample);
	printf("Actual data starts after: %li bytes\n", md->whereDataAt);
}

/* Bit of a questionable decision to open the file both times ngl */
void parse_data(char *path, long offset, wav_metadata *md)
{
	return;
}

int main(int argc, char **argv)
{
	if (argc == 1) {
		printf("Usage: vmzr [.WAV FILE PATH] ...\n");
	}
	for (int i = 1; i < argc; i++) {
		wav_metadata md;
		extract_metadata(argv[i], &md);
		printmd(&md);
	}
	return EXIT_SUCCESS;
}
