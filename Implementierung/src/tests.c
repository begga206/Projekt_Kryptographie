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

void testIfWisInWSolutions()
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

	uint32_t U0 = variables[Y0] ^ constants[N3];
	uint32_t V0 = constants[M1] ^ G(variables[Y0]);
	uint32_t W  = constants[M3] ^ constants[N2];

	printf("U0 = 0x%" PRIx32 " V0 = 0x%" PRIx32 " W = 0x%" PRIx32"\n\n", U0,
			V0, W);

	uint32_t CL[20];
	uint32_t CR[20];
	uint32_t PL[20];
	uint32_t PR[20];
	uint32_t D[20];
	uint32_t Q[20];

	for(int i = 0; i < 20; ++i)
	{
		CL[i] = (uint32_t)(C[i]>>32);
		CR[i] = (uint32_t)(C[i]);
		PL[i] = (uint32_t)(P[i]>>32);
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
		printf("W Kandidat: 0x%" PRIx32 "\n", wSolutions[i]);
		d = CL[0] ^ CL[5] ^ G(D[0] ^ W) ^ G(D[5] ^ wSolutions[i]);
		e = CL[0] ^ CL[6] ^ G(D[0] ^ W) ^ G(D[6] ^ wSolutions[i]);
		v0SolutionCount  = getSolutionsForXFrom3_7(PL[0], PL[5], PL[6], d, e, &v0Solutions);

		triplets = realloc(triplets, (tripletsCount + v0SolutionCount) * sizeof(struct triplet));
		for(int j = 0; j < v0SolutionCount; ++j)
		{
			printf("0x%"PRIx32"\n", v0Solutions[j]);
			triplets[tripletsCount] = getTripletFrom5_5(CL[0], PL[0], v0Solutions[j], D[0], wSolutions[i]);
			tripletsCount++;
		}
	}

	for(int i = 0; i < tripletsCount; ++i)
	{
		printf("U0 = 0x%" PRIx32 " V0 = 0x%" PRIx32 " W = 0x%" PRIx32"\n\n", triplets[i].U0,
				triplets[i].V0, triplets[i].W);
	}

	printf("d = 0x%"PRIx32" == 0x%"PRIx32"\n", d, G(V0 ^ PL[0])^G(V0 ^ PL[5]));
	printf("e = 0x%"PRIx32" == 0x%"PRIx32"\n", e, G(V0 ^ PL[0])^G(V0 ^ PL[6]));

	free(triplets);
	free(wSolutions);
	free(v0Solutions);
}
