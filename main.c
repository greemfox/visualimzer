#include "types.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
	long where_data_at;

	FILE *f = fopen(path, "rb");
	if (fgetc(f) != 'R' || fgetc(f) != 'I' || fgetc(f) != 'F' ||
	    fgetc(f) != 'F') {
		char what[4];
		fseek(f, 0, SEEK_SET);
		fread(what, 4, 1, f);
		fprintf(stderr, "ERROR in %s: expected RIFF, found %c%c%c%c\n",
			path, what[0], what[1], what[2], what[3]);
		return false;
	}
	fread(&file_size, 4, 1, f);
	file_size += 8; // ACCOUNTING FOR HEADER & POSSIBLE PADDING
	if (file_size % 2 != 0) {
		file_size++;
	}

	if (fgetc(f) != 'W' || fgetc(f) != 'A' || fgetc(f) != 'V' ||
	    fgetc(f) != 'E') {
		fprintf(stderr, "ERROR in %s: not a WAVE file\n", path);
		return false;
	}
	if (fgetc(f) != 'f' || fgetc(f) != 'm' || fgetc(f) != 't' ||
	    fgetc(f) != ' ') {
		fprintf(stderr, "ERROR in %s: couldn't find the fmt chunk\n",
			path);
		return false;
	}
	fread(&format_size, 4, 1, f);
	fread(&format_tag, 2, 1, f);
	if (format_tag != 1) {
		fprintf(stderr, "ERROR in %s: IBM formats are not supported\n",
			path);
		return false;
	}
	fread(&channels, 2, 1, f);
	fread(&samples_per_sec, 4, 1, f);
	fread(&avg_bytes_per_sec, 4, 1, f);
	fread(&block_align, 2, 1, f);
	fread(&bits_per_sample, 2, 1, f);

	u8 list_maybe[4];
	u32 info_skip = -4; // To go back if none is found
	fread(list_maybe, 4, 1, f);
	if (list_maybe[0] == 'L' && list_maybe[1] == 'I' &&
	    list_maybe[2] == 'S' && list_maybe[3] == 'T') {
		fread(&info_skip, 4, 1, f);
	}
	fseek(f, info_skip, SEEK_CUR);

	if (fgetc(f) != 'd' || fgetc(f) != 'a' || fgetc(f) != 't' ||
	    fgetc(f) != 'a') {
		fprintf(stderr, "ERROR in %s: couldn't find the data chunk\n",
			path);
		return false;
	}
	where_data_at = ftell(f) + 4;

	*md = (wav_metadata){
	    file_size,	 format_size,	  format_tag,
	    channels,	 samples_per_sec, avg_bytes_per_sec,
	    block_align, bits_per_sample, where_data_at,
	};
	return true;
}

void printmd(wav_metadata *md)
{
	printf("File size: %u bytes\n", md->fileSize);
	printf("Format size: %u bytes\n", md->formatSize);
	printf("Format tag: %04x\n", md->formatTag);
	printf("Channels: %u\n", md->channels);
	printf("Samples per sec: %u Hz\n", md->samplesPerSec);
	printf("Avg bytes per sec: %u bytes\n", md->avgBytesPerSec);
	printf("Block alignment: %u bytes\n", md->blockAlign);
	printf("Bits per sample: %u\n", md->bitsPerSample);
	printf("Actual data starts after: %li bytes\n", md->whereDataAt);
}

sample_s16 *yoink_samples(char *path, wav_metadata *md, int *count_out)
{
	if (md->bitsPerSample != 16 || md->channels != 2) {
		fprintf(stderr, "We only do 16-bit stereo here, sorry\n");
		return NULL;
	}

	long data_size = md->fileSize - md->whereDataAt;
	sample_s16 *samples = malloc(data_size);
	int sample_count = data_size / 4;
	*count_out = sample_count;

	FILE *f = fopen(path, "rb");
	fseek(f, md->whereDataAt, SEEK_SET);
	fread(samples, sizeof(sample_s16), sample_count, f);
	return samples;
}

void printpcm(int offset, int count, sample_s16 *data)
{
	sample_s16 *start = data + offset;
	for (int i = 0; i < count; i++) {
		sample_s16 s = *(start + i);
		printf(" Sample %i\n", offset + i + 1);
		printf("Channel 0: %i\n", s.chan0);
		printf("Channel 1: %i\n", s.chan1);
	}
}

void do_the_thing(sample_s16 *samples, int count)
{
	InitWindow(510, 510, "test");
	Color couleur = {0, 0, 0, 255};
	while (!WindowShouldClose()) {
		for (int i = 0; i < count; i++) {
			sample_s16 s = samples[i];
			couleur.r = 255 * sin(time(NULL));
			couleur.g = 255 * s.chan0 / 65536 + .5;
			couleur.b = 255 * s.chan1 / 65536 + .5;
			BeginDrawing();
			ClearBackground(couleur);
			EndDrawing();
		}
	}
	CloseWindow();
}

int main(int argc, char **argv)
{
	if (argc == 1) {
		printf("Usage: vmzr [.WAV FILE PATH] ...\n");
	}
	for (int i = 1; i < argc; i++) {
		char *path = argv[i];
		wav_metadata md;
		if (extract_metadata(path, &md)) {
			int scnt;
			sample_s16 *samples = yoink_samples(path, &md, &scnt);

			printmd(&md);
			do_the_thing(samples, scnt);
			free(samples);
		} else {
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
