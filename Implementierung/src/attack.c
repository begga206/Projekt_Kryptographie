/*
 * attack.c
 *
 *  Created on: 04.05.2016
 *      Author: Lukas
 */

#include "attack.h"


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

uint32_t theta_L(uint32_t a)
{
	return((a >> 8) & 0xffffff00);
}

uint32_t theta_R(uint32_t a)
{
	return((a << 8) & 0x00ffffff);
}

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
	/*
	P[18]  = ((uint64_t)rand() << 48)^ ((uint64_t)rand() << 32) ^
			((uint32_t)rand() << 16) ^ ((uint32_t)rand());
	P[19]  = ((uint64_t)rand() << 48)^ ((uint64_t)rand() << 32) ^
			((uint32_t)rand() << 16) ^ ((uint32_t)rand());
 	*/
	P[18] = (uint64_t)("FEAL is\0");
	P[19] = (uint64_t)(" safe!!\0");


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
	P[15] |= (P[15] >> 32) ^ ((P[14] >> 32) ^(P[14] & 0x00000000FFFFFFFF));

	return P;
}

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

int doesSatisfy5_4(uint32_t CiL, uint32_t Ui, uint32_t PiL, uint32_t Vi, uint32_t Di , uint32_t W)
{
	if(( CiL ^ Ui ^ G(PiL ^ Vi) ^ G(Di ^ W)) == 0)
		return 1;
	return 0;
}

int doesSatisfy5_5(uint32_t CiL, struct triplet trippel, uint32_t PiL, uint32_t Di)
{
	if(( CiL ^ trippel.U0 ^ G(PiL ^ trippel.V0) ^ G(Di ^ trippel.W)) == 0)
		return 1;
	return 0;
}

int doesSatisfy5_5ForOtherPlaintexts(struct triplet triple, uint32_t *PL, uint32_t *CL, uint32_t *D)
{
	int doesSatisfy = 1;

	for(int i = 0; i < 12; ++i)
	{
		if((CL[i] ^ triple.U0 ^ G(PL[i] ^ triple.V0) ^ G(D[i] ^ triple.W)) != 0)
			doesSatisfy = 0;
	}

	return doesSatisfy;
}

struct triplet getTripletFrom5_5(uint32_t CL0, uint32_t PL0, uint32_t V0, uint32_t D0, uint32_t W)
{
	struct triplet t;
	t.W = W;
	t.V0 = V0;
	t.U0 = CL0 ^ G(PL0 ^ V0) ^ G(D0 ^ W);

	return t;
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

uint32_t *calculateKeyConstants(uint32_t *PL, uint32_t *PR, uint32_t *CL, uint32_t *CR, uint32_t *Q, uint32_t *D, uint32_t *U, uint32_t *V, uint32_t W)
{
	// Finde Loesungen fuer N1
	uint32_t *N1Solutions = NULL;
	uint32_t N1SolutionsCount = getSolutionsForXFrom3_7(Q[0],Q[12],Q[14], V[0] ^ V[12], V[0] ^ V[14], &N1Solutions);

	for(int i = 0; i < N1SolutionsCount; ++i)
	{
		// Berechne V16. Entspricht der Struktur von (5.13) nach V16 aufgeloest.
		uint32_t V16 = G(Q[0] ^ N1Solutions[i]) ^ G(Q[16] ^ N1Solutions[i]) ^ V[0];
		uint32_t U16 = U[0] ^ Q[0] ^ Q[16];
		// Ueberpruefe, ob (5.4) mit V16 erfuellt ist.
		if(!doesSatisfy5_4(CL[16],U16,PL[16],V16,D[16],W))
			continue;

		// Mittels N1 lassen sich M1 und N3 errechnen.
		uint32_t y0 = Q[0] ^ N1Solutions[i];
		uint32_t m1 = V[0] ^ G(y0);
		uint32_t n3 = y0 ^ U[0];

		// Finde Loesungen fuer M2.
		uint32_t x1_0  = PL[0] ^ m1 ^ G(y0);
		uint32_t x1_17 = PL[17] ^ m1 ^ G(Q[17] ^ N1Solutions[i]);
		uint32_t x1_18 = PL[18] ^ m1 ^ G(Q[18] ^ N1Solutions[i]);
		uint32_t y1_0  = y0 ^ G(x1_0);
		uint32_t y1_17 = Q[17] ^ N1Solutions[i] ^ G(x1_17);
		uint32_t y1_18 = Q[18] ^ N1Solutions[i] ^ G(x1_18);

		uint32_t d = x1_0 ^ x1_17 ^ D[0] ^ D[17];
		uint32_t e = x1_0 ^ x1_18 ^ D[0] ^ D[18];

		uint32_t *M2Solutions = NULL;
		int M2SolutionsCount = getSolutionsForXFrom3_7(y1_0,y1_17,y1_18,d,e,&M2Solutions);

		for(int j = 0; j < M2SolutionsCount; ++j)
		{
			// Check, ob die auesseren 16 bit 0 sind.
			if((M2Solutions[j] & 0xFF0000FF) != 0)
				continue;

			// Mit Hilfe von M2 drei Werte fuer M3 schreiben, die uebereinstimmen sollten.
			uint32_t x2_0  = x1_0 ^ G(y1_0 ^ M2Solutions[j]);
			uint32_t x2_17 = x1_17 ^ G(y1_17 ^ M2Solutions[j]);
			uint32_t x2_18 = x1_18 ^ G(y1_18 ^ M2Solutions[j]);
			uint32_t m3_0  = D[0] ^ x2_0;
			uint32_t m3_17 = D[17] ^ x2_17;
			uint32_t m3_18 = D[18] ^ x2_18;

			if((m3_0 != m3_17) || (m3_17 != m3_18))
				continue;


			// N2 berechnen und pruefen, ob die aeusseren 16 Bit 0 sind.
			uint32_t n2 = W ^ m3_0;

			if((n2 & 0xFF0000FF) != 0)
				continue;

			// Speichere die Konstanten in einem Array und gib es zurück;
			uint32_t *constants = malloc(6 * sizeof(uint32_t));
			constants[M1] = m1;
			constants[N1] = N1Solutions[i];
			constants[M2] = M2Solutions[j];
			constants[N2] = n2;
			constants[M3] = m3_0;
			constants[N3] = n3;
			return constants;
		}
	}
	return NULL;
}

void attack()
{
	// Testschlüssel
	uint64_t key = 0xFF00FF00FF00FF00;

	// Zuerst werden die 12 16 bit subkeys errechnet
	uint16_t *subkeys = compSubKeys(key);

	// Danach werden 20 Plaintexte nach der Definition aus dem Paper erzeugt.
	uint64_t *P = choosePlainTexts();

	// Allokiere Speicher fuer die Ciphertextbloecke und wende das FEAL-
	// Verfahren zur Verschluesselung an.
	uint64_t *C = malloc(20 * sizeof(uint64_t));
	for(int i = 0; i < 20; ++i)
	{
		C[i] = encode(P[i], subkeys);
	}

	// Berechne die 6 32-bit Schluesselabhaengigen Konstanten (2.6).
	uint32_t *constants = computeConstants(subkeys);

	// Bekomme die 6 Variablen bei dem codieren von P0
	//uint32_t *variables = getLinearEncodeVariables(P[0], constants);
	//uint32_t U0 = variables[Y0] ^ constants[N3];
	//uint32_t V0 = constants[M1] ^ G(variables[Y0]);
	//uint32_t W  = constants[M3] ^ constants[N2];

	uint32_t *CL = malloc(20 * sizeof(uint32_t));
	uint32_t *CR = malloc(20 * sizeof(uint32_t));
	uint32_t *PL = malloc(20 * sizeof(uint32_t));
	uint32_t *PR = malloc(20 * sizeof(uint32_t));
	uint32_t *D  = malloc(20 * sizeof(uint32_t));
	uint32_t *Q  = malloc(20 * sizeof(uint32_t));

	for(int i = 0; i < 20; ++i)
	{
		CL[i] = (uint32_t)(C[i]>>32);
		CR[i] = (uint32_t)(C[i]);
		PL[i] = (uint32_t)(P[i]>>32);
		PR[i] = (uint32_t)(P[i]);
		D[i]  = CL[i] ^ CR[i];
		Q[i]  = PL[i] ^ PR[i];
	}

	// Finde W
	uint32_t d = CL[0] ^ CL[1] ^ 0x02000000;
	uint32_t e = CL[0] ^ CL[2] ^ 0x00000002;
	uint32_t *wSolutions = NULL;

	uint32_t wSolutionsCount = getSolutionsForXFrom3_7(D[0], D[1], D[2], d, e,
			&wSolutions);
	wSolutionsCount = getSolutionsFor5_9(D[0], D[3], D[4], CL[0], CL[3], CL[4],
			&wSolutions, wSolutionsCount);
	printf("\n%d Solutions for W:\n", wSolutionsCount);
	for(int i = 0; i < wSolutionsCount; ++i)
	{
		printf("0x%"PRIx32"\n", wSolutions[i]);
	}


	// Finde V0

	uint32_t *v0Solutions;
	uint32_t v0SolutionCount;

	struct triplet *triplets = NULL;
	int tripletsCount = 0;
	for(int i = 0; i < wSolutionsCount; ++i)
	{
		d = CL[0] ^ CL[5] ^ G(D[0] ^ wSolutions[i]) ^ G(D[5] ^ wSolutions[i]);
		e = CL[0] ^ CL[6] ^ G(D[0] ^ wSolutions[i]) ^ G(D[6] ^ wSolutions[i]);
		v0SolutionCount  = getSolutionsForXFrom3_7(PL[0], PL[5], PL[6], d, e,
				&v0Solutions);

		triplets = realloc(triplets, (tripletsCount + v0SolutionCount) *
				sizeof(struct triplet));
		for(int j = 0; j < v0SolutionCount; ++j)
		{
			//printf("0x%"PRIx32"\n", v0Solutions[j]);
			// Berechne dazugehöriges U0 (= CL0 ^ G(PL0 ^ V0) ^ G(D0 ^ W))
			struct triplet triple = getTripletFrom5_5(CL[0], PL[0],
					v0Solutions[j], D[0], wSolutions[i]);

			// Adde nur die Triple, die fuer die anderen Plaintexte (5.5)
			// erfuellen
			if(doesSatisfy5_5ForOtherPlaintexts(triple, PL, CL, D))
			{
				triplets[tripletsCount] = triple;
				tripletsCount++;
			}
		}
	}
	printf("\n%d Solutions for Triplets:\n", tripletsCount);
	for(int i = 0; i < wSolutionsCount; ++i)
	{
		printf("U0: 0x%"PRIx32" V0: 0x%"PRIx32" W: 0x%"PRIx32"\n",
				triplets[i].U0, triplets[i].V0, triplets[i].W);
	}

	//=========================================================================
	// Finde Loesungen fuer V12 und V14
	//=========================================================================
	for(int i = 0; i < tripletsCount; ++i)
	{
		uint32_t U12 = triplets[i].U0 ^ Q[0] ^ Q[12];
		uint32_t U13 = U12;
		uint32_t U14 = triplets[i].U0 ^ Q[0] ^ Q[14];
		uint32_t U15 = U14;

		uint32_t *V12Solutions = NULL;
		int V12SolutionsCount = getSolutionsForXFrom3_1(PL[12], G(D[12] ^
				triplets[i].W) ^ CL[12] ^ U12, &V12Solutions);
		uint32_t *V14Solutions = NULL;
		int V14SolutionsCount = getSolutionsForXFrom3_1(PL[14], G(D[14] ^
				triplets[i].W) ^ CL[14] ^ U14, &V14Solutions);

		for(int j = 0; j < V12SolutionsCount; ++j)
		{
			if(doesSatisfy5_4(CL[13], U13, PL[13], V12Solutions[j], D[13],
					triplets[i].W))
			{
				for(int k = 0; k < V14SolutionsCount; ++k)
				{
					if(doesSatisfy5_4(CL[15], U15, PL[15], V14Solutions[k],
							D[15], triplets[i].W))
					{
						// Key Konstanten berechnen.
						uint32_t *U = malloc(20 * sizeof(uint32_t));
						uint32_t *V = malloc(20 * sizeof(uint32_t));
						for(int l = 0; l < 12; ++l)
						{
							U[l] = triplets[i].U0;
							V[l] = triplets[i].V0;
						}
						U[12] = U12;
						U[13] = U13;
						U[14] = U14;
						U[15] = U15;
						V[12] = V12Solutions[j];
						V[13] = V12Solutions[j];
						V[14] = V14Solutions[k];
						V[15] = V14Solutions[k];
						uint32_t *calculatedKeyConstants =
								calculateKeyConstants(PL,PR,CL,CR,Q,D,U,V,
										triplets[i].W);

						if(calculatedKeyConstants != NULL)
						{
							printf("\nPossible Key Constants:\n");
							printf("M1: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[M1], calculatedKeyConstants[M1]);
							printf("N1: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[N1], calculatedKeyConstants[N1]);
							printf("M2: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[M2], calculatedKeyConstants[M2]);
							printf("N2: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[N2], calculatedKeyConstants[N2]);
							printf("M3: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[M3], calculatedKeyConstants[M3]);
							printf("N3: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[N3], calculatedKeyConstants[N3]);

							printf("%s",linearDecode(C[18],
									calculatedKeyConstants));
							printf("%s\n\n",linearDecode(C[19],
									calculatedKeyConstants));

							free(calculatedKeyConstants);


						}
						free(U);
						free(V);
					}
				}
			}
		}

		free(V12Solutions);
		free(V14Solutions);
	}


	free(CR);
	free(CL);
	free(PR);
	free(PL);
	free(D);
	free(Q);
	free(triplets);
	free(wSolutions);
	free(v0Solutions);
	//free(variables);
	free(constants);
}

