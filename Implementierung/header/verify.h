/*
 * verify.h
 *
 *  Created on: 07.07.2016
 *      Author: Lukas
 */

#ifndef HEADER_VERIFY_H_
#define HEADER_VERIFY_H_

#include <stdio.h>
#include <time.h>
#include "attack.h"
#include <inttypes.h>

/**
 * Enum zum Halten von IDs fuer unsere Verifikationen/Tests
 */
typedef enum {
	FUNCTION_S,
	FUNCTION_F_K,
	FUNCTION_F,
	FUNCTION_SPLIT_TO_BYTES,
	FUNCTION_BYTES_TO_UINT32,
	FUNCTION_WORDS_TO_UINT32,
	FUNCTION_COMP_SUBKEYS,
	FUNCTION_ENCODE,
	FUNCTION_DECODE,
	FUNCTION_G,
	FUNCTION_THETA_L,
	FUNCTION_THETA_R,
	FUNCTION_COMP_CONSTANTS,
	FUNCTION_LINEAR_ENCODE,
	FUNCTION_LINEAR_DECODE
}VerificationID;

int verify(VerificationID id, int withOutput);
int verifyFunctionS(int withOutput);
int verifyFunctionFK(int withOutput);
int verifyFunctionF(int withOutput);
int verifyFunctionSplitToBytes(int withOutput);
int verifyFunctionBytesToUint32(int withOutput);
int verifyFunctionWordsToUint32(int withOutput);
int verifyFunctionCompSubKeys(int withOutput);
int verifyFunctionEncode(int withOutput);
int verifyFunctionDecode(int withOutput);
int verifyFunctionG(int withOutput);
int verifyFunctionThetaL(int withOutput);
int verifyFunctionThetaR(int withOutput);
int verifyFunctionComputeConstants(int withOutput);
int verifyFunctionLinearEncode(int withOutput);
int verifyFunctionLinearDecode(int withOutput);

#endif /* HEADER_VERIFY_H_ */
