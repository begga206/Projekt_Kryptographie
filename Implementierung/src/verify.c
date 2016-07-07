#include "verify.h"


/**
 * Funktion, um die Verifikation einheitlich zu halten.
 * Gibt aus, ob die Funktion/Test unter
 * der VerificationID erfolgreich verifiziert wurde.
 *
 * @param id 		 - ID der Funktion/Test, der verifiziert werden soll
 * @param withOutput - 1 = Es soll der Verlauf des Tests ausgegeben werden
 *
 * @return - 1, wenn OK; 0, wenn nicht
 */
int verify(VerificationID id, int withOutput)
{
	switch(id)
	{
	case FUNCTION_S:
		return verifyFunctionS(withOutput);
	case FUNCTION_F_K:
		return verifyFunctionFK(withOutput);
	case FUNCTION_F:
		return verifyFunctionF(withOutput);
	case FUNCTION_SPLIT_TO_BYTES:
		return verifyFunctionSplitToBytes(withOutput);
	case FUNCTION_BYTES_TO_UINT32:
		return verifyFunctionBytesToUint32(withOutput);
	case FUNCTION_WORDS_TO_UINT32:
		return verifyFunctionBytesToUint32(withOutput);
	case FUNCTION_COMP_SUBKEYS:
		return verifyFunctionCompSubKeys(withOutput);
	case FUNCTION_ENCODE:
		return verifyFunctionEncode(withOutput);
	case FUNCTION_DECODE:
		return verifyFunctionDecode(withOutput);
	case FUNCTION_G:
		return verifyFunctionG(withOutput);
	case FUNCTION_THETA_L:
		return verifyFunctionThetaL(withOutput);
	case FUNCTION_THETA_R:
		return verifyFunctionThetaR(withOutput);
	case FUNCTION_COMP_CONSTANTS:
		return verifyFunctionComputeConstants(withOutput);
	case FUNCTION_LINEAR_ENCODE:
		return verifyFunctionLinearEncode(withOutput);
	case FUNCTION_LINEAR_DECODE:
		return verifyFunctionLinearDecode(withOutput);
	default:
		printf("Unknown ID.\n");
		return 0;
	}
}

int verifyFunctionS(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionFK(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionF(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionSplitToBytes(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionBytesToUint32(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionWordsToUint32(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionCompSubKeys(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionEncode(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionDecode(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionG(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionThetaL(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionThetaR(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

/**
 * Funktion verifiziert die Korrektheit der Funktion computeConstants.
 * Es werden Plaintexte mittels FEAL Algorithmus verschluesselt, dann
 * die Schluessel abhaengigen Konstaten berechnet und die Ciphertexte
 * mit Hilfe der Konstanten entschluesselt. Stimmen diese dekodierten
 * Plaintexte mit den urspruenglichen ueberein, arbeitet
 * computeConstants korrekt.
 *
 * @return 1, wenn die Plaintexte uebereinstimmen; 0, falls nicht.
 */
int verifyFunctionComputeConstants(int withOutput)
{
	// Testschlüssel
	uint64_t key = 0xFF00FF00FF00FF00;

	// Zuerst werden die 12 16 bit subkeys errechnet
	uint16_t *subkeys = compSubKeys(key);

	// Danach werden 20 Plaintexte nach der Definition aus dem Paper erzeugt.
	uint64_t *P = choosePlainTexts();

	// Allokiere Speicher fuer die Ciphertextbloecke und wende das FEAL-
	// Verfahren zur Verschluesselung an.
	uint64_t *C = malloc(20 * sizeof(uint64_t));
	for(int i = 0; i < 20; ++i)
	{
		C[i] = encode(P[i], subkeys);
	}

	// Berechne die 6 32-bit Schluesselabhaengigen Konstanten (2.6).
	uint32_t *constants = computeConstants(subkeys);
	if(withOutput)
	{
		for(int i = 0; i < 6; ++i)
		{
			printf("Konstante %d: 0x%" PRIx32 "\n", i, constants[i]);
		}
		printf("\n\n");
	}

	// Entschluessel mit Hilfe der Konstanten die 20 Ciphertextbloecke und
	// vergleiche, ob sie mit den urspruenglichen Plaintextbloecken ueber-
	// einstimmen.
	uint64_t decodedP[20];
	int isEqual = 1;
	for(int i = 0; i < 20; ++i)
	{
		decodedP[i] = linearDecode(C[i], constants);
		if(decodedP[i] != P[i])
		{
			isEqual = 0;
		}
		if(withOutput)
		{
			printf("Urspruenglicher Plaintext: 0x%" PRIx64 "\t", P[i]);
			printf("Dekodierter Plaintext: 0x%" PRIx64 "\n", decodedP[i]);
		}
	}

	return isEqual;
}

int verifyFunctionLinearEncode(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}

int verifyFunctionLinearDecode(int withOutput)
{
	printf("Not implemented yet.\n");
	return 0;
}
