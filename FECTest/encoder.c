#include <string.h>
#include <limits.h>

#include "common.h"
#include "encoder.h"

void encode(unsigned char *in, int n, int m, int fragSize, unsigned char *out) {
	int i;
	unsigned char matrix[MAX_BITMAP_SIZE];

	memset(out, 0, fragSize);
	matrixLine(matrix, n, m);
	for (i = 0; i < m; i++) {
		if (getBit(matrix, i)) {
			xor(out, &in[i * fragSize], fragSize);
		}
	}
}
