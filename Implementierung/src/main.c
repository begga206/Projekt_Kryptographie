/*
 * 	main.c
 *
 *  Created on: 22.04.2016
 *  Author: Lukas
 */

#include <stdio.h>
#include "feal.h"
#include <time.h>
#include <inttypes.h>
#include <util.h>
#include "attack.h"
#include "verify.h"


/**
 * Main-Funktion
 *
 * Wird vor�bergehend f�r Tests genutzt
 */
int main(void)
{
	printf("Beginn Attacke %s\n\n", time_stamp());
	attack();
	printf("Ende Attacke %s", time_stamp());
	return 0;
}
