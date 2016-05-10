/*
 * attack.c
 *
 *  Created on: 04.05.2016
 *      Author: Lukas
 */

#include "attack.h"

/**
 * Hilfsfunktion G
 */
uint32_t G(uint32_t aDWord)
{
	uint8_t a[4] = {0};
	uint8_t c0, c1, c2, c3, d1, d2;

	// Split a to a0, a1, a2, a3
	splitToBytes(aDWord, a);

	d1 = a[0] ^ a[1];
	d2 = a[2] ^ a[3];
	c1 = s(d1, d2, ONE);
	c2 = s(d2, c1, ZERO);
	c0 = s(a[0], c1, ZERO);
	c3 = s(a[3], c2, ONE);

	return bytesToUint32(c0, c1, c2, c3);
}

/**
 * "Simple functions" O_L
 */
uint32_t O_L(uint32_t a)
{
	return (a >> 8) & 0x00FFFF00;
}

/**
 * "Simple functions" O_R
 */
uint32_t O_R(uint32_t a)
{
	return (a << 8) & 0x00FFFF00;
}

/**
 * Wähle die 20 Plaintexte, die nötig für die Attacke sind, nach der Vorgabe
 * aus der Quelle.
 */
uint64_t *choosePlainTexts()
{
	uint64_t *P = malloc(20 * sizeof(uint64_t));

	// (1) Choose P0,12,14,16,17,18,19 randomly.
	srand(time(0));
	P[0]   = ((uint64_t)rand() << 48)+ ((uint64_t)rand() << 32) +
			((uint32_t)rand() << 16) + ((uint32_t)rand());
	P[12]  = ((uint64_t)rand() << 48)+ ((uint64_t)rand() << 32) +
			((uint32_t)rand() << 16) + ((uint32_t)rand());
	P[14]  = ((uint64_t)rand() << 48)+ ((uint64_t)rand() << 32) +
			((uint32_t)rand() << 16) + ((uint32_t)rand());
	P[16]  = ((uint64_t)rand() << 48)+ ((uint64_t)rand() << 32) +
			((uint32_t)rand() << 16) + ((uint32_t)rand());
	P[17]  = ((uint64_t)rand() << 48)+ ((uint64_t)rand() << 32) +
			((uint32_t)rand() << 16) + ((uint32_t)rand());
	P[18]  = ((uint64_t)rand() << 48)+ ((uint64_t)rand() << 32) +
			((uint32_t)rand() << 16) + ((uint32_t)rand());
	P[19]  = ((uint64_t)rand() << 48)+ ((uint64_t)rand() << 32) +
			((uint32_t)rand() << 16) + ((uint32_t)rand());

	// (2) Choose P5_L, 6, 7, 8, 9, 10, 11, 13, 15 randomly.
	P[5]  = (uint64_t)(((uint32_t)rand() << 16) + rand()) << 32;
	P[6]  = (uint64_t)(((uint32_t)rand() << 16) + rand()) << 32;
	P[7]  = (uint64_t)(((uint32_t)rand() << 16) + rand()) << 32;
	P[8]  = (uint64_t)(((uint32_t)rand() << 16) + rand()) << 32;
	P[9]  = (uint64_t)(((uint32_t)rand() << 16) + rand()) << 32;
	P[10] = (uint64_t)(((uint32_t)rand() << 16) + rand()) << 32;
	P[11] = (uint64_t)(((uint32_t)rand() << 16) + rand()) << 32;
	P[13] = (uint64_t)(((uint32_t)rand() << 16) + rand()) << 32;
	P[15] = (uint64_t)(((uint32_t)rand() << 16) + rand()) << 32;


	// (3) Define P1_L, P2_L, P3_L, P4_L.
	P[1] = ((P[0] >> 32) ^ 0x80800000) << 32;
	P[2] = ((P[0] >> 32) ^ 0x00008080) << 32;
	P[3] = ((P[0] >> 32) ^ 0x40400000) << 32;
	P[4] = ((P[0] >> 32) ^ 0x00004040) << 32;


	// (4) Define (see 4.4 in the paper).
	uint32_t Q0 = (P[0] >> 32) ^ (P[0] & 0x00000000FFFFFFFF);
	for(int i = 1; i < 12; ++i)
	{
		P[i] |= (P[i] >> 32) ^ Q0;
	}
	P[13] |= (P[13] >> 32) ^ ((P[12] >> 32) ^(P[12] & 0x00000000FFFFFFFF));
	P[15] |= (P[15] >> 32) ^ ((P[13] >> 32) ^(P[13] & 0x00000000FFFFFFFF));

	return P;
}
