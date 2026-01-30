#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef struct {
	u32 ckID;
	u32 ckSize;
	u8 *ckData; // Caller's gonna be responsible for freeing ts
} chunk;
typedef union {
	u32 u;
	char c[sizeof(u32) + 1];
} fourcc_converter; // Mind the endianness
