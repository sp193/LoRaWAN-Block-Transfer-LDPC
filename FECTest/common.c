#include <string.h>
#include <limits.h>

#include "common.h"

static int prbs23(int start);

void matrixLine(unsigned char *matrix, int n, int m) {
	int x, r, mAdd, numCoeff;

	memset(matrix, 0, m / CHAR_BIT + (m % CHAR_BIT != 0));

	if (n < m) {
		setBit(matrix, n);
	}
	else {
		// we must treat powers of 2 differently to make sure matrix content is close to random. Powers of 2 tend to generate patterns
		mAdd = m != 0 && ((m & (m - 1)) == 0);

		x = 1 + 1001 * (n + 1); //initialize the seed differently for each line
		for (numCoeff = 0; numCoeff < m / 2; numCoeff++) { //will generate a line with M/2 bits set to 1 (50%)
			for (r = 1 << 16; r >= m; r = x % (m + mAdd)) {
				x = prbs23(x);
			}

			setBit(matrix, r); // set 1 to the column which was randomly selected.
		}
	}

}

/*
  A PRBS generator with 2²³ period.
 */
static int prbs23(int start) {
	return start / 2 + (((start & 1) ^ (start >> 5 & 1)) << 22);
}

void xor(unsigned char *v1, unsigned char *v2, int m) {
	int i;

	for (i = 0; i < m; i++)
		v1[i] ^= v2[i];
}

void set(unsigned char *v1, unsigned char *v2, int m) {
	int i;

	for (i = 0; i < m; i++)
		v1[i] = v2[i];
}

void setBit(unsigned char *v, int bit) {
	v[bit / CHAR_BIT] |= 1 << (CHAR_BIT - 1 - bit % CHAR_BIT);
}

int getBit(unsigned char *v, int bit) {
	return ((v[bit / CHAR_BIT] >> (CHAR_BIT - 1 - bit % CHAR_BIT)) & 1);
}
