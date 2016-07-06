/*
 * 	main.c
 *
 *  Created on: 22.04.2016
 *  Author: Lukas
 */

#include <stdio.h>
#include "feal.h"
#include <time.h>
#include <inttypes.h>
#include "attack.h"
#include "tests.h"


/**
 * Main-Funktion
 *
 * Wird vorübergehend für Tests genutzt
 */
int main(void)
{

	printf("Aufruf main.\n");
	// Testschlüssel
	uint64_t key = 0xFF00FF00FF00FF00;
	// Array mit 16 bit subkeys
	uint16_t *k = compSubKeys(key);
	// 20 Plaintexte erstellen
	printf("Waehle Plaintexte.\n");
	uint64_t *P = malloc(20 * sizeof(uint64_t));//choosePlainTexts();
	uint64_t *C = malloc(20 * sizeof(uint64_t));

	// Diese Plaintexte verwenden, um mit konsistenten Daten zu debuggen
	P[0]= 0x0efe078658612aea;
	P[1]= 0x8e7e0786d8e12aea;
	P[2]= 0x0efe87065861aa6a;
	P[3]= 0x4ebe078618212aea;
	P[4]= 0x0efe47c658616aaa;
	P[5]= 0x18703f4d4eef1221;
	P[6]= 0x5b7d2e040de20368;
	P[7]= 0x74d40a28224b2744;
	P[8]= 0x5e157305088a5e69;
	P[9]= 0x5cc055db0a5f78b7;
	P[10]= 0x77d01994214f34f8;
	P[11]= 0x4f1609161989247a;
	P[12]= 0x46b5511410e50c15;
	P[13]= 0x0ab379725ce32473;
	P[14]= 0x244e513970df4926;
	P[15]= 0x0b22523a5d720f3b;
	P[16]= 0x5b5851a02e6346b5;
	P[17]= 0x69a0162b05d87759;
	P[18]= 0x3fed32d9484d00b3;
	P[19]= 0x62ed78ed241131ae;

	// Alle Plaintexte drucken
	if(P)
	{
		for(int i = 0; i < 20; i++)
		{
			printf("P[%d]: 0x%" PRIx64 "\n", i, P[i]);
		}
		printf("\n\n");
	}

	/*
	// Kodieren und Dekodieren Test
	if(k)
	{
		uint64_t p = 0xAAAAAAAAAAAAAAAA;	// plain text
		printf("Plaintext: 0x%" PRIx64 "\n", p);
		uint64_t c = encode(p,k);			// cipher
		printf("Encoded: 0x%" PRIx64 "\n", c);
		uint64_t d = decode(c,k);			// decoded == plain text, wenn alles passt
		printf("Decoded: 0x%" PRIx64 "\n", d);
		free(k);
	}*/
	/*
	// C fuellen
	for(int i = 0; i < 20; ++i)
	{
		C[i] = encode(P[i], k);
	}

	printf("\n");

	if(P)
	{
		for(int i = 0; i < 20; i++)
		{
			printf("P[%d]: 0x%" PRIx64 "\n", i, P[i]);
		}
		printf("\n\n");
	}
	// Attacke ausfuehren
	uint32_t *constants = NULL;
	printf("Fuehre Attacke aus.\n");
	printf("Pointer P: 0x%" PRIx32 "\n", P);
	constants = attack(P, C);

	if(constants != NULL)
	{
		uint64_t p = 0xAAAAAAAAAAAAAAAA;	// plain text
		printf("Plaintext: 0x%" PRIx64 "\n", p);
		uint64_t c = encode(p,k);			// cipher
		printf("Encoded: 0x%" PRIx64 "\n", c);
		uint64_t d = decode(c,k);			// decoded == plain text, wenn alles passt
		printf("Decoded: 0x%" PRIx64 "\n", d);
		uint64_t ld = linearDecode(c, constants); // decoded == plain text, wenn alles passt
		printf("Linear Decoded: 0x%" PRIx64 "\n", ld);
		free(constants);
	}
	free(k);
	free(C);
	free(P);
	*/
	//testGetSolutionsForXFrom3_7();
	return 0;
}
