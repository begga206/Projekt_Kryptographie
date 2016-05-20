/*
 * feal.h
 *
 *  Created on: 22.04.2016
 *      Author: Lukas
 */

#ifndef HEADER_FEAL_H_
#define HEADER_FEAL_H_

#include <stdlib.h>
#include <stdint.h>


typedef enum {ZERO = 0, ONE = 1} Index_t;

uint8_t S(int x, int y, Index_t i);
uint32_t fK(uint32_t aWord, uint32_t bWord);
uint32_t f(uint32_t aWord, uint16_t b);
void splitToBytes(uint32_t word, uint8_t *buf);
uint32_t bytesToUint32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
uint16_t *compSubKeys(uint64_t key);
uint64_t encode(uint64_t p, uint16_t *k);
uint64_t decode(uint64_t c, uint16_t * k);

#endif /* HEADER_FEAL_H_ */
