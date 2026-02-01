#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int16_t i16;

/* I don't expect all of it to be useful but it's nice to look at*/
typedef struct {
	u32 fileSize;
	u32 formatSize;
	u16 formatTag;
	u16 channels;
	u32 samplesPerSec;
	u32 avgBytesPerSec;
	u16 blockAlign;
	u16 bitsPerSample;
	u64 whereDataAt;
} wav_metadata;
