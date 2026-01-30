#include "types.h"
#include <stdio.h>
#include <stdlib.h>

/* Takes pointer to chunk, frees it of suffering */
void free_chunk(chunk *chonk)
{
	free(chonk->ckData);
	free(chonk);
}

/* Turn raw 4-char-code into null-terminated string */
char *convert_id(u32 id)
{
	fourcc_converter fccc = {.u = id};
	fccc.c[4] = '\0';
	char *code = fccc.c;
	return code;
}

/* Goes to nth byte, assumes chunk there, retrieves it */
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
	u8 *data = malloc(size); // Lost my heap allocation virginity right here
	size_t body_read_ret = fread(data, sizeof(u8), size, f);
	if (body_read_ret != size) {
		if (feof(f)) {
			fprintf(
			    stdout,
			    "!!reached EOF in %s while parsing chunk %s!!\n",
			    path, convert_id(id));
		} else if (ferror(f)) {
			perror("ERROR");
			return NULL;
		}
	}

	chunk *out_chunk = malloc(sizeof(u32) * 2 + sizeof(u8 *));
	out_chunk->ckID = id;
	out_chunk->ckSize = size;
	out_chunk->ckData = data;
	return out_chunk;
}

/* Prints out chunk's fields */
void printck(chunk *chonk)
{
	char *fcc = convert_id(chonk->ckID);

	printf("\nID: %s\n", fcc);
	printf("Size: %d\n", chonk->ckSize);
	printf("Data: %X %X %X ...\n", *(chonk->ckData), *(chonk->ckData + 1),
	       *(chonk->ckData + 2));
}

int main(void)
{
	chunk *ck = yoink_chunk(0, "test.wav");
	chunk *ck2 = yoink_chunk(8, "test.wav");
	printck(ck);
	printck(ck2);
	return EXIT_SUCCESS;
}
