#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int16_t i16;

typedef struct {
	u32 fileSize;
	u32 formatSize;
	u16 formatTag;
	u16 channels;
	u32 samplesPerSec;
	u32 avgBytesPerSec;
	u16 blockAlign;
	u16 bitsPerSample;
	long whereDataAt; // Offset in bytes; long because ftell
} wav_metadata;

typedef struct {
	i16 chan0;
	i16 chan1;
} sample_s16;
