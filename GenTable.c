/*
  Harshdeep Goraya
  Rainbow Tables Proj
  ENEE457 : 0101
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "aes.h"

// AES Source: https://tls.mbed.org/aes-source-code
#define s32 4
#define s128 16
#define CHAIN 100
#define CLEAR (array, len) (array[len/CHAR_BIT] &= ~(1 << (len%CHAR_BIT)) )
#define SET (array, len)   (array[len/CHAR_BIT] |=  (1 << (len%CHAR_BIT)) )
#define TEST (array, len)  (array[len/CHAR_BIT] &   (1 << (len%CHAR_BIT)) )
#define BYT (numBits) ((numBits + CHAR_BIT - 1) / CHAR_BIT)

struct rainbows {
  unsigned int key;
	unsigned char aes[s128];
};

unsigned long int linesMax (unsigned int, unsigned int);
unsigned int nextKey (char*, unsigned int, unsigned int);
void key128 (unsigned char*, unsigned int, unsigned int);
void reset (unsigned char*);
unsigned int reduce (unsigned char*, unsigned int, unsigned int);

int main (int argc, char **argv) {
	unsigned int len = 0;
	unsigned int size = 0;

	if ((argc != 0) && (argc != 2)) {
        printf("./gentable Len <Length of Password>\n");
        exit(1);
    } else if (argc == 2) {
		if((sscanf (argv[1], "%i", &len) != 1) || (len < 0 || len > 32)) {
      printf("Invalid Len\n");
      exit(1);
    }
		if ((sscanf (argv[2], "%i", &size) != 1) || (size < 0 || size > 32)) {
      printf("Invalid Size\n");
      exit(1);
    }
}

	FILE *rainFile;
	rainFile = fopen("rainbow.txt","w");
	if(rainFile  == NULL) {
 		perror("File write error");
        exit(1);
	}

	struct rainbows rainbee;
	unsigned int max = (1 << len);
	char *bitmap = (char*) calloc(max, 1);
	char *aes_bitmap = (char*) calloc(max, 1);

  // AES Source: https://tls.mbed.org/aes-source-code
	aes_context _aes;
	unsigned char plaintext[s128] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char k128[s128] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char ciphertext[16];
	unsigned int k32 = 0;
	unsigned int first = 0;
	unsigned int count=0;
	unsigned int countAES = 0;

	unsigned long int lines = (unsigned long int) linesMax (len, size);
	unsigned long int counter;

  for(counter = 0; counter < lines; counter++) {
		if (countAES == max)
			break;
		if (count == max) {
			int temp;
			for (temp = 0; temp < BYT (max); temp++)
				bitmap[temp] = aes_bitmap[temp];

			k32 = 0;
			first = 0;
			count = 0;
		}

		first = nextKey(bitmap, first, max);
		k32 = first;
		count++;

		unsigned int temp;
    for (temp = 0; temp < CHAIN; temp++) {
			key128 (k128, k32, len);
			memset (ciphertext, 0, 16);
   			aes_setkey_enc (&_aes, k128, 128);
   			aes_crypt_ecb (&_aes, AES_ENCRYPT, plaintext, ciphertext);
   			k32 = reduce (ciphertext, len, i);
			if(!TEST(bitmap, k32)) {
				SET(bitmap, k32);
				count++;
			}	else {
				break;
   		}
    }

		if (!TEST (aes_bitmap, first)) {
			SET (aes_bitmap, first);
			countAES++;
			rainbee.key=first;
			int cnt;
      for (cnt = 0; cnt < s128; cnt++)
        rainbee.aes[j] = ciphertext[j];
			fwrite (&rainbee, sizeof(struct rainbows), 1, rainFile);
		}
	}

	fclose(rainFile);
	free(bitmap);
	printf("\n");
	return 0;
}

unsigned long int linesMax (unsigned int len, unsigned int size) {
	unsigned long int s2 = (1 << size);
	unsigned long int lines = ((s2)*3*16)/(s32+s128);
	return (lines);
}

unsigned int nextKey (char *bitmap, unsigned int counter, unsigned int size) {
   	while ((TEST(bitmap, counter)) && counter < size - 1)
   		counter++;
   	if(!TEST(bitmap, counter))
		SET(bitmap, counter);

	return counter;
}

void key128 (unsigned char *k128, unsigned int key, unsigned int len) {
	unsigned char byte;
	reset (k128);

	int space = s128 - 1;
	while (key > 0) {
		byte = key & 0xFF;
		k128[space] = byte;
		space--;
		key = key >> 8;
	}
}

void reset (unsigned char *k128) {
	int temp;
	for(temp = 0; temp < s128; temp++)
		k128[i] = 0x00;
}

unsigned int reduce (unsigned char *k128, unsigned int len, unsigned int position) {
	int temp, temp2;
	unsigned int padding = 3;
	unsigned char str[s32];

	for (temp = s128 - s32, temp2 = 0; temp < s128; temp++, temp2++)
		str[temp2] = k128[temp];

	unsigned char str[s32 + 1];
	strcpy (str, "");
	char chr[2];
	strcpy (chr, "");

	for (temp = 0; temp < s32; temp++) {
		sprintf (chr, "%X", str[temp]);
		strcat (str, chr);
	}
	char *symbol;
	unsigned int k32 = strtoul (str, &symbol, 16L);
	k32 = k32 + (position % padding);

  for (temp = len; temp < s32 * 8; temp++)
		k32 &= ~(1 << temp);

	return k32;
}
