/*
  Harshdeep Goraya
  Rainbow Tables
  ENEE457 : 0101
*/

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"

#define s32 4
#define s128 16
#define CLEAR (array, len) (array[len/CHAR_BIT] &= ~(1 << (len % CHAR_BIT)) )
#define SET (array, len)   (array[len / CHAR_BIT] |=  (1 << (len % CHAR_BIT)) )
#define TEST (array, len)  (array[len / CHAR_BIT] &   (1 << (len % CHAR_BIT)) )
#define BYT (numBits) ((numBits + CHAR_BIT - 1) / CHAR_BIT)
typedef int bool;
enum { false, true };

struct rainbows {
    unsigned int key;
	unsigned char aes[s128];
};

// AES Source: https://tls.mbed.org/aes-source-code
bool rainCheck (char*, unsigned char*, unsigned int*, unsigned int);
void cipher (unsigned char*, unsigned char*);
bool compCipher (unsigned char*, unsigned char*);
unsigned int nextKey (char*, unsigned int, unsigned int);
void key128 (unsigned char*, unsigned int, unsigned int);
void reset (unsigned char*);
unsigned int reduce (unsigned char*, unsigned int, unsigned int);

int main (int argc, char **argv) {
	unsigned int len = 0;
	unsigned int size = 0;
	unsigned char plaintext[s128] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char k128[s128] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char ciphertext[16];
	memset (ciphertext, 0, 16);
	unsigned char out[16];
	aes_context _aes;

	if ((argc != 0) && (argc != 3)) {
    printf ("./crack Len <Length of Password> Ciphertext <Ciphertext (Hex)>\n");
    exit(1);
  } else if (argc == 3) {
		if ((sscanf (argv[1], "%i", &len) != 1) || (len < 0 || len >= 32)) {
      printf ("Invalid Size\n");
      exit(1);
    }
  }
		unsigned char* in = argv[3];
		cipher (in, ciphertext);
	}
	unsigned int max = (1 << len);
	char *bitmap = (char*) calloc(max, 1);

	unsigned int k32 = 0;
	unsigned int kCount = 0;
  unsigned int key[1];

    if(rainCheck (bitmap, ciphertext, key, len) == true) {
    	bool flag = false;
    	k32 = key[0];
		  int temp = 0;
    	while (flag == false) {
        getK128 (k128, k32, len);
        memset (out, 0, 16);
   			aes_setkey_enc (&_aes, k128, 128);
   			aes_crypt_ecb (&_aes, AES_ENCRYPT, plaintext, out);

   			if (compCipher (ciphertext, out) == true)
   				flag = true;
   			else
   				k32 = reduceKey (out, len, kCount);
			kCount++;
   		}
    	printf ("\nPassword is %X. AES was evaluated %u times\n", k32, kCount);
    } else {
    	bool flag = false;
    	k32 = 0;
    	while (flag == false) {
        k32 = nextKey (bitmap, k32, max);
        getK128 (k128, k32, n);
        memset (out, 0, 16);
   			aes_setkey_enc (&_aes, k128, 128);
   			aes_crypt_ecb (&_aes, AES_ENCRYPT, plaintext, out);

   			if (compCipher (ciphertext, out) == true)
   				flag = true;
			kCount++;
    	}
    	printf ("\nPassword is %X. AES was evaluated %u times\n", k32, kCount);
    }
    free(bitmap);
}

void cipher (unsigned char *in, unsigned char *ciphertext) {
	int value;
	unsigned char store[2];
	strcpy (store, "");
	unsigned char symb, symb2;
	int temp, temp2;

	for (temp = 2, temp2 = 0; temp < strlen(in); temp += 2, temp2++) {
		sprintf (store, "%c", in[temp]);
		sscanf (store, "%X", &value);
		symb = (char) value;
		symb <<= 4;
		sprintf (store, "%c", in[temp + 1]);
		sscanf (store, "%X", &value);
		symb2 = (char) value;
		symb |= symb2;
		ciphertext[temp2] = symb;
	}
}

bool compCipher (unsigned char *cipher1, unsigned char *cipher2) {
	bool flag = true;
	int temp;
	for (temp = 0; temp < s128; temp++)
		if (cipher1[temp] != cipher2[temp])
			flag = false;
	return flag;
}

bool rainCheck (char *bitmap, unsigned char *ciphertext, unsigned int *key, unsigned int len) {
	bool found = false;
	FILE *rainFile;
	rainFile = fopen ("rainbow.txt","r");
	struct rainbows rainbee;
	unsigned int k32;

	if (rainFile != NULL) {
		int chk;
		int valid = 1;
 		while (valid == (chk = fread (&rainbee, sizeof(struct rainbows), valid, rainFile))) {
 			if (ferror(rainFile)) {
        perror("File read error");
       	break;
   		}

			if (compCipher (ciphertext, rainbee.aes) == true) {
				found = true;
				key[0] = rainbee.key;
			} else {
				k32 = (unsigned int) rainbee.key;
				int temp;
				for (temp = len; temp < s32 * 8; temp++)
					k32 &= ~(1 << temp);

				if (!TEST(bitmap, k32))
					SET(bitmap, k32);
			}
 		}
 	}
 	fclose (rainFile);
 	return found;
 }

unsigned int nextKey (char *bitmap, unsigned int k, unsigned int size) {
   	while((TEST (bitmap, k)) && k < size - 1)
   		k++;
   	if (!TEST(bitmap, k))
      SET(bitmap, k);

	return k;
}

void key128 (unsigned char *k128, unsigned int key, unsigned int len) {
	unsigned char byte;
	resetKey(k128);
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
	for (temp = 0; temp < s128; temp++)
		k128[temp] = 0x00;
}

unsigned int reduce (unsigned char *k128, unsigned int n, unsigned int position) {
	int temp, temp2;
	unsigned int padding = 3;
	unsigned char str[s32];

	for (temp = s128 - s32, temp2 = 0; temp < s128; temp++, temp2++)
		str[temp2] = k128[temp];

	unsigned char str[s32+1];
	strcpy (str, "");
	char chr[2];
	strcpy (chr, "");

	for (temp = 0; temp < s32; temp++) {
		sprintf (chr, "%X", str[temp]);
		strcat (str, chr);
	}
	char *symbol;
	unsigned int k32 = strtoul (str, &symbol, 16L);
	k32 = k32 + (position % k);

	for (temp = len; temp < s32 * 8; temp++)
		k32 &= ~(1 << temp);

	return k32;
}
