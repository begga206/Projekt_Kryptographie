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


/**
 * Main-Funktion
 *
 * Wird vor�bergehend f�r Tests genutzt
 */
int main(void)
{
	// Testschl�ssel
	uint64_t key = 0xFF00FF00FF00FF00;
	// Array mit 16 bit subkeys
	uint16_t *k = compSubKeys(key);
	// 20 Plaintexte erstellen
	uint64_t *P = choosePlainTexts();

	// Alle Plaintexte drucken
	if(P)
	{
		for(int i = 0; i < 20; i++)
		{
			printf("P[%d]: 0x%" PRIx64 "\n", i, P[i]);
		}
		printf("\n\n");
		free(P);
	}

	// Kodieren und Dekodieren Test
	if(k)
	{
		uint64_t p = 0xAAAAAAAAAAAAAAAA;	// plain text
		printf("Plaintext: 0x%" PRIx64 "\n", p);
		uint64_t c = encode(p,k);			// cipher
		printf("Encoded: 0x%" PRIx64 "\n", c);
		uint64_t d = decode(c,k);			// decoded == plain text, wenn alles passt :D
		printf("Decoded: 0x%" PRIx64 "\n", d);
		free(k);
	}
	return 0;
}
