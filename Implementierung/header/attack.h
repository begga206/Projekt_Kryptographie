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
#include <inttypes.h>

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

/*
 * Enum fuer die vom Textblock abhaengigen Variablen
 */
typedef enum
{
	X0,
	X1,
	X2,
	Y0,
	Y1,
	Y2
} Variables;

/*
 * Struct fuer das tripel, welches in 5.3 vorgestellt wird
 */
struct triplet{
	uint32_t W;
	uint32_t V0;
	uint32_t U0;
};

/**
 * Hilfsfunktion G aus dem Paper
 * c = G(a) 	(2.1)
 *
 * @param aDWord - 32 bit parameter
 * @return c
 */
uint32_t G(uint32_t aDWord);

/**
 *
 */
uint32_t theta_L(uint32_t a);

/**
 *
 */
uint32_t theta_R(uint32_t a);

/**
 * Wähle die 20 Plaintexte, die nötig für die Attacke sind, nach der Vorgabe
 * aus der Quelle.
 *
 * @return Pointer auf die Plaintextbloecke
 */
uint64_t *choosePlainTexts();

/**
 * Implementierung der Funktionen 3.2-3.4 um für G(x ^ a) = b Loesungen fuer
 * x zu finden. Diese sind in einem Array gespeichert.
 *
 * @param aDWord 		- a
 * @param bDWord 		- b
 * @param **solutions 	- Wo die Loesungen gespeichert sind
 *
 * @return Anzahl an Loesungen fuer x
 */
int getSolutionsForXFrom3_1(uint32_t aDWord, uint32_t bDWord, uint32_t **solutions);

/**
 * Schaut fuer z1, z2, x0, x3, ob die Gleichungen aus (3.6) korrekt sind.
 * Jedoch fuer zwei Gleichungen simultan (3.7). Falls alle Gleichungen
 * korrekt sind, wird die Loesung abgespeichert.
 *
 * G(x ^ a) ^ G(x ^ b) = d
 * G(x ^ a) ^ G(x ^ c) = e			(3.7)
 *
 * @param aDWord 	- a
 * @param bDWord 	- b
 * @param cDWord	- c
 * @param dDWord	- d
 * @param eDWord	- e
 * @param solutions - Wo die Loesungen gespeichert sind
 *
 * @return Anzahl der gefundenen Loesungen
 */
int getSolutionsForXFrom3_7(uint32_t aDWord, uint32_t bDWord, uint32_t cDWord,
		uint32_t dDWord, uint32_t eDWord, uint32_t ** solutions);

/*
 * Schraenkt die Loesungen an Ws ein, indem geprueft wird, ob W die folgenden
 * Gleichungen loest:
 *
 * 	G(D0 ^ W) ^ G(D3 ^ W) ^ C0L ^ C3L = 01000000 || 03000000	(5.9)
 * 	G(D0 ^ W) ^ G(D4 ^ W) ^ C0L ^ C4L = 00000001 || 00000003
 *
 * 	@param D0
 * 	@param D3
 * 	@param D4
 * 	@param C0L
 * 	@param C3L
 * 	@param C4L
 * 	@param solutions		- Wo die bisherigen Loesungen gespeichert sind
 * 	@param solutionsCount 	- Anzahl der bisherigen Loesungen
 */
int getSolutionsFor5_9(uint32_t D0, uint32_t D3, uint32_t D4, uint32_t C0L,
		uint32_t C3L, uint32_t C4L, uint32_t **solutions, int oldSolutionsCount);

/**
 * Berechnet Loesungen fuer U0, abhaengig von W und V0 Loesungen mit Hilfe der
 * Gleichung:
 * 	CiL ^ U0 ^ G(PiL ^ V0) ^ G(Di ^ W) = 0		i = 0 		(5.5)
 *
 * 	@param C0L
 * 	@param P0L
 * 	@param D0
 * 	@param wSolutions
 * 	@param wSolutionsCount
 * 	@param vSolutions
 * 	@param vSolutionsCount
 * 	@param uSolutions
 *
 * 	@return Anzahl an Loesungen fuer U0
 */
int getU0Solutions(uint32_t C0L, uint32_t P0L, uint32_t D0,
		uint32_t *wSolutions, int wSolutionCount,
		uint32_t *vSolutions, int vSolutionCount, uint32_t * uSolutions);

/**
 * Prueft, ob die Parameter Gleichung 5.4 erfuellen:
 *
 * CiL ^ Ui ^ G(PiL ^ Vi) ^ G(Di ^ W) = 0 	(5.4)
 *
 * @param CiL
 * @param PiL
 * @param Vi
 * @param Di
 * @param W
 *
 * @return 1, wenn erfuellt ; 0, wenn nicht
 */
int doesSatisfy5_4(uint32_t CiL, uint32_t Ui, uint32_t PiL, uint32_t Vi,
		uint32_t Di , uint32_t W);

/**
 * Prueft, ob die Parameter Gleichung 5.5 erfuellen:
 *
 * 	CiL ^ U0 ^ G(PiL ^ V0) ^ G(Di ^ W) = 0		i = 0 		(5.5)
 *
 * @param CiL
 * @param trippel
 * @param PiL
 * @param Di
 *
 * @return 1, wenn erfuellt ; 0, wenn nicht
 */
int doesSatisfy5_5(uint32_t CiL, struct triplet trippel, uint32_t PiL,
		uint32_t Di);

/**
 * Pruef, ob alle mitgegebenen Plaintextbloecke die Gleichung 5.5 erfuellen.
 *
 * @param triple
 * @param PL - Linke Haelfte der 64 bit Plaintextbloecke
 * @param CL - Linke Haelfte der 64 bit Ciphertextblocke
 * @param D - Siehe 4.2 im Paper
 *
 * @return 1, wenn alle erfuellt; 0, wenn nicht
 */
int doesSatisfy5_5ForOtherPlaintexts(struct triplet triple, uint32_t *PL,
		uint32_t *CL, uint32_t *D);

/**
 * Berechnet die Schluesselkonstanten mithilfe der Subkeys
 *
 * @param subkeys - Subkeys des 64 bit Schluessels
 *
 * @return die Schluesselkonstanten
 */
uint32_t *computeConstants(uint16_t *subkeys);

/**
 * Verschluesselung eines Plaintextblocks mithilfe der Schluesselkonstanten
 *
 * @param P - Plaintextblock
 * @param constants - Schluesselkonstanten
 *
 * @return Ciphertextblock
 */
uint64_t linearEncode(uint64_t P, uint32_t *constants);

/**
 * Funktion, die einen Cipherblock mit Hilfe der Schluesselkonstanten dekodiert.
 *
 * @param C 		- Cipherblock
 * @param constants	- Schluesselkonstanten
 *
 * @return 64 Plaintextblock
 */
uint64_t linearDecode(uint64_t C, uint32_t *constants);

/**
 * Bekomme die 6 vom Plaintext abhaengigen Verschluesselvariablen
 *
 * @param P - Plaintextblock
 * @param constants - Schluesselkonstanten
 *
 * @return die abhaengigen Variablen
 */
uint32_t *getLinearEncodeVariables(uint64_t P, uint32_t *constants);

/**
 * Bekomme die 6 vom Ciphertext abhaengigen Entschluesselungsvariablen
 *
 * @param C - Ciphertextblock
 * @param constants - Schluesselkonstanten
 *
 * @return die abhaengigen Variablen
 */
uint32_t *getLinearDecodeVariables(uint64_t C, uint32_t *constants);

/**
 * Berechnet die Schluesselkonstanten anhand der bereits gefundenen Werte
 * in der Attacke
 *
 * @param PL - linke Plaintextseite
 * @param PR - rechte Plaintextseite
 * @param CL - linke Ciphertextseite
 * @param CR - rechte Ciphertextseite
 * @param Q  - siehe 4.1
 * @param D  - siehe 4.2
 * @param U  - U Wert fuer jeden Textblock
 * @param V  - V Wert fuer jeden Textblock
 * @param W  - W Wert fuer jeden Textblock
 *
 * @return Schluesselkonstanten
 */
uint32_t *calculateKeyConstants(uint32_t *PL, uint32_t *PR, uint32_t *CL,
		uint32_t *CR, uint32_t *Q, uint32_t *D, uint32_t *U, uint32_t *V,
		uint32_t W);

/**
 * Fuehrt die Attacke nach Murphy aus
 */
void attack();


#endif /* HEADER_ATTACK_H_ */
