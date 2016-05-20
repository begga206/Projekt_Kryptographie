#include "tests.h"


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

uint32_t getRandomUint32()
{
	uint32_t result = ((uint32_t)rand() << 16) + ((uint32_t)rand());
	return result;
}

void testGetSolutionsForXFrom3_7()
{
	srand(time(0));
	uint32_t *solutions = NULL;
	// Erstelle a, b, c, x, d, e
	uint32_t a = getRandomUint32();
	uint32_t b = getRandomUint32();
	uint32_t c = getRandomUint32();
	uint32_t x = 0xABCDEF12;
	uint32_t d = G(x ^ a) ^ G(x ^ b);
	uint32_t e = G(x ^ a) ^ G(x ^ c);

	printf("%s: Anfang getSolutionsForXFrom3_7\n", time_stamp());
	printf("\ta = 0x%" PRIx32 "\n", a);
	printf("\tb = 0x%" PRIx32 "\n", b);
	printf("\tc = 0x%" PRIx32 "\n", c);
	printf("\tx = 0x%" PRIx32 "\n", x);
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
		free(solutions);
	}
	printf("Fertig.");
}
