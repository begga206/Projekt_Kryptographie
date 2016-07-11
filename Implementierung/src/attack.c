/*
 * attack.c
 *
 *  Created on: 04.05.2016
 *      Author: Lukas
 */

#include "attack.h"

/**
 * Hilfsfunktion G aus dem Paper
 * c = G(a) 	(2.1)
 *
 * @param aDWord - 32 bit parameter
 * @return c
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
 * Wähle die 20 Plaintexte, die nötig für die Attacke sind, nach der Vorgabe
 * aus der Quelle.
 *
 * @return Pointer auf die Plaintextbloecke
 */
uint64_t *choosePlainTexts()
{
	uint64_t *P = malloc(20 * sizeof(uint64_t));

	// (1) Choose P0,12,14,16,17,18,19 randomly.
	srand(time(0));
	P[0]   = ((uint64_t)rand() << 48)^ ((uint64_t)rand() << 32) ^
			((uint32_t)rand() << 16) ^ ((uint32_t)rand());
	P[12]  = ((uint64_t)rand() << 48)^ ((uint64_t)rand() << 32) ^
			((uint32_t)rand() << 16) ^ ((uint32_t)rand());
	P[14]  = ((uint64_t)rand() << 48)^ ((uint64_t)rand() << 32) ^
			((uint32_t)rand() << 16) ^ ((uint32_t)rand());
	P[16]  = ((uint64_t)rand() << 48)^ ((uint64_t)rand() << 32) ^
			((uint32_t)rand() << 16) ^ ((uint32_t)rand());
	P[17]  = ((uint64_t)rand() << 48)^ ((uint64_t)rand() << 32) ^
			((uint32_t)rand() << 16) ^ ((uint32_t)rand());
	P[18]  = ((uint64_t)rand() << 48)^ ((uint64_t)rand() << 32) ^
			((uint32_t)rand() << 16) ^ ((uint32_t)rand());
	P[19]  = ((uint64_t)rand() << 48)^ ((uint64_t)rand() << 32) ^
			((uint32_t)rand() << 16) ^ ((uint32_t)rand());

	// (2) Choose P5_L, 6, 7, 8, 9, 10, 11, 13, 15 randomly.
	P[5]  = (uint64_t)(((uint32_t)rand() << 16) ^ rand()) << 32;
	P[6]  = (uint64_t)(((uint32_t)rand() << 16) ^ rand()) << 32;
	P[7]  = (uint64_t)(((uint32_t)rand() << 16) ^ rand()) << 32;
	P[8]  = (uint64_t)(((uint32_t)rand() << 16) ^ rand()) << 32;
	P[9]  = (uint64_t)(((uint32_t)rand() << 16) ^ rand()) << 32;
	P[10] = (uint64_t)(((uint32_t)rand() << 16) ^ rand()) << 32;
	P[11] = (uint64_t)(((uint32_t)rand() << 16) ^ rand()) << 32;
	P[13] = (uint64_t)(((uint32_t)rand() << 16) ^ rand()) << 32;
	P[15] = (uint64_t)(((uint32_t)rand() << 16) ^ rand()) << 32;


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
	//TODO: Mit Weber diskutieren, ob das Paper falsch ist!!!
	P[15] |= (P[15] >> 32) ^ ((P[14] >> 32) ^(P[14] & 0x00000000FFFFFFFF));

	return P;
}

/**
 * Implementierung der Funktionen 3.2-3.4 um für G(x ^ a) = b Loesungen fuer
 * x zu finden. Diese sind in einem Array gespeichert.
 *
 * @param aDWord 		- a
 * @param bDWord 		- b
 * @param **solutions 	- Wo die Loesungen gespeichert sind
 *
 * @return Anzahl an Loesungen fuer x
 */
int getSolutionsForXFrom3_1(uint32_t aDWord, uint32_t bDWord, uint32_t **solutions)
{
	int solutionCount = 0;	// amount of solutions
	uint8_t a[4] = {0};
	uint8_t b[4] = {0};

	// Allokiere Plaetze, um die Loesungen fuer x zu speichern.
	// Nehmen wir an, das 50% aller z1, z2 die Gleichung (3.2) erfuellen.
	// Dann allokieren wir 2^8 * 32 = 8192 bit = 1024 Byte = 1 KB im Heap.
	// Damit sollten alle moeglichen Loesungen fuer x in diesem Array
	// gespeichert werden koennen.
	*solutions = malloc((2^12 * sizeof(uint32_t)));

	// Split a to a0, a1, a2, a3 (analog fuer b)
	splitToBytes(aDWord, a);
	splitToBytes(bDWord, b);

	//fuer jedes z1
	for(int i = 0; i < 256; ++i)
	{
		uint8_t z1 = i;
		//fuer jedes z2
		for(int j = 0; j < 256; ++j)
		{
			uint8_t z2 = j;
			// Falls Gleichung (3.2) nicht erfuellt ist naechste z's
			if(S(z1 ^ a[0] ^ a[1], z2 ^ a[2] ^ a[3], ONE) != b[1])
				continue;

			// Checke, ob Gleichung aus (3.3) fuer x0, x3 erfuellt werden.
			for(int k = 0; k < 256; ++k)
			{
				uint8_t x0 = k;
				for(int l = 0; l < 256; ++l)
				{
					uint8_t x3 = l;
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
					(*solutions)[solutionCount] = x;
					++solutionCount;
				}
			}
		}
	}
	realloc(*solutions, solutionCount * sizeof(uint32_t));
	return solutionCount;
}

/**
 * Schaut fuer z1, z2, x0, x3, ob die Gleichungen aus (3.6) korrekt sind.
 * Jedoch fuer zwei Gleichungen simultan (3.7). Falls alle Gleichungen
 * korrekt sind, wird die Loesung abgespeichert.
 *
 * G(x ^ a) ^ G(x ^ b) = d
 * G(x ^ a) ^ G(x ^ c) = e			(3.7)
 *
 * @param aDWord 	- a
 * @param bDWord 	- b
 * @param cDWord	- c
 * @param dDWord	- d
 * @param eDWord	- e
 * @param solutions - Wo die Loesungen gespeichert sind
 *
 * @return Anzahl der gefundenen Loesungen
 */
int getSolutionsForXFrom3_7(uint32_t aDWord, uint32_t bDWord, uint32_t cDWord, uint32_t dDWord, uint32_t eDWord, uint32_t ** solutions)
{
	int solutionCount = 0;	// Anzahl an Loesungen fuer x
	uint8_t a[4] = {0};
	uint8_t b[4] = {0};
	uint8_t c[4] = {0};
	uint8_t d[4] = {0};
	uint8_t e[4] = {0};

	// Allokiere Plaetze, um die Loesungen fuer x zu speichern.
	// Nehmen wir an, das 100% aller z1, z2 die Gleichung (3.2) erfuellen.
	// Dann allokieren wir 2^17 * 32 = 4194304 bit = 524288 Byte = 512 KB im Heap.
	// Damit sollten alle moeglichen Loesungen fuer x in diesem Array
	// gespeichert werden koennen.

	uint32_t *tmpPointer = malloc(131072 * sizeof(uint32_t));	//2^17 hat nicht funktioniert, also 131072 ausgeschrieben...


	// Split a to a0, a1, a2, a3 (analog fuer b, c, d, e)
	splitToBytes(aDWord, a);
	splitToBytes(bDWord, b);
	splitToBytes(cDWord, c);
	splitToBytes(dDWord, d);
	splitToBytes(eDWord, e);

	uint8_t z1 = 0;
	uint8_t z2 = 0;
	// Check fuer jedes z1, z2...
	for(int i = 0; i < 256; ++i)
	{
		z1 = i;
		for(int j = 0; j < 256; ++j)
		{
			z2 = j;
			// Wir checken fuer beide Gleichungen in (3.7) gleichzeitig!!!
			uint8_t alpha1 = S(z1 ^ a[0] ^ a[1], z2 ^ a[2] ^ a[3], ONE);
			uint8_t beta1  = S(z1 ^ b[0] ^ b[1], z2 ^ b[2] ^ b[3], ONE);
			uint8_t gamma1 = S(z1 ^ c[0] ^ c[1], z2 ^ c[2] ^ c[3], ONE);

			if(((alpha1 ^ beta1) != d[1]) || ((alpha1 ^ gamma1) != e[1]))
				continue;

			uint8_t alpha2 = S(alpha1, z2 ^ a[2] ^ a[3], ZERO);
			uint8_t beta2  = S(beta1, z2 ^ b[2] ^ b[3], ZERO);
			uint8_t gamma2 = S(gamma1, z2 ^ c[2] ^ c[3], ZERO);

			if(((alpha2 ^ beta2) != d[2]) || ((alpha2 ^ gamma2) != e[2]))
				continue;

			for(int k = 0; k < 256; ++k)
			{
				uint8_t x0 = k;
				for(int l = 0; l < 256; ++l)
				{
					uint8_t x3 = l;
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
					tmpPointer[solutionCount] = x;
					++solutionCount;
				}
			}
		}
	}
	*solutions = realloc(tmpPointer, solutionCount * sizeof(uint32_t));
	return solutionCount;
}

/*
 * Schraenkt die Loesungen an Ws ein, indem geprueft wird, ob W die folgenden
 * Gleichungen loest:
 *
 * 	G(D0 ^ W) ^ G(D3 ^ W) ^ C0L ^ C3L = 01000000 || 03000000	(5.9)
 * 	G(D0 ^ W) ^ G(D4 ^ W) ^ C0L ^ C4L = 00000001 || 00000003
 *
 * 	@param D0
 * 	@param D3
 * 	@param D4
 * 	@param C0L
 * 	@param C3L
 * 	@param C4L
 * 	@param solutions		- Wo die bisherigen Loesungen gespeichert sind
 * 	@param solutionsCount 	- Anzahl der bisherigen Loesungen
 */
int getSolutionsFor5_9(uint32_t D0, uint32_t D3, uint32_t D4, uint32_t C0L, uint32_t C3L, uint32_t C4L, uint32_t **solutions, int oldSolutionsCount)
{
	uint32_t *newSolutions = malloc(oldSolutionsCount * sizeof(uint32_t));
	int newSolutionsCount = 0;

	// Ueberpruefe fuer jedes W, ob es die Gleichungen loest.
	for(int i = 0; i < oldSolutionsCount; ++i)
	{
		uint32_t firstEquation = G(D0 ^ (*solutions)[i]) ^ G(D3 ^ (*solutions)[i]) ^ C0L ^ C3L;
		uint32_t secondEquation = G(D0 ^ (*solutions)[i]) ^ G(D4 ^(*solutions)[i]) ^ C0L ^ C4L;

		if(((firstEquation == 0x01000000) || (firstEquation == 0x03000000)) &&
				((secondEquation == 0x00000001) || (secondEquation == 0x00000003)))
		{
			newSolutions[newSolutionsCount] = (*solutions)[i];
			++newSolutionsCount;
		}
	}
	// die alten Loesungen werden nicht mehr gebraucht.
	free(solutions);
	*solutions = newSolutions;
	realloc(*solutions, newSolutionsCount * sizeof(uint32_t));
	return newSolutionsCount;
}

/**
 * Berechnet Loesungen fuer U0, abhaengig von W und V0 Loesungen mit Hilfe der
 * Gleichung:
 * 	CiL ^ U0 ^ G(PiL ^ V0) ^ G(Di ^ W) = 0		i = 0 		(5.5)
 *
 * 	@param C0L
 * 	@param P0L
 * 	@param D0
 * 	@param wSolutions
 * 	@param wSolutionsCount
 * 	@param vSolutions
 * 	@param vSolutionsCount
 * 	@param uSolutions
 *
 * 	@return Anzahl an Loesungen fuer U0
 */
int getU0Solutions(uint32_t C0L, uint32_t P0L, uint32_t D0,uint32_t *wSolutions, int wSolutionCount,
		uint32_t *vSolutions, int vSolutionCount, uint32_t * uSolutions)
{
	printf("Aufruf getU0Solutions.\n");
	uSolutions = malloc(wSolutionCount * vSolutionCount * sizeof(uint32_t));
	int uSolutionsCount = 0;

	for(int i = 0; i < wSolutionCount; ++i)
	{
		for(int j = 0; j < vSolutionCount; ++i)
		{
			uSolutions[uSolutionsCount] = C0L ^ G(P0L ^ vSolutions[j]) ^ G(D0 ^ wSolutions[i]);
			++uSolutionsCount;
		}
	}
	realloc(uSolutions, uSolutionsCount * sizeof(uint32_t));
	printf("Ende getU0Solutions.\n");
	return uSolutionsCount;
}

/**
 * Prueft, ob die Parameter Gleichung 5.4 erfuellen:
 *
 * CiL ^ Ui ^ G(PiL ^ Vi) ^ G(Di ^ W) = 0 	(5.4)
 *
 * @param CiL
 * @param PiL
 * @param Vi
 * @param Di
 * @param W
 *
 * @return 1, wenn erfuellt ; 0, wenn nicht
 */
int doesSatisfy5_4(uint32_t CiL, uint32_t Ui, uint32_t PiL, uint32_t Vi, uint32_t Di , uint32_t W)
{
	if(( CiL ^ Ui ^ G(PiL ^ Vi) ^ G(Di ^ W)) == 0)
		return 1;
	return 0;
}

/**
 * Prueft, ob die Parameter Gleichung 5.5 erfuellen:
 *
 * 	CiL ^ U0 ^ G(PiL ^ V0) ^ G(Di ^ W) = 0		i = 0 		(5.5)
 *
 * @param CiL
 * @param trippel
 * @param PiL
 * @param Di
 *
 * @return 1, wenn erfuellt ; 0, wenn nicht
 */
int doesSatisfy5_5(uint32_t CiL, struct triplet trippel, uint32_t PiL, uint32_t Di)
{
	if(( CiL ^ trippel.U0 ^ G(PiL ^ trippel.V0) ^ G(Di ^ trippel.W)) == 0)
		return 1;
	return 0;
}

/**
 * Findet Tripel (W, U0, V0), die Kandidaten sind, um die Schluesselkonstanten
 * zu errechnen.
 *
 * @param C
 * @param D
 * @param P
 * @param wSolutions
 * @param wSolutionsCount
 * @param vSolutions
 * @param vSolutionsCount
 * @param triplets
 *
 * @return Anzahl an gefundenen Tripeln
 */
int getTriplets(uint64_t *C, uint32_t *D, uint64_t *P, uint32_t *wSolutions,
		int wSolutionCount, uint32_t *vSolutions, int vSolutionCount, struct triplet **triplets)
{
	printf("Aufruf getTriplets().\n");
	uint32_t *uSolutions = NULL;
	int uSolutionsCount = getU0Solutions((uint32_t)(C[0] >> 32),
			(uint32_t)(P[0] >> 32),D[0],wSolutions, wSolutionCount, vSolutions,
			vSolutionCount, uSolutions);

	int maxSize = 20;
	*triplets = malloc(maxSize * sizeof(struct triplet));
	int tripletsCount = 0;

	uint32_t C7L = (uint32_t)(P[7] >> 32);
	uint32_t C8L = (uint32_t)(P[8] >> 32);
	uint32_t C9L = (uint32_t)(P[9] >> 32);
	uint32_t C10L = (uint32_t)(P[10] >> 32);
	uint32_t C11L = (uint32_t)(P[11] >> 32);
	uint32_t P7L = (uint32_t)(P[7] >> 32);
	uint32_t P8L = (uint32_t)(P[8] >> 32);
	uint32_t P9L = (uint32_t)(P[9] >> 32);
	uint32_t P10L = (uint32_t)(P[10] >> 32);
	uint32_t P11L = (uint32_t)(P[11] >> 32);

	printf("Finde trippel.\n");
	for(int i = 0; i < uSolutionsCount; ++i)
	{
		for(int j = 0; j < vSolutionCount; ++j)
		{
			for(int k = 0; k < wSolutionCount; ++k)
			{
				struct triplet trippel = {wSolutions[k] , vSolutions[j], uSolutions[i]};

				if(doesSatisfy5_5(C7L, trippel, P7L, D[7]) &&
						doesSatisfy5_5(C8L, trippel, P8L, D[8]) &&
						doesSatisfy5_5(C9L, trippel, P9L, D[9]) &&
						doesSatisfy5_5(C10L, trippel, P10L, D[10]) &&
						doesSatisfy5_5(C11L, trippel, P11L, D[11]))
				{
					if(tripletsCount == maxSize){
						maxSize += 20;
						realloc(*triplets, maxSize * sizeof(struct triplet));
					}
					(*triplets)[tripletsCount] = trippel;
					++tripletsCount;
				}
			}
		}
	}
	realloc(*triplets, tripletsCount * sizeof(struct triplet));
	printf("Trippel gefunden.\nEnde getTriplets().\n");
	return tripletsCount;
}

/**
 * Ueberprueft, ob x aeussere bits eines Doppelworts 0 sind und fuegt diese dann
 * denn Loesungen hinzu.
 *
 * @param x 			- Anzahl der aeusseren bits
 * @param **solution 	- Pointer auf den Pointer der bisherigen Loesungen
 * @param solutionCount - Anzahl der bisherigen Loesungen
 *
 * @return Teilmenge der bisherigen Loesungen, die die Bedingung erfuellen
 */
int constrainXOuterBitsZero(uint8_t x, uint32_t **solutions, int solutionsCount)
{
	uint32_t *rtSolutions = malloc(solutionsCount * sizeof(uint32_t));
	int rtSolutionsCount = 0;

	// Konstruiere die Maske.
	uint32_t mask = (uint32_t)0xFFFFFFFF;
	uint32_t mask1 = mask >> x/2;
	uint32_t mask2 = mask << x/2;
	mask = mask1 & mask2;

	// Ueberpruefe welche Doppelwoerter die Bedingung erfuellen.
	for(int i = 0; i < solutionsCount; ++i)
	{
		if(((*solutions)[i] | mask) == mask)
		{
			rtSolutions[rtSolutionsCount] = (*solutions)[i];
			++rtSolutionsCount;
		}
	}

	// Reallokiere auf die neue Anzahl Loesungen und gib diese zurueck.
	realloc(rtSolutions, rtSolutionsCount * sizeof(uint32_t));
	free(*solutions);
	*solutions = rtSolutions;
	return rtSolutionsCount;
}

/**
 * Einstieg der Attacke auf den FEAL Algorithmus
 *
 * @param *P	- Pointer auf die 20 64 bit Plaintextbloecke
 * @param *C	- Pointer auf die 20 64 bit Cipherbloecke zu P
 *
 * @return Pointer auf die Konstanten, die zum "linearen Dekodieren" benoetigt werden
 */
uint32_t *attack(uint64_t *P, uint64_t *C)
{
	printf("Beginn attack().\n");

	uint32_t Q[20];
	uint32_t D[20];

	// Initialisiere Q und D fuer i = 1,...,20.
	for(int i = 0; i < 20; ++i)
	{
		Q[i] = (uint32_t)((P[i] >> 32) ^ (P[i] & 0x00000000FFFFFFFF));
		D[i] = (uint32_t)((C[i] >> 32) ^ (C[i] & 0x00000000FFFFFFFF));
		//printf("Q[%d]: 0x%" PRIx32 "\tP[%d]: 0x%" PRIx64 "\n",i, Q[i], i, P[i]);
	}

	// Finde Loesungen fuer W.
	uint32_t *wSolutions = NULL;
	uint32_t dDWord = (uint32_t)((C[0] >> 32) ^ (C[1] >> 32) ^ 0x02000000);
	uint32_t eDWord = (uint32_t)((C[0] >> 32) ^ (C[2] >> 32) ^ 0x00000002);
	// Das ist Gleichung (5.8).

	printf("Get Solutions for w.\n");

	int wSolutionCount = getSolutionsForXFrom3_7(D[0], D[1], D[2], dDWord, eDWord, &wSolutions);
	printf("Found Solutions for w.\n");
	for(int i = 0; i < wSolutionCount; ++i)
	{
		printf("0x%" PRIx32 "\n", wSolutions[i]);
	}
	// Finde alle W, die die Gleichungen (5.9) erkennen.

	printf("Finde alle W, die (5.9) erfuellen.\n");
	wSolutionCount = getSolutionsFor5_9(D[0], D[3], D[4], (uint32_t)(C[0] >> 32),
			(uint32_t)(C[3] >> 32),(uint32_t)(C[4] >> 32), &wSolutions, wSolutionCount);
	printf("Alle Ws gefunden:\n");
	for(int i = 0; i < wSolutionCount; ++i)
	{
		printf("0x%" PRIx32 "\n", wSolutions[i]);
	}
	// Finde fue jeden W Wert Loesungen fuer V0 mit Hilfe von (5.10)
	uint32_t *v0Solutions = NULL;
	uint32_t *tmpSolutions = NULL;
	int v0SolutionCount = 0;

	printf("Finde fuer jedes W Loesungen fuer V0.\n");
	//=========================================================================
	//=========================================================================
	// Problem beim finden von Loesungen fuer V0
	//=========================================================================
	//=========================================================================
	for(int i = 0; i < wSolutionCount; ++i)
	{
		dDWord = (uint32_t)((C[0] >> 32) ^ (C[5] >> 32) ^ G(D[0] + wSolutions[i]) ^ G(D[5] ^ wSolutions[i]));
		eDWord = (uint32_t)((C[0] >> 32) ^ (C[6] >> 32) ^ G(D[0] + wSolutions[i]) ^ G(D[6] ^ wSolutions[i]));

		// Wir finden Loesungen fuer V0 fuer ein W
		int tmpCount = getSolutionsForXFrom3_7((uint32_t)(P[0] >> 32), (uint32_t)(P[5] >> 32), (uint32_t)(P[6] >> 32),
				dDWord, eDWord, &tmpSolutions);

		// Wir sammeln alle Loesungen in v0Solutions.
		realloc(v0Solutions, (v0SolutionCount + tmpCount) * sizeof(uint32_t));	// Bem.: Vielleicht muss man vorher einmal malloc machen?
		for(int j = 0; j < tmpCount; ++j)
		{
			v0Solutions[j + v0SolutionCount] = tmpSolutions[j];
		}
		v0SolutionCount += tmpCount;
		free(tmpSolutions);
	}
	//=========================================================================
	//=========================================================================
	// Der folgende Programbereich wurde noch nicht debuggt, da das Problem
	// schon vorher keine weiteren Rechnungen ermoeglicht.
	//=========================================================================
	//=========================================================================
	printf("Alle V0s gefunden.\n");

	// Get the (W, V0, U0) triplets, die (5.10f.) loesen.
	struct triplet *triplets = NULL;
	int tripletCount = getTriplets(C, D, P, wSolutions, wSolutionCount, v0Solutions, v0SolutionCount, &triplets);

	// For each triplet solve for the key constants.
	uint32_t *constants = NULL;
	constants = attack2(C, P, D, Q, triplets, tripletCount);


	free(wSolutions);
	free(v0Solutions);
	free(triplets);

	printf("End attack().\n");

	return constants;
}

/**
 * Zweiter Teil der Attacke
 *
 * @param *P		- Pointer auf die 20 64 bit Plaintextbloecke
 * @param *C		- Pointer auf die 20 64 bit Cipherbloecke zu P
 * @param *D
 * @param *Q
 * @param triplets 	- Die gefundenen Tripel bestehend aus: W, U0, V0
 *
 * @return Pointer auf die Konstanten, die zum "linearen Dekodieren" benoetigt werden
 */
uint32_t *attack2(uint64_t *C, uint64_t *P, uint32_t *D, uint32_t *Q, struct triplet *triplets, int tripletCount)
{
	printf("Aufruf attack2().\n");
	for(int i = 0; i < tripletCount; ++i)
	{
		// Implementierung von Gleichung (5.11)
		uint32_t U12 = triplets[i].U0 ^ Q[0] ^ Q[12];
		uint32_t U13 = U12;
		uint32_t U14 = triplets[i].U0 ^ Q[0] ^ Q[14];
		uint32_t U15 = U14;

		// Bekomme Loesungen fuer V12, V14 (5.12)
		printf("Finde fuer jedes Tripel Loesungen fuer V12 und V14.\n");
		uint32_t *V12Solutions = NULL;
		int V12SolutionsCount = getSolutionsForXFrom3_1((uint32_t)(P[12] >> 32), G(D[12] ^ triplets[i].W) ^ (uint32_t)(C[12] >> 32) ^ U12, &V12Solutions);
		uint32_t *V14Solutions = NULL;
		int V14SolutionsCount = getSolutionsForXFrom3_1((uint32_t)(P[14] >> 32), G(D[14] ^ triplets[i].W) ^ (uint32_t)(C[14] >> 32) ^ U14, &V14Solutions);
		printf("Loesungen fuer V12 und V14 gefunden.\nSchau, ob V13, V15 (5.4) erfuellen.\n");
		// Check, ob V13, V15 (5.4) erfuellen
		for(int j = 0; j < V12SolutionsCount; ++j)
		{
			for(int k = 0; k < V14SolutionsCount; ++k)
			{
				if(doesSatisfy5_4((uint32_t)(C[13] >> 32), U13, (uint32_t)(P[13] >> 32), V12Solutions[j], D[13], triplets[i].W) ||
						doesSatisfy5_4((uint32_t)(C[15] >> 32), U15, (uint32_t)(P[15] >> 32), V14Solutions[k], D[15], triplets[i].W))
				{
					printf("Kalkuliere Schlüsselkonstanten.\n");
					uint32_t *constants = calculateKeyConstants(C, P, D, Q, triplets[i], V12Solutions[j], V14Solutions[k]);
					if(constants != NULL)
					{
						free(V12Solutions);
						free(V14Solutions);
						printf("Ende attack2().\n");
						return constants;
					}

				}
			}
			free(V14Solutions);
		}
		free(V12Solutions);
	}
	printf("Ende attack2().\n");
	return NULL;
}

/**
 * Errechnet die Schluesselkonstanten
 *
 * @param C
 * @param P
 * @param D
 * @param Q
 * @param triplet
 * @param V12
 * @param V14
 *
 * @return Pointer, der auf die Schluesselkonstanten zeigt oder NULL
 */
uint32_t *calculateKeyConstants(uint64_t *C, uint64_t *P, uint32_t *D, uint32_t *Q, struct triplet triplet, uint32_t V12, uint32_t V14)
{
	printf("Aufruf calculateKeyConstants().\n");
	uint32_t *N1Solutions = NULL;
	printf("Finde Loesungen fuer N1.\n");
	int N1SolutionsCount = getSolutionsForXFrom3_7(Q[0], Q[12], Q[14], V12, V14, &N1Solutions);
	printf("Loesungen fuer N1 gefunden.\n");
	for(int i = 0; i < N1SolutionsCount; ++i)
	{
		// Errechne V16 und check, ob (5.4) erfuellt ist (TODO: Besprechen mit Juri)
		uint32_t V16 = G(Q[0] ^ N1Solutions[i]) ^ G(Q[16] ^ N1Solutions[i]) ^ triplet.V0;
		uint32_t U16 = triplet.U0 ^ Q[0] ^ Q[16];
		if(!doesSatisfy5_4((uint32_t)(C[i] >> 32), U16, (uint32_t)(P[i] >> 32), V16, D[i], triplet.W))
			continue;

		// Wenn wir ein N1 haben koennen wir M1 und N3 errechnen.
		uint32_t Y0_16 = Q[16] ^ N1Solutions[i];
		uint32_t M1 = V16 ^ G(Y0_16);
		uint32_t N3 = Y0_16 ^ U16;

		// Jetzt finden wir M2.
		// Errechne X1 und Y1 fuer P0, P17 und P18 (2.7)
		uint32_t Y0_0 = Q[0] ^ N1Solutions[i];
		uint32_t X1_0 = (uint32_t)(P[0] >> 32) ^ M1 ^ G(Y0_0);
		uint32_t Y1_0 = Y0_0 ^ G(X1_0);

		uint32_t Y0_17 = Q[17] ^ N1Solutions[i];
		uint32_t X1_17 = (uint32_t)(P[17] >> 32) ^ M1 ^ G(Y0_17);
		uint32_t Y1_17 = Y0_17 ^ G(X1_17);

		uint32_t Y0_18 = Q[18] ^ N1Solutions[i];
		uint32_t X1_18 = (uint32_t)(P[18] >> 32) ^ M1 ^ G(Y0_18);
		uint32_t Y1_18 = Y0_18 ^ G(X1_18);

		uint32_t *M2Solutions = NULL;
		int M2SolutionsCount = getSolutionsForXFrom3_7(Y1_0, Y1_17, Y1_18,
				X1_0 ^ X1_17 ^ D[0] ^ D[17], X1_0 ^ X1_18 ^ D[0] ^ D[18], &M2Solutions);

		// Wir duerfen nur die M2 Loesungen nehmen, deren 16 aeussere Bits 0 sind.
		M2SolutionsCount = constrainXOuterBitsZero(16, &M2Solutions, M2SolutionsCount);

		// Fuer jedes moegliche M2 errechnen wir X2_0, X2_17, X2_18
		for(int j = 0; j < M2SolutionsCount; ++j)
		{
			uint32_t X2_0 = X1_0 ^ G(Y1_0 ^ M2Solutions[j]);
			uint32_t X2_17 = X1_17 ^ G(Y1_17 ^ M2Solutions[j]);
			uint32_t X2_18 = X1_18 ^ G(Y1_18 ^ M2Solutions[j]);

			// Wir koennen jetzt 3 mal M3 berrechnen und diese sollten gleich sein.
			uint32_t M3_0 = D[0] ^ X2_0;
			uint32_t M3_17 = D[17] ^ X2_17;
			uint32_t M3_18 = D[18] ^ X2_18;

			// Wenn die M3s nicht gleich, war es die falsche Loesung fuer M2
			if((M3_0 != M3_17) || (M3_17 != M3_18))
				continue;

			// Berrechne N2 (16 outer bit 0). Somit haben wir Alle Key Konstanten.
			uint32_t N2 = triplet.W ^ M3_0;
			if((N2 | 0x00FFFF00) != 0x00FFFF00)
				continue;

			// Wir haben alle Konstanten zum codieren und decodieren gefunden.
			uint32_t *constants = malloc(6 * sizeof(uint32_t));
			constants[0] = M1;
			constants[1] = N1Solutions[i];
			constants[2] = M2Solutions[j];
			constants[3] = N2;
			constants[4] = M3_0;
			constants[5] = N3;

			free(M2Solutions);
			free(N1Solutions);
			printf("Ende calculateKeyConstants().\n");
			return constants;
		}
		free(M2Solutions);
	}
	free(N1Solutions);
	printf("Ende calculateKeyConstants().\n");
	return NULL;
}

uint32_t theta_L(uint32_t a)
{
	return((a >> 8) & 0xffffff00);
}

uint32_t theta_R(uint32_t a)
{
	return((a << 8) & 0x00ffffff);
}

uint32_t *computeConstants(uint16_t *subkeys)
{
	uint32_t *constants = malloc(6 * sizeof(uint32_t));
	uint32_t b[7] = {0};

	// b initialisieren.
	for(int i = 1; i <= 6; ++i)
	{
		b[i] = subkeys[2*(i-1)];
		b[i] = (b[i] << 16) | subkeys[2*i-1];
	}

	// Keykonstanten errechnen.
	constants[M1] = b[3] ^ theta_R(b[1]);
	constants[N1] = b[3] ^ b[4] ^ theta_L(b[1]);
	constants[M2] = theta_L(b[1]) ^ theta_L(b[2]);
	constants[N2] = theta_R(b[1]) ^ theta_R(b[2]);
	constants[M3] = b[5] ^ b[6] ^ theta_R(b[1]);
	constants[N3] = b[5] ^ theta_L(b[1]);

	return constants;
}

uint64_t linearEncode(uint64_t P, uint32_t *constants)
{
	uint32_t PL = (uint32_t)(P >> 32);
	uint32_t PR = (uint32_t)(P & 0x00000000FFFFFFFF);

	uint32_t x0 = PL ^ constants[M1];
	uint32_t y0 = PL ^ PR ^ constants[N1];
	uint32_t x1 = x0 ^ G(y0);
	uint32_t y1 = y0 ^ G(x1);
	uint32_t x2 = x1 ^ G(y1 ^ constants[M2]);
	uint32_t y2 = y1 ^ G(x2 ^ constants[N2]);
	uint32_t CL = y2 ^ constants[N3];
	uint32_t CR = x2 ^ constants[M3] ^ CL;

	return (uint64_t)(((uint64_t)CL << 32) | CR);
}

/**
 * Funktion, die einen Cipherblock mit Hilfe der Schluesselkonstanten dekodiert.
 *
 * @param C 		- Cipherblock
 * @param constants	- Schluesselkonstanten
 *
 * @return 64 Plaintextblock
 */
uint64_t linearDecode(uint64_t C, uint32_t *constants)
{
	uint32_t CL = (uint32_t)(C >> 32);
	uint32_t CR = (uint32_t)(C & 0x00000000FFFFFFFF);

	uint32_t x2 = constants[M3] ^ CL ^ CR;
	uint32_t y2 = CL ^ constants[N3];
	uint32_t y1 = G(x2 ^ constants[N2]) ^ y2;
	uint32_t x1 = G(y1 ^ constants[M2]) ^ x2;
	uint32_t y0 = G(x1) ^ y1;
	uint32_t x0 = G(y0) ^ x1;
	uint32_t PL = constants[M1] ^ x0;
	uint32_t PR = y0 ^ PL ^ constants[N1];

	return (uint64_t)(((uint64_t)PL << 32) | PR);
}

uint32_t *getLinearEncodeVariables(uint64_t P, uint32_t *constants)
{
	uint32_t *variables = malloc(6 * sizeof(uint32_t));

	uint32_t PL = (uint32_t)(P >> 32);
	uint32_t PR = (uint32_t)(P & 0x00000000FFFFFFFF);

	uint32_t x0 = PL ^ constants[M1];
	uint32_t y0 = PL ^ PR ^ constants[N1];
	uint32_t x1 = x0 ^ G(y0);
	uint32_t y1 = y0 ^ G(x1);
	uint32_t x2 = x1 ^ G(y1 ^ constants[M2]);
	uint32_t y2 = y1 ^ G(x2 ^ constants[N2]);

	variables[X0] = x0;
	variables[X1] = x1;
	variables[X2] = x2;
	variables[Y0] = y0;
	variables[Y1] = y1;
	variables[Y2] = y2;

	return variables;
}

uint32_t *getLinearDecodeVariables(uint64_t C, uint32_t *constants)
{
	uint32_t *variables = malloc(6 * sizeof(uint32_t));

	uint32_t CL = (uint32_t)(C >> 32);
	uint32_t CR = (uint32_t)(C & 0x00000000FFFFFFFF);

	uint32_t x2 = constants[M3] ^ CL ^ CR;
	uint32_t y2 = CL ^ constants[N3];
	uint32_t y1 = G(x2 ^ constants[N2]) ^ y2;
	uint32_t x1 = G(y1 ^ constants[M2]) ^ x2;
	uint32_t y0 = G(x1) ^ y1;
	uint32_t x0 = G(y0) ^ x1;

	variables[X0] = x0;
	variables[X1] = x1;
	variables[X2] = x2;
	variables[Y0] = y0;
	variables[Y1] = y1;
	variables[Y2] = y2;

	return variables;
}
