/*
 * feal.c
 *
 *  Created on: 22.04.2016
 *      Author: Lukas
 */

#include "feal.h"

uint8_t rotateLeft(uint8_t byte, uint8_t amountOfBits)
{
	if(amountOfBits > 8)
		return -1; // Muss geändert werden.
	uint8_t mask = 0xFF;
	mask = mask << (8 - amountOfBits);
	uint8_t carryover = byte & mask;
	carryover = carryover >> (8 - amountOfBits);
	byte = byte << amountOfBits;
	return (byte + carryover);
}

uint8_t s(uint8_t x, uint8_t y, Index_t i)
{
	return rotateLeft((x+y+i)%256 , 2); // stimmt vllt nicht, da datentyp uint8_t
}

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
	c1 = s(d1, d2 ^ b[0], ONE);
	c2 = s(d2, c1 ^ b[1], ZERO);
	c0 = s(a[0], c1 ^ b[2], ZERO);
	c3 = s(a[3], c2 ^ b[3], ONE);

	uint32_t c = bytesToUint32(c0, c1, c2, c3);
	return c;
}

uint32_t f(uint32_t aWord, uint16_t b)
{
	uint8_t b1 = b >> 8;
	uint8_t b2 = b;
	uint8_t a[4] = {0};
	uint8_t c0, c1, c2, c3, d1, d2;

	// Split a to a0, a1, a2, a3
	splitToBytes(aWord, a);

	d1 = a[0] ^ a[1] ^ b1;
	d2 = a[2] ^ a[3] ^ b2;
	c1 = s(d1, d2, ONE);
	c2 = s(d2, c1, ZERO);
	c0 = s(a[0], c1, ZERO);
	c3 = s(a[3], c2, ONE);

	return bytesToUint32(c0, c1, c2, c3);
}

void splitToBytes(uint32_t word, uint8_t *buf)
{
	for(int i = 0; i < 4; ++i)
	{
		buf[i] = word & 0x000000FF;
		word = word >> 8;
	}
}

uint32_t bytesToUint32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
	uint32_t ret = b3;
	ret = ret << 8;
	ret = ret + b2;
	ret = ret << 8;
	ret = ret + b1;
	ret = ret << 8;
	ret = ret + b0;
	return ret;
}

uint16_t *compSubKeys(uint64_t key)
{
	uint16_t *subkeys = malloc(12);

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

uint64_t encode(uint64_t p, uint16_t *k)
{
	uint32_t l[5];
	uint32_t r[5];

	uint32_t pL = p >> 32;
	uint32_t pR = p;

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
		l[i-1] = r[i] ^ f(r[i-1], k[i-1]); // Inverse von f???
	}

	uint32_t pL = l[0] ^ k45;
	uint32_t pR = r[0] ^ pL ^ k45 ^ k67;

	uint64_t p = pL;
	p = (p << 32) + pR;

	return p;
}
