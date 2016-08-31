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

/**
 * Implementierung der S0 und S1 Funktion aus dem Paper
 *
 * Si(x, y) = Rot_2 ( x + y + i (Mod 256))
 *
 * @param x
 * @param y
 * @param i - 0 oder 1
 *
 * @result 32 bit Zahl
 */
uint8_t S(int x, int y, Index_t i);

/**
 * Implementierung der fK Funktion aus dem Paper
 *
 * c = fK(a, b)
 *
 * @param aDWord - a
 * @param bDWord - b
 *
 * @return c
 */
uint32_t fK(uint32_t aDWord, uint32_t bDWord);

/**
 * Implementierung der f Funktion aus dem Paper
 *
 * c = f(a, b)
 *
 * @param aDWord - a
 * @param b		 - b
 *
 * @result c (32 bit)
 */
uint32_t f(uint32_t aDWord, uint16_t b);

/**
 * Splittet ein 32 bit uint in 4 bytes auf
 *
 * @param dWord - Das zu splittende Doppelwort
 * @param *buf  - Pointer auf die 4 bytes
 */
void splitToBytes(uint32_t dWord, uint8_t *buf);

/**
 * Konkateniert 4 Bytes zu einem Doppelwort.
 *
 * @param b0 - Most significant byte
 * @param b1
 * @param b2
 * @param b3
 *
 * @result Das resultierende Doppelwort
 */
uint32_t bytesToUint32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);

/**
 * Konkateniert zwei Woerter zu einem Doppelwort.
 *
 * @param w0 - Linke Haelfte des Doppelworts
 * @param w1 - Rechte Haelfte des Doppelworts
 *
 * @return Das resultierende Doppelwort
 */
uint32_t wordsToUint32(uint16_t w0, uint16_t w1);

/**
 * Berrechnet die 12 16 bit Subkeys aus dem eigentlichen 64 bit Schluessel
 *
 * @param key - 64 bit Schluessel
 *
 * @return *k - Pointer auf die 12 16 bit Subkeys
 */
uint16_t *compSubKeys(uint64_t key);

/**
 * Kodieren eines 64 bit Plaintextblocks
 *
 * @param p 	- 64 bit Plaintextblock
 * @param *k	- Pointer auf 16 bit Teilschluessel
 *
 * @return		- 64 bit Cipherblock
 */
uint64_t encode(uint64_t p, uint16_t *k);

/**
 * Dekodieren eines 64 bit Chiffreblocks
 *
 * @param c 	- 64 bit Cipherblock
 * @param *k	- Pointer auf 12 16 bit Subkeys
 *
 * @return		- 64 bit Plaintextblock
 */
uint64_t decode(uint64_t c, uint16_t * k);

#endif /* HEADER_FEAL_H_ */
