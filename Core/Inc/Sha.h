/*=============================================================================
	Sha.h
	Unreal SHA Implementation
=============================================================================*/

//definitions
#define SHA256_BLOCK_LENGTH		64
#define SHA256_DIGEST_LENGTH		32
#define SHA256_DIGEST_STRING_LENGTH	(SHA256_DIGEST_LENGTH * 2 + 1)
#define SHA256_SHORT_BLOCK_LENGTH	(SHA256_BLOCK_LENGTH - 8)
#define MEMSET_BZERO(p,l)	memset((p), 0, (l))
#define MEMCPY_BCOPY(d,s,l)	memcpy((d), (s), (l))

/*** THE SIX LOGICAL FUNCTIONS ****************************************/
/*
 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
 *
 *   NOTE:  The naming of R and S appears backwards here (R is a SHIFT and
 *   S is a ROTATION) because the SHA-256/384/512 description document
 *   (see http://csrc.nist.gov/cryptval/shs/sha256-384-512.pdf) uses this
 *   same "backwards" definition.
 */
/* Shift-right (used in SHA-256, SHA-384, and SHA-512): */

#define ShiftR(b,x) 		((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define S32(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define S64(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512: */
#define ShaCh(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)	(S32(2,  (x)) ^ S32(13, (x)) ^ S32(22, (x)))
#define Sigma1_256(x)	(S32(6,  (x)) ^ S32(11, (x)) ^ S32(25, (x)))
#define sigma0_256(x)	(S32(7,  (x)) ^ S32(18, (x)) ^ ShiftR(3 ,   (x)))
#define sigma1_256(x)	(S32(17, (x)) ^ S32(19, (x)) ^ ShiftR(10,   (x)))

/* Four of six logical functions used in SHA-384 and SHA-512: */
#define Sigma0_512(x)	(S64(28, (x)) ^ S64(34, (x)) ^ S64(39, (x)))
#define Sigma1_512(x)	(S64(14, (x)) ^ S64(18, (x)) ^ S64(41, (x)))
#define sigma0_512(x)	(S64( 1, (x)) ^ S64( 8, (x)) ^ ShiftR( 7,   (x)))
#define sigma1_512(x)	(S64(19, (x)) ^ S64(61, (x)) ^ ShiftR( 6,   (x)))

#define REVERSE32(w,x)	{ \
	UINT tmp = (w); \
	tmp = (tmp >> 16) | (tmp << 16); \
	(x) = ((tmp & 0xff00ff00UL) >> 8) | ((tmp & 0x00ff00ffUL) << 8); \
}
#define REVERSE64(w,x)	{ \
	QWORD tmp = (w); \
	tmp = (tmp >> 32) | (tmp << 32); \
	tmp = ((tmp & 0xff00ff00ff00ff00ULL) >> 8) | \
	      ((tmp & 0x00ff00ff00ff00ffULL) << 8); \
	(x) = ((tmp & 0xffff0000ffff0000ULL) >> 16) | \
	      ((tmp & 0x0000ffff0000ffffULL) << 16); \
}
#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h)	\
	REVERSE32(*data++, W256[j]); \
	T1 = (h) + Sigma1_256(e) + ShaCh((e), (f), (g)) + \
             K256[j] + W256[j]; \
	(d) += T1; \
	(h) = T1 + Sigma0_256(a) + Maj((a), (b), (c)); \
	j++

#define ROUND256(a,b,c,d,e,f,g,h)	\
	s0 = W256[(j+1)&0x0f]; \
	s0 = sigma0_256(s0); \
	s1 = W256[(j+14)&0x0f]; \
	s1 = sigma1_256(s1); \
	T1 = (h) + Sigma1_256(e) + ShaCh((e), (f), (g)) + K256[j] + \
	     (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0); \
	(d) += T1; \
	(h) = T1 + Sigma0_256(a) + Maj((a), (b), (c)); \
	j++

const static UINT K256[64] = {
	0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
	0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
	0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
	0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
	0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
	0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
	0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
	0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
	0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
	0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
	0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
	0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
	0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
	0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
	0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
	0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

/* Initial hash value H for SHA-256: */
const static UINT sha256_initial_hash_value[8] = {
	0x6a09e667UL,
	0xbb67ae85UL,
	0x3c6ef372UL,
	0xa54ff53aUL,
	0x510e527fUL,
	0x9b05688cUL,
	0x1f83d9abUL,
	0x5be0cd19UL
};

typedef struct _SHA256_CTX {
	UINT	state[8];
	QWORD	bitcount;
	BYTE	buffer[SHA256_BLOCK_LENGTH];
} SHA256_CTX;

/*
 * Constant used by SHA256/384/512_End() functions for converting the
 * digest to a readable hexadecimal character string:
 */
CORE_API FString GenHash(FString InString);
CORE_API void SHA256_Init(SHA256_CTX *);
CORE_API void SHA256_Update(SHA256_CTX*, const BYTE*, size_t);
CORE_API void SHA256_Final(BYTE[SHA256_DIGEST_LENGTH], SHA256_CTX*);
CORE_API char* SHA256_End(SHA256_CTX*, char[SHA256_DIGEST_STRING_LENGTH]);
CORE_API char* SHA256_Data(const BYTE*, size_t, char[SHA256_DIGEST_STRING_LENGTH]);
CORE_API void SHA256_Transform(SHA256_CTX*, const UINT*);
