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
		return verifyFunctionWordsToUint32(withOutput);
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

int testForS(int x, int y, uint8_t expectedS0, uint8_t expectedS1, int withOutput)
{
	// S0 und S1 fuer die Variablen ausfuehren.
	uint8_t resultS0 = S(x,y,ZERO);
	uint8_t resultS1 = S(x,y,ONE);

	if(withOutput)
	{
		printf("Test S0 mit x = 0x%" PRIx8 " und y = 0x%" PRIx8". Erwartet: 0x%" PRIx8 " Erhalten: 0x%" PRIx8"\n",
				x, y, expectedS0, resultS0);
		printf("Test S1 mit x = 0x%" PRIx8 " und y = 0x%" PRIx8". Erwartet: 0x%" PRIx8 " Erhalten: 0x%" PRIx8"\n",
						x, y, expectedS1, resultS1);
	}
	if(resultS0 != expectedS0 || resultS1 != expectedS1)
		return 0;
	return 1;
}

int verifyFunctionS(int withOutput)
{
	// Test mit x,y = 0. Erwartet: S0=0 , S1=4
	if(!testForS(0,0,0,4,withOutput))
		return 0;

	// Test mit x = 255 und y = 0. Erwartet: S0 = 0xFF , S1 = 0x00
	if(!testForS(255,0,255,0,withOutput))
		return 0;

	// Test mit x = 255 und y = 255. Erwartet: S0 = 0xFB  , S1 = 0xFF
	if(!testForS(255,255,0xFB,255,withOutput))
		return 0;

	return 1;
}

int verifyFunctionFK(int withOutput)
{
	uint32_t a = 0x12345678;
	uint32_t b = 0x9abcdef0;
	uint32_t expected = 0x0f6f04b5;
	uint32_t result = fK(a,b);

	if(withOutput)
	{
		printf("Test fK mit a = 0x%" PRIx32", b = 0x%"PRIx32 ". Expected: 0x%"PRIx32" Result: 0x%"PRIx32"\n",
				a, b, expected, result);
	}
	if(expected != result)
		return 0;
	return 1;
}

int verifyFunctionF(int withOutput)
{
	uint32_t a = 0x12345678;
	uint16_t b = 0xbcde;
	uint32_t expected = 0x012e78c7;
	uint32_t result = f(a,b);

	if(withOutput)
	{
		printf("Test f mit a = 0x%" PRIx32", b = 0x%"PRIx32 ". Expected: 0x%"PRIx32" Result: 0x%"PRIx32"\n",
				a, b, expected, result);
	}
	if(expected != result)
		return 0;
	return 1;
}

int verifyFunctionSplitToBytes(int withOutput)
{
	uint32_t a = 0x12345678;
	uint8_t expected[4] = {0x12, 0x34, 0x56, 0x78};
	uint8_t result[4] = {0};
	int ret = 1;

	splitToBytes(a, result);

	if(withOutput)
	{
		printf("Test SplitToBytes:\nEXP:\tRESULT:\n");
	}
	for(int i = 0; i < 4; ++i)
	{
		if(withOutput)
			printf("0x%" PRIx8 "\t0x%" PRIx8"\n", expected[i], result[i]);
		if(expected[i] != result[i])
			ret = 0;
	}
	return ret;
}

int verifyFunctionBytesToUint32(int withOutput)
{
	uint8_t b0 = 0x12;
	uint8_t b1 = 0x34;
	uint8_t b2 = 0x56;
	uint8_t b3 = 0x78;

	uint32_t expected = 0x12345678;

	uint32_t result = bytesToUint32(b0, b1, b2, b3);

	if(withOutput)
	{
		printf("Test bytesToUint32 mit b0=0x%" PRIx8 ", b1=0x%" PRIx8 ", b2=0x%" PRIx8 ", b3=0x%" PRIx8 " Erwartet: 0x%" PRIx32 " Erhalten: 0x%" PRIx32"\n",
						b0, b1, b2, b3, expected, result);
	}
	if(result != expected)
		return 0;

	return 1;
}

int verifyFunctionWordsToUint32(int withOutput)
{
	uint16_t b0 = 0x1234;
	uint16_t b1 = 0x5678;

	uint32_t expected = 0x12345678;

	uint32_t result = wordsToUint32(b0, b1);

	if(withOutput)
	{
		printf("Test WordsToUint32 mit b0=0x%" PRIx8 ", b1=0x%" PRIx8" Erwartet: 0x%" PRIx32 " Erhalten: 0x%" PRIx32"\n",
						b0, b1, expected, result);
	}
	if(result != expected)
		return 0;

	return 1;
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

	// Entschluessel die 20 Ciphertextbloecke und
	// vergleiche, ob sie mit den urspruenglichen Plaintextbloecken ueber-
	// einstimmen.
	uint64_t decodedP[20];
	int isEqual = 1;
	for(int i = 0; i < 20; ++i)
	{
		decodedP[i] = decode(C[i], subkeys);
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

int verifyFunctionG(int withOutput)
{
	// Wird verifizert mittels der Gleichungen aus (5.7)
	uint32_t a  = 0x12345678;
	uint32_t ga = G(a);

	uint32_t expected = 0x02000000;
	uint32_t result	  = ga ^ G(a ^ 0x80800000);

	if(withOutput)
		printf("Expected: 0x%" PRIx32 "  Result: 0x%" PRIx32 "\n", expected, result);
	if(expected != result)
		return 0;

	expected = 0x00000002;
	result   = ga ^ G(a ^ 0x00008080);
	if(withOutput)
		printf("Expected: 0x%" PRIx32 "  Result: 0x%" PRIx32 "\n", expected, result);
	if(expected != result)
		return 0;

	expected = 0x01000000;
	uint32_t expected2 = 0x03000000;
	result   = ga ^ G(a ^ 0x40400000);
	if(withOutput)
		printf("Expected: 0x%" PRIx32 ", 0x%" PRIx32 "  Result: 0x%" PRIx32 "\n", expected, expected2, result);
	if(expected != result && result != expected2)
		return 0;

	expected = 0x00000001;
	expected2 = 0x00000003;
	result   = ga ^ G(a ^ 0x00004040);
	if(withOutput)
		printf("Expected: 0x%" PRIx32 ", 0x%" PRIx32 "  Result: 0x%" PRIx32 "\n", expected, expected2, result);
	if(expected != result && result != expected2)
		return 0;

	return 1;
}

int verifyFunctionThetaL(int withOutput)
{
	uint32_t a = 0x12345678;
	uint32_t expected = 0x00123400;

	uint32_t result	= theta_L(a);

	if(withOutput)
	{
		printf("Test Theta_L mit a = 0x%" PRIx32 " Erwartet: 0x%" PRIx32 " Erhalten: 0x%" PRIx32"\n",
						a, expected, result);
	}
	if(result != expected)
		return 0;

	return 1;
}

int verifyFunctionThetaR(int withOutput)
{
	uint32_t a = 0x12345678;
	uint32_t expected = 0x00567800;

	uint32_t result	= theta_R(a);

	if(withOutput)
	{
		printf("Test Theta_L mit a = 0x%" PRIx32 " Erwartet: 0x%" PRIx32 " Erhalten: 0x%" PRIx32"\n",
						a, expected, result);
	}
	if(result != expected)
		return 0;

	return 1;
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
	// Testschlüssel
	uint64_t key = 0xFF00FF00FF00FF00;

	// Zuerst werden die 12 16 bit subkeys errechnet
	uint16_t *subkeys = compSubKeys(key);

	// Danach werden 20 Plaintexte nach der Definition aus dem Paper erzeugt.
	uint64_t *P = choosePlainTexts();

	// Allokiere Speicher fuer die Ciphertextbloecke und wende das FEAL-
	// Verfahren zur Verschluesselung an.
	uint64_t C[20];
	for(int i = 0; i < 20; ++i)
	{
		C[i] = encode(P[i], subkeys);
	}

	// Berechne die 6 32-bit Schluesselabhaengigen Konstanten (2.6).
	uint32_t *constants = computeConstants(subkeys);

	// Verschluessel mit Hilfe der Konstanten die 20 Plaintextbloecke und
	// vergleiche, ob sie mit den urspruenglichen Ciphertextbloecken ueber-
	// einstimmen.
	uint64_t linearEncoded[20];
	int isEqual = 1;
	for(int i = 0; i < 20; ++i)
	{
		linearEncoded[i] = linearEncode(P[i], constants);
		if(linearEncoded[i] != C[i])
		{
			isEqual = 0;
		}
		if(withOutput)
		{
			printf("Urspruenglicher Ciphertext: 0x%" PRIx64 "\t", C[i]);
			printf("\"linearer\" Ciphertext: 0x%" PRIx64 "\n", linearEncoded[i]);
		}
	}

	return isEqual;
}

int verifyFunctionLinearDecode(int withOutput)
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

	// Entschluessel mit Hilfe der Konstanten die 20 Ciphertextbloecke und
	// vergleiche, ob sie mit den urspruenglichen Plaintextbloecken ueber-
	// einstimmen.
	uint64_t decodedP[20];
	uint64_t linearDecodedP[20];
	int isEqual = 1;
	for(int i = 0; i < 20; ++i)
	{
		decodedP[i] = decode(C[i], subkeys);
		linearDecodedP[i] = linearDecode(C[i], constants);
		if(linearDecodedP[i] != P[i])
		{
			isEqual = 0;
		}
		if(withOutput)
		{
			printf("Urspruenglicher Plaintext: 0x%" PRIx64 "\t", P[i]);
			printf("Dekodierter Plaintext: 0x%" PRIx64 "\n", decodedP[i]);
			printf("\"linear\" Dekodierter Plaintext: 0x%" PRIx64 "\n", linearDecodedP[i]);
		}
	}

	return isEqual;
}
