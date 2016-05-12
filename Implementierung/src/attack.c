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
	c1 = S(d1, d2, ONE);
	c2 = S(d2, c1, ZERO);
	c0 = S(a[0], c1, ZERO);
	c3 = S(a[3], c2, ONE);

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


/**
 * Implementierung der Funktionen 3.2-3.4 um für G(x ^ a) = b Loesungen fuer
 * x zu finden. Diese sind in einem Array gespeichert.
 *
 * @return Anzahl an Loesungen fuer x
 */
int getSolutionsForX(uint32_t aDWord, uint32_t bDWord, uint32_t *solutions)
{
	int solutionCount = 0;	// amount of solutions
	uint8_t a[4] = {0};
	uint8_t b[4] = {0};

	// Allokiere Plaetze, um die Loesungen fuer x zu speichern.
	// Nehmen wir an, das 50% aller z1, z2 die Gleichung (3.2) erfuellen.
	// Dann allokieren wir 2^8 * 32 = 8192 bit = 1024 Byte = 1 KB im Heap.
	// Damit sollten alle moeglichen Loesungen fuer x in diesem Array
	// gespeichert werden koennen.
	solutions = malloc(2^8 * sizeof(uint32_t));

	// Split a to a0, a1, a2, a3 (analog fuer b)
	splitToBytes(aDWord, a);
	splitToBytes(bDWord, b);

	//fuer jedes z1
	for(uint8_t z1 = 0; z1 < 256; ++z1)
	{
		//fuer jedes z2
		for(uint8_t z2 = 0; z2 < 256; ++z2)
		{
			// Falls Gleichung (3.2) nicht erfuellt ist naechste z's
			if(S(z1 ^ a[0] ^ a[1], z2 ^ a[2] ^ a[3], ONE) != b[1])
				continue;

			// Checke, ob Gleichung aus (3.3) fuer x0, x3 erfuellt werden.
			for(uint8_t x0 = 0; x0 < 256; ++x0)
			{
				for(uint8_t x3 = 0; x3 < 256; ++x3)
				{
					// Falls eine Gleichung aus (3.3) nicht erfuellt, naechste Iteration
					if(S(b[1], z2 ^ a[2] ^ a[3], ZERO) != b[2])
						continue;
					if(S(b[1], x0 ^ a[0], ZERO) != b[0])
						continue;
					if(S(b[2], x3 ^ a[3], ONE) != b[3])
						continue;

					// Jede Gleichung fuer z1, z2, x0, x3 ist korrekt.
					// Errechne x1, x2 (3.4) und speichere die Loesung fuer x ab.
					uint32_t x = bytesToUint32(x0, z1 ^ x0, z2 ^ x3, x3);
					solutions[solutionCount] = x;
					++solutionCount;
				}
			}
		}
	}
	return solutionCount;
}

/**
 * Schaut fuer z1, z2, x0, x3, ob die Gleichungen aus (3.6) korrekt sind.
 * Jedoch fuer zwei Gleichungen simultan (3.7). Falls alle Gleichungen
 * korrekt sind, wird die Loesung abgespeichert.
 *
 * @return Anzahl der gefundenen Loesungen
 */
int getSolutionsForXFrom3_7(uint32_t aDWord, uint32_t bDWord, uint32_t cDWord, uint32_t dDWord, uint32_t eDWord, uint32_t * solutions)
{
	int solutionCount = 0;	// Anzahl an Loesungen fuer x
	uint8_t a[4] = {0};
	uint8_t b[4] = {0};
	uint8_t c[4] = {0};
	uint8_t d[4] = {0};
	uint8_t e[4] = {0};

	// Allokiere Plaetze, um die Loesungen fuer x zu speichern.
	// Nehmen wir an, das 50% aller z1, z2 die Gleichung (3.2) erfuellen.
	// Dann allokieren wir 2^8 * 32 = 8192 bit = 1024 Byte = 1 KB im Heap.
	// Damit sollten alle moeglichen Loesungen fuer x in diesem Array
	// gespeichert werden koennen.
	solutions = malloc(2^8 * sizeof(uint32_t));

	// Split a to a0, a1, a2, a3 (analog fuer b, c, d, e)
	splitToBytes(aDWord, a);
	splitToBytes(bDWord, b);
	splitToBytes(cDWord, c);
	splitToBytes(dDWord, d);
	splitToBytes(eDWord, e);


	// Check fuer jedes z1, z2...
	for(uint8_t z1 = 0; z1 < 256; ++z1)
	{
		for(uint8_t z2 = 0; z2 < 256; ++z2)
		{
			// Wir checken fuer beide Gleichungen in (3.7) gleichzeitig!!!
			uint8_t alpha1 = S(z1 ^ a[0] ^ a[1], z2 ^ a[2] ^ a[3], ONE);
			uint8_t beta1  = S(z1 ^ b[0] ^ b[1], z2 ^ b[2] ^ b[3], ONE);
			uint8_t gamma1 = S(z1 ^ c[0] ^ c[1], z2 ^ c[2] ^ c[3], ONE);

			if((alpha1 ^ beta1 != d[1]) || (alpha1 ^ gamma1 != e[1]))
				continue;

			uint8_t alpha2 = s(alpha1, z2 ^ a[2] ^ a[3], ZERO);
			uint8_t beta2  = s(beta1, z2 ^ b[2] ^ b[3], ZERO);
			uint8_t gamma2 = s(gamma1, z2 ^ c[2] ^ c[2], ZERO);

			if((alpha2 ^ beta2 != d[2]) || (alpha2 ^ gamma2 != e[2]))
				continue;

			for(uint8_t x0 = 0; x0 < 256; ++x0)
			{
				for(uint8_t x3 = 0; x3 < 256; ++x3)
				{
					uint8_t s0Alpha1 = S(alpha1, x0 ^ a[0], ZERO);
					if((s0Alpha1 ^ S(beta1, x0 ^ b[0], ZERO)) != d[0])
						continue;
					if((s0Alpha1 ^ S(gamma1, x0 ^ c[0], ZERO)) != e[0])
						continue;

					uint8_t s1Alpha2 = S(alpha2, x3 ^ a[3], ONE);
					if((s1Alpha2 ^ S(beta2, x3 ^ b[3], ONE)) != d[3])
						continue;
					if((s1Alpha2 ^ S(gamma2, x3 ^ c[3], ONE)) != e[3])
						continue;

					// Jede Gleichung fuer z1, z2, x0, x3 ist korrekt.
					// Errechne x1, x2 (3.4) und speichere die Loesung fuer x ab.
					uint32_t x = bytesToUint32(x0, z1 ^ x0, z2 ^ x3, x3);
					solutions[solutionCount] = x;
					++solutionCount;
				}
			}
		}
	}
	return solutionCount;
}
