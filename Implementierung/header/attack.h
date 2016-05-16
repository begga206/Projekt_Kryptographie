/*
 * attack.h
 *
 *  Created on: 04.05.2016
 *      Author: Lukas
 */

#ifndef HEADER_ATTACK_H_
#define HEADER_ATTACK_H_

#include "feal.h"
#include "time.h"

uint32_t G(uint32_t aDWord);

uint32_t O_L(uint32_t a);

uint32_t O_R(uint32_t a);

uint64_t *choosePlainTexts();

int getSolutionsForXFrom3_1(uint32_t aDWord, uint32_t bDWord, uint32_t *solutions);

int getSolutionsForXFrom3_7(uint32_t aDWord, uint32_t bDWord, uint32_t cDWord,
			uint32_t dDWord, uint32_t eDWord, uint32_t * solutions);

#endif /* HEADER_ATTACK_H_ */
