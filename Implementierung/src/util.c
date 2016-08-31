#include <util.h>

/**
 * Erstellt einen Timestamp
 *
 * @return Timestamp als String
 */
char *time_stamp(){

	char *timestamp = (char *)malloc(sizeof(char) * 16);
	time_t ltime;
	ltime=time(NULL);
	struct tm *tm;
	tm=localtime(&ltime);

	sprintf(timestamp,"%04d.%02d.%02d.  %02d:%02d:%02d", tm->tm_year+1900, tm->tm_mon,
		tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	return timestamp;
}

/**
 * Erstellt eine pseudo zufaellige 32 bit Zahl
 *
 * @return pseudo random 32 bit Uint
 */
uint32_t getRandomUint32()
{
	uint32_t result = ((uint32_t)rand() << 16) + ((uint32_t)rand());
	return result;
}
