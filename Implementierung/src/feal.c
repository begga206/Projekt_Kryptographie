/*
 * 	feal.c
 *
 *  Created on: 22.04.2016
 *  Author: Lukas
 */

#include "feal.h"

/**
 * Implementierung der S0 und S1 Funktion aus der Quelle
 */
uint8_t S(int x, int y, Index_t i)
{
	uint8_t byte = (uint8_t)((x + y + i) % 256);
	uint8_t mask = 0xC0;
	uint8_t carryover = byte & mask;
	byte = byte << 2;
	carryover = carryover >> 6;
	return(byte | carryover);
}

/**
 * Implementierung der fK Funktion aus der Quelle
 */
uint32_t fK(uint32_t aWord, uint32_t bWord)
{
	uint8_t a[4] = {0};
	uint8_t b[4] = {0};
	uint8_t c0, c1, c2, c3, d1, d2;

	//split word a and b.
	splitToBytes(aWord, a);
	splitToBytes(bWord, b);

	d1 = a[0] ^ a[1];
	d2 = a[2] ^ a[3];
	c1 = S(d1, d2 ^ b[0], ONE);
	c2 = S(d2, c1 ^ b[1], ZERO);
	c0 = S(a[0], c1 ^ b[2], ZERO);
	c3 = S(a[3], c2 ^ b[3], ONE);

	uint32_t c = bytesToUint32(c0, c1, c2, c3);
	return c;
}

/**
 * Implementierung der f Funktion aus der Quelle
 */
uint32_t f(uint32_t aDWord, uint16_t b)
{
	uint8_t b1 = (uint8_t) b;
	uint8_t b2 = (uint8_t)(b >> 8);
	uint8_t a[4] = {0};
	uint8_t c0, c1, c2, c3, d1, d2;

	// Split a to a0, a1, a2, a3
	splitToBytes(aDWord, a);

	d1 = a[0] ^ a[1] ^ b1;
	d2 = a[2] ^ a[3] ^ b2;
	c1 = S(d1, d2, ONE);
	c2 = S(d2, c1, ZERO);
	c0 = S(a[0], c1, ZERO);
	c3 = S(a[3], c2, ONE);

	return bytesToUint32(c0, c1, c2, c3);
}

/**
 * Splittet ein 32 bit uint in 4 bytes auf
 */
void splitToBytes(uint32_t dWord, uint8_t *buf)
{
	for(int i = 0; i < 4; ++i)
	{
		buf[i] = dWord & 0x000000FF;
		dWord = dWord >> 8;
	}
}

/**
 *
 */
uint32_t bytesToUint32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
	return (uint32_t)(((uint32_t)b3 << 24) |
					  ((uint32_t)b2 << 16) |
					  ((uint32_t)b1 << 8)  | b0);
}

/**
 * Compute the 12 16bit subkeys with the help of the 64 bit key
 */
uint16_t *compSubKeys(uint64_t key)
{
	uint16_t *subkeys = malloc(12 * sizeof(uint16_t));

	uint32_t keyR = key;
	uint32_t keyL = key >> 32;

	uint32_t b[9] = {0};
	b[0] = 0;
	b[1] = keyL;
	b[2] = keyR;

	for(int i = 3; i < 9; i++)
	{
		b[i] = fK(b[i-2], b[i-1]^b[i-3]);
	}

	for(int i = 1; i <= 6; ++i)
	{
		subkeys[2*(i-1)] = b[i+2] >> 16;
		subkeys[2*i-1]   = b[i+2];
	}

	return subkeys;
}

/**
 * Kodieren eines 64 bit Plaintextblocks
 */
uint64_t encode(uint64_t p, uint16_t *k)
{
	uint32_t l[5];
	uint32_t r[5];

	uint32_t pL = (uint32_t)(p >> 32);
	uint32_t pR = (uint32_t)(p & 0x00000000ffffffff);

	uint32_t k45 = k[4];
	k45 = (k45 << 16) + k[5];

	uint32_t k67 = k[6];
	k67 = (k67 << 16) + k[7];

	uint32_t k89 = k[8];
	k89 = (k89 << 16) + k[9];

	uint32_t k1011 = k[10];
	k1011 = (k1011 << 16) + k[11];

	l[0] = pL ^ k45;
	r[0] = pL ^ pR ^ k45 ^ k67;

	for(int i = 1; i < 5; ++i)
	{
		l[i] = r[i-1];
		r[i] = l[i-1] ^ f(r[i-1], k[i-1]);
	}

	uint32_t cL = r[4] ^ k89;
	uint32_t cR = r[4] ^ l[4] ^ k1011;

	uint64_t c = cL;
	c = (c << 32) + cR;

	return c;
}

/**
 * Dekodieren eines 64 bit Chiffreblocks
 */
uint64_t decode(uint64_t c, uint16_t * k)
{
	uint32_t l[5];
	uint32_t r[5];

	uint32_t cL = c >> 32;
	uint32_t cR = c;

	uint32_t k45 = k[4];
	k45 = (k45 << 16) + k[5];

	uint32_t k67 = k[6];
	k67 = (k67 << 16) + k[7];

	uint32_t k89 = k[8];
	k89 = (k89 << 16) + k[9];

	uint32_t k1011 = k[10];
	k1011 = (k1011 << 16) + k[11];


	r[4] = cL ^ k89;
	l[4] = cR ^ r[4] ^ k1011;

	for(int i = 4; i > 0; --i)
	{
		r[i-1] = l[i];
		l[i-1] = r[i] ^ f(r[i-1], k[i-1]);
	}

	uint32_t pL = l[0] ^ k45;
	uint32_t pR = r[0] ^ pL ^ k45 ^ k67;

	uint64_t p = pL;
	p = (p << 32) + pR;

	return p;
}
