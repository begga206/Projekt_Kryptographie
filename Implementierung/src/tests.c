#include "tests.h"

/**
 * Erstellt einen Timestamp
 *
 * @return Timestamp als String
 */
char *time_stamp(){

	char *timestamp = (char *)malloc(sizeof(char) * 16);
	time_t ltime;
	ltime=time(NULL);
	struct tm *tm;
	tm=localtime(&ltime);

	sprintf(timestamp,"%04d.%02d.%02d.  %02d:%02d:%02d", tm->tm_year+1900, tm->tm_mon,
		tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	return timestamp;
}

/**
 * Erstellt eine zufaellige 32 bit Zahl
 *
 * @return random 32 bit Uint
 */
uint32_t getRandomUint32()
{
	uint32_t result = ((uint32_t)rand() << 16) + ((uint32_t)rand());
	return result;
}

/**
 * Testfunktion fuer "GetSolutionsForXFrom3_7)
 */
void testGetSolutionsForXFrom3_7()
{
	srand(time(0));
	uint32_t *solutions = NULL;
	// Erstelle a, b, c, x, d, e
	/*uint32_t a = getRandomUint32();
	uint32_t b = getRandomUint32();
	uint32_t c = getRandomUint32();
	uint32_t x = 0xABCDEF12;
	uint32_t d = G(x ^ a) ^ G(x ^ b);
	uint32_t e = G(x ^ a) ^ G(x ^ c);
	*/
	uint32_t a = 0x56be8770;
	uint32_t b = 0x6e069f91;
	uint32_t c = 0xd5de0fc8;
	uint32_t d = 0x6791a414;
	uint32_t e = 0xaca6c6fb;

	printf("%s: Anfang getSolutionsForXFrom3_7\n", time_stamp());
	printf("\ta = 0x%" PRIx32 "\n", a);
	printf("\tb = 0x%" PRIx32 "\n", b);
	printf("\tc = 0x%" PRIx32 "\n", c);
	//printf("\tx = 0x%" PRIx32 "\n", x);
	printf("\td = 0x%" PRIx32 "\n", d);
	printf("\te = 0x%" PRIx32 "\n", e);

	int solutionsCount = getSolutionsForXFrom3_7(a, b, c, d, e, &solutions);
	printf("%s: Ende getSolutionsForXFrom3_7\n SolutionCount: %d\n", time_stamp(), solutionsCount);

	if(solutionsCount != 0)
	{
		printf("Loesugen fuer x:\n");
		for(int i = 0; i < solutionsCount; ++i)
			{
				printf("\t0x%" PRIx32 "\n", solutions[i]);
			}
	}
	free(solutions);
	printf("Fertig.");
}

struct triplet getTripletFrom5_5(uint32_t CL0, uint32_t PL0, uint32_t V0, uint32_t D0, uint32_t W)
{
	struct triplet t;
	t.W = W;
	t.V0 = V0;
	t.U0 = CL0 ^ G(PL0 ^ V0) ^ G(D0 ^ W);

	return t;
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

uint32_t *calculateKeyConstants2(uint32_t *PL, uint32_t *PR, uint32_t *CL, uint32_t *CR, uint32_t *Q, uint32_t *D, uint32_t *U, uint32_t *V, uint32_t W)
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

void newAttack()
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
	uint32_t *variables = getLinearEncodeVariables(P[0], constants);

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

	// finde W
	uint32_t d = CL[0] ^ CL[1] ^ 0x02000000;
	uint32_t e = CL[0] ^ CL[2] ^ 0x00000002;
	uint32_t *wSolutions = NULL;

	uint32_t wSolutionsCount = getSolutionsForXFrom3_7(D[0], D[1], D[2], d, e, &wSolutions);
	wSolutionsCount = getSolutionsFor5_9(D[0], D[3], D[4], CL[0], CL[3], CL[4], &wSolutions, wSolutionsCount);


	//finde V0

	uint32_t *v0Solutions;
	uint32_t v0SolutionCount;

	struct triplet *triplets = NULL;
	int tripletsCount = 0;
	for(int i = 0; i < wSolutionsCount; ++i)
	{
		d = CL[0] ^ CL[5] ^ G(D[0] ^ wSolutions[i]) ^ G(D[5] ^ wSolutions[i]);
		e = CL[0] ^ CL[6] ^ G(D[0] ^ wSolutions[i]) ^ G(D[6] ^ wSolutions[i]);
		v0SolutionCount  = getSolutionsForXFrom3_7(PL[0], PL[5], PL[6], d, e, &v0Solutions);

		triplets = realloc(triplets, (tripletsCount + v0SolutionCount) * sizeof(struct triplet));
		for(int j = 0; j < v0SolutionCount; ++j)
		{
			//printf("0x%"PRIx32"\n", v0Solutions[j]);
			struct triplet triple = getTripletFrom5_5(CL[0], PL[0], v0Solutions[j], D[0], wSolutions[i]);

			// Adde nur die Triple, die fuer die anderen Plaintexte (5.5) erfuellen
			if(doesSatisfy5_5ForOtherPlaintexts(triple, PL, CL, D))
			{
				triplets[tripletsCount] = triple;
				tripletsCount++;
			}
		}
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
		int V12SolutionsCount = getSolutionsForXFrom3_1(PL[12], G(D[12] ^ triplets[i].W) ^ CL[12] ^ U12, &V12Solutions);
		uint32_t *V14Solutions = NULL;
		int V14SolutionsCount = getSolutionsForXFrom3_1(PL[14], G(D[14] ^ triplets[i].W) ^ CL[14] ^ U14, &V14Solutions);

		for(int j = 0; j < V12SolutionsCount; ++j)
		{
			if(doesSatisfy5_4(CL[13], U13, PL[13], V12Solutions[j], D[13], triplets[i].W))
			{

				for(int k = 0; k < V14SolutionsCount; ++k)
				{
					if(doesSatisfy5_4(CL[15], U15, PL[15], V14Solutions[k], D[15], triplets[i].W))
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
						uint32_t *calculatedKeyConstants = calculateKeyConstants2(PL,PR,CL,CR,Q,D,U,V,triplets[i].W);

						if(calculatedKeyConstants != NULL)
						{
							printf("M1: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[M1], calculatedKeyConstants[M1]);
							printf("N1: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[N1], calculatedKeyConstants[N1]);
							printf("M2: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[M2], calculatedKeyConstants[M2]);
							printf("N2: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[N2], calculatedKeyConstants[N2]);
							printf("M3: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n", constants[M3], calculatedKeyConstants[M3]);
							printf("N3: 0x%"PRIx32"\tBerechnet: 0x%"PRIx32"\n\n", constants[N3], calculatedKeyConstants[N3]);
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
	free(variables);
	free(constants);
}
