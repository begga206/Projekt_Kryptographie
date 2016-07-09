/*
 * 	feal.c
 *
 *  Created on: 22.04.2016
 *  Author: Lukas
 */

#include "feal.h"

/**
 * Implementierung der S0 und S1 Funktion aus dem Paper
 *
 * Si(x, y) = Rot_2 ( x + y + i (Mod 256))
 *
 * @param x
 * @param y
 * @param i - 0 oder 1
 *
 * @result 32 bit Zahl
 */
uint8_t S(int x, int y, Index_t i)
{
	uint8_t byte = (uint8_t)((x + y + i) % 256);

	// Rotate Left by 2.
	uint8_t mask = 0xC0;
	uint8_t carryover = byte & mask;
	byte = byte << 2;
	carryover = carryover >> 6;
	return(byte | carryover);
}

/**
 * Implementierung der fK Funktion aus dem Paper
 *
 * c = fK(a, b)
 *
 * @param aDWord - a
 * @param bDWord - b
 *
 * @return c
 */
uint32_t fK(uint32_t aDWord, uint32_t bDWord)
{
	uint8_t a[4] = {0};
	uint8_t b[4] = {0};
	uint8_t c0, c1, c2, c3, d1, d2;

	//split word a and b.
	splitToBytes(aDWord, a);
	splitToBytes(bDWord, b);

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
 * Implementierung der f Funktion aus dem Paper
 *
 * c = f(a, b)
 *
 * @param aDWord - a
 * @param b		 - b
 *
 * @result c (32 bit)
 */
uint32_t f(uint32_t aDWord, uint16_t b)
{
	uint8_t b2 = (uint8_t) b;
	uint8_t b1 = (uint8_t)(b >> 8);
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
 *
 * @param dWord - Das zu splittende Doppelwort
 * @param *buf  - Pointer auf die 4 bytes
 */
void splitToBytes(uint32_t dWord, uint8_t *buf)
{
	for(int i = 3; i >= 0; --i)
	{
		buf[i] = dWord & 0x000000FF;
		dWord = dWord >> 8;
	}

}

/**
 * Konkateniert 4 Bytes zu einem Doppelwort.
 *
 * @param b0 - Most significant byte
 * @param b1
 * @param b2
 * @param b3
 *
 * @result Das resultierende Doppelwort
 */
uint32_t bytesToUint32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
	return (uint32_t)(((uint32_t)b0 << 24) |
						  ((uint32_t)b1 << 16) |
						  ((uint32_t)b2 << 8)  | b3);
}

/**
 * Konkateniert zwei Woerter zu einem Doppelwort.
 *
 * @param w0 - Linke Haelfte des Doppelworts
 * @param w1 - Rechte Haelfte des Doppelworts
 *
 * @return Das resultierende Doppelwort
 */
uint32_t wordsToUint32(uint16_t w0, uint16_t w1)
{
	return (uint32_t)(((uint32_t)w0 << 16) | w1);
}

/**
 * Berrechnet die 12 16 bit Subkeys aus dem eigentlichen 64 bit Schluessel
 *
 * @param key - 64 bit Schluessel
 *
 * @return *k - Pointer auf die 12 16 bit Subkeys
 */
uint16_t *compSubKeys(uint64_t key)
{
	// Allokieren des Speicherbereichs.
	uint16_t *subkeys = malloc(12 * sizeof(uint16_t));

	// Aufspalten des Keys in 2 Doppelwoerter.
	uint32_t keyR = key;
	uint32_t keyL = (uint32_t)(key >> 32);

	// Initialisierung des zur Berechnung verwendeten Arrays.
	uint32_t b[9] = {0};
	b[0] = 0;
	b[1] = keyL;
	b[2] = keyR;

	// Berechnung der restlichen Plaetze von b.
	for(int i = 3; i < 9; i++)
	{
		b[i] = fK(b[i-2], b[i-1]^b[i-3]);
	}

	// Berechnung der Subkeys.
	for(int i = 1; i <= 6; ++i)
	{
		subkeys[2*(i-1)] = b[i+2] >> 16;
		subkeys[2*i-1]   = b[i+2];
	}

	return subkeys;
}

/**
 * Kodieren eines 64 bit Plaintextblocks
 *
 * @param p 	- 64 bit Plaintextblock
 * @param *k	- Pointer auf 16 bit Teilschluessel
 *
 * @return		- 64 bit Cipherblock
 */
uint64_t encode(uint64_t p, uint16_t *k)
{
	uint32_t l[5];
	uint32_t r[5];

	// Splitten des 64 bit Plaintextblock in 2 Doppelwoerter.
	uint32_t pL = (uint32_t)(p >> 32);
	uint32_t pR = (uint32_t)(p & 0x00000000ffffffff);

	// Konkatenation von 16 bit Schluesselpaaren zu einem Doppelwort.
	// TODO: Entspricht die Konkatenation dem Paper-Ausdruck?
	uint32_t k45 = wordsToUint32(k[4], k[5]);
	uint32_t k67 = wordsToUint32(k[6], k[7]);
	uint32_t k89 = wordsToUint32(k[8], k[9]);
	uint32_t k1011 = wordsToUint32(k[10], k[11]);

	// Beginn der eigentlichen Kodierung.
	l[0] = pL ^ k45;
	r[0] = pL ^ pR ^ k45 ^ k67;

	// Ausfuehren der 4 Feistel Runden
	for(int i = 1; i < 5; ++i)
	{
		l[i] = r[i-1];
		r[i] = l[i-1] ^ f(r[i-1], k[i-1]);
	}

	// Berechnung der 2 32 Bit Cipherbloecke.
	uint32_t cL = r[4] ^ k89;
	uint32_t cR = r[4] ^ l[4] ^ k1011;

	// Zusammenfuehrung der linken und rechten Cipherblockhaelften.
	return (uint64_t)(((uint64_t)cL << 32) | cR);
}

/**
 * Dekodieren eines 64 bit Chiffreblocks
 *
 * @param c 	- 64 bit Cipherblock
 * @param *k	- Pointer auf 12 16 bit Subkeys
 *
 * @return		- 64 bit Plaintextblock
 */
uint64_t decode(uint64_t c, uint16_t * k)
{
	uint32_t l[5];
	uint32_t r[5];

	// Splitten des 64 bit Cipherblock in 2 Doppelwoerter.
	uint32_t cL = c >> 32;
	uint32_t cR = c;

	// Konkatenation von 16 bit Schluesselpaaren zu einem Doppelwort.
	// TODO: Entspricht die Konkatenation dem Paper-Ausdruck?
	uint32_t k45 = wordsToUint32(k[4], k[5]);
	uint32_t k67 = wordsToUint32(k[6], k[7]);
	uint32_t k89 = wordsToUint32(k[8], k[9]);
	uint32_t k1011 = wordsToUint32(k[10], k[11]);

	// Beginn der eigentlichen Dekodierung.
	r[4] = cL ^ k89;
	l[4] = cR ^ r[4] ^ k1011;

	// Ausfuehren der 4 Feistel Runden
	for(int i = 4; i > 0; --i)
	{
		r[i-1] = l[i];
		l[i-1] = r[i] ^ f(r[i-1], k[i-1]);
	}

	// Berechnung der 2 32 Bit Plaintextbloecke.
	uint32_t pL = l[0] ^ k45;
	uint32_t pR = r[0] ^ pL ^ k45 ^ k67;

	// Zusammenfuehrung der linken und rechten Plaintextblockhaelften.
	return (uint64_t)(((uint64_t)pL << 32) | pR);
}
