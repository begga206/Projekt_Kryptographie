/*
 * attack.h
 *
 *  Created on: 04.05.2016
 *      Author: Lukas
 */

#ifndef HEADER_ATTACK_H_
#define HEADER_ATTACK_H_

#include <stdio.h>
#include "feal.h"
#include "time.h"
#include "tests.h"

typedef enum {
	M1 = 0,
	N1 = 1,
	M2 = 2,
	N2 = 3,
	M3 = 4,
	N3 = 5
} KeyConstantsIndex;

struct triplet{
	uint32_t W;
	uint32_t V0;
	uint32_t U0;
};



uint32_t G(uint32_t aDWord);

uint32_t O_L(uint32_t a);

uint32_t O_R(uint32_t a);

uint64_t *choosePlainTexts();

int getSolutionsForXFrom3_1(uint32_t aDWord, uint32_t bDWord, uint32_t **solutions);

int getSolutionsForXFrom3_7(uint32_t aDWord, uint32_t bDWord, uint32_t cDWord,
			uint32_t dDWord, uint32_t eDWord, uint32_t ** solutions);

uint32_t *attack(uint64_t *P, uint64_t *C);

uint32_t *attack2(uint64_t *C, uint64_t *P, uint32_t *D, uint32_t *Q, struct triplet *triplets, int tripletCount);

uint8_t invS(uint8_t a, uint8_t b, Index_t i);

uint32_t *getInnerBits(uint8_t a0, uint8_t a3, uint8_t b0, uint8_t b3, uint8_t c0, uint8_t c3);

uint32_t *calculateKeyConstants(uint64_t *C, uint64_t *P, uint32_t *D, uint32_t *Q, struct triplet triplet, uint32_t V12, uint32_t V14);

uint64_t linearDecode(uint64_t C, uint32_t *constants);

#endif /* HEADER_ATTACK_H_ */
