/*
 * main.c
 *
 *  Created on: 22.04.2016
 *      Author: Lukas
 */

#include <stdio.h>
#include "feal.h"

void printBits(uint8_t num)
{
	uint8_t maxPow = 1 << 7;
	for(int i = 0; i < 8; i++){
		printf("%u", num&maxPow ? 1 : 0);
		num = num <<1;
	}
	printf("\n");
}

int main(void)
{
	uint64_t key = 0xFF00FF00FF00FF00;
	uint16_t *k = compSubKeys(key);

	if(k)
	{
		uint64_t p = 0xAAAAAAAAAAAAAAAA;

		printf("Before encoding: %d\n", p);

		uint64_t c = encode(p,k);

		printf("Encoded: %d\n", c);

		uint64_t d = decode(c,k);

		printf("Decoded: %d\n", d);

		//free(k);
	}
	return 0;
}
