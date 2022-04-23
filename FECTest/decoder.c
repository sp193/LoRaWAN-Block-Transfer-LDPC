#include <string.h>
#include <limits.h>

#include "common.h"
#include "decoder.h"

static int allLinesOfAUpdated(unsigned char **a, int m);
static int positionOfLeadingOne(unsigned char *v, int m);

int decode(unsigned char *p, int n, int m, int fragSize, unsigned char *s, unsigned char **a) {
	unsigned char c[MAX_BITMAP_SIZE];
	int i, j, leadingOne;

	do {
		// 1. fetch the corresponding line of the parity check matrix : C = matrix_line(N, M)
		matrixLine(c, n, m);

		// 2. Proceed from left to right along the C vector (𝑖 𝑣𝑎𝑟𝑦𝑖𝑛𝑔 𝑓𝑟𝑜𝑚 1 𝑡𝑜 𝑀)
		for (i = 0; i < m; i++) {
			// For each entry 𝐶𝑖 equal to 1, check if the line 𝑖 of the matrix A contains a 1 in row 𝑖.
			if (getBit(c, i) == 1 && getBit(a[i], i) == 1) {
				//perform a Xor between line 𝑖 of matrix A “A(𝑖)“ and the vector C and store the result in C
				xor (c, a[i], m / CHAR_BIT + (m % CHAR_BIT != 0));
				//perform a xor between PMN and the coded fragment stored at position 𝑖 in the fragment memory store 𝑆𝑖 and update PMN with the result
				xor (p, &s[i * fragSize], fragSize);
			}
		}

		// 3
		leadingOne = positionOfLeadingOne(c, m);

		if (leadingOne < 0) {
			// 3a. C now contains only zeros, in that case just get rid of the coded fragment PMN; it isn’t bringing any new information
			return 0;
		}
		else { // 3b
			//write it in the matrix A at the line 𝑖 corresponding to the first non-zero element of C
			set(a[leadingOne], c, m / CHAR_BIT + (m % CHAR_BIT != 0));
			//add the modified PMN fragment to the memory store at position 𝑖 : 𝑆𝑖
			set(&s[leadingOne * fragSize], p, fragSize);
		}
		// 4. Loop to 1 until all lines of the matrix A have been updated
	} while (!allLinesOfAUpdated(a, m));

	// 5
	for (i = m - 1; i >= 0; i--) {
		for (j = m - 1; j > i; j--) {
			//For any 1 at position j> 𝑖 perform a xor between 𝑆𝑖 and 𝑆𝑗 and update 𝑆𝑖 with the result.
			if (getBit(a[i], j)) {
				xor (&s[i * fragSize], &s[j * fragSize], fragSize);
			}
		}
	}

	// 6. The fragment memory store now contains the original uncoded fragments 𝑆𝑖 = 𝐵𝑖
	return 1;
}

/*
  Returns whether all lines of A have been updated.
  The matrix A will have only 1’s on its diagonal and will be a triangular matrix with only 0’s on the lower left half.
*/
static int allLinesOfAUpdated(unsigned char **a, int m) {
	int i;
	for (i = 0; i < m; i++) {
		if (positionOfLeadingOne(a[i], m) != i)
			return 0;
	}

	return 1;
}

int getMissing(unsigned char **a, int m) {
	int i, missing;

	missing = 0;
	for (i = 0; i < m; i++) {
		if (positionOfLeadingOne(a[i], m) != i)
			missing++;
	}

	return missing;
}

/*
  Given a vector v of length m, returns the index of the first bit that is set to 1.
  If all bits are 0, -1 is returned.
*/
static int positionOfLeadingOne(unsigned char *v, int m) {
	int i;

	for (i = 0; i < m; i++) {
		if (getBit(v, i)) {
			return i;
		}
	}

	return -1;
}
