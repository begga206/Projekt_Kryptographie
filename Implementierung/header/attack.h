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

/*
 * Enum fuer die 6 Schluesselkonstanten
 */
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

uint64_t *choosePlainTexts();

int getSolutionsForXFrom3_1(uint32_t aDWord, uint32_t bDWord, uint32_t **solutions);

int getSolutionsForXFrom3_7(uint32_t aDWord, uint32_t bDWord, uint32_t cDWord,
			uint32_t dDWord, uint32_t eDWord, uint32_t ** solutions);

int getTriplets(uint64_t *C, uint32_t *D, uint64_t *P, uint32_t *wSolutions,
		int wSolutionCount, uint32_t *vSolutions, int vSolutionCount, struct triplet **triplets);

int getSolutionsFor5_9(uint32_t D0, uint32_t D3, uint32_t D4, uint32_t C0L, uint32_t C3L,
		uint32_t C4L, uint32_t **solutions, int oldSolutionsCount);

int getU0Solutions(uint32_t C0L, uint32_t P0L, uint32_t D0,uint32_t *wSolutions, int wSolutionCount,
		uint32_t *vSolutions, int vSolutionCount, uint32_t * uSolutions);

int constrainXOuterBitsZero(uint8_t x, uint32_t **solutions, int solutionsCount);

uint32_t *attack(uint64_t *P, uint64_t *C);

uint32_t *attack2(uint64_t *C, uint64_t *P, uint32_t *D, uint32_t *Q, struct triplet *triplets, int tripletCount);

uint32_t *calculateKeyConstants(uint64_t *C, uint64_t *P, uint32_t *D, uint32_t *Q, struct triplet triplet, uint32_t V12, uint32_t V14);

uint32_t *computeConstants(uint16_t *subkeys);

int doesStaisfy5_4(uint32_t CiL, uint32_t Ui, uint32_t PiL, uint32_t Vi, uint32_t Di , uint32_t W);

int doesSatisfy5_5(uint32_t CiL, struct triplet trippel, uint32_t PiL, uint32_t Di);

uint64_t linearDecode(uint64_t C, uint32_t *constants);

#endif /* HEADER_ATTACK_H_ */
