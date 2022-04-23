#pragma once

#define MAX_BITMAP_SIZE (256 / CHAR_BIT) // Assumption: m will never specify more fragments than can be represented by this.

/*
  Generates a parity-check vector, of length m.
  Refer to figure 1 of the LoRaWAN Fragmented Data Block Transport v1.0.0 document.
  When n < m, the vector will have a 1 set in the position n, while other bits will be 0. An identify matrix is formed when the first M results are combined into a matrix.
  When n ≥ m, the vector will contain as statistically as many 0s and 1s. The order is pseudo-random, derived from n.
  n is the line number [0...N], of this vector.
  m is the number of data fragments. n may be higher than m, when there are reduncy fragments (which are coded).
*/
void matrixLine(unsigned char *matrix, int n, int m);

/*
  Performs a logical XOR across all bytes of the vectors v1 and v2, storing the result in v1.
  Each vector is of size m bytes.
*/
void xor(unsigned char *v1, unsigned char *v2, int m);
/*
  Sets the value of the bytes of vector v2 into v1. Each vector is of size m bytes.
*/
void set(unsigned char *v1, unsigned char *v2, int m);
/*
  Sets the specified bit of vector v, to 1.
*/
void setBit(unsigned char *v, int bit);
/*
  Returns the value of the given bit, of vector v.
*/
int getBit(unsigned char *v, int bit);
