#pragma once

/*
  Encodes a fragment.
  in is a pointer to data to encode, consisting of n fragments.
  n is the fragment number [0...N].
  m is the number of data fragments. n may be higher than m, when there are reduncy fragments (which are coded).
  fragSize is the size of each fragment, in bytes.
  out is a pointer to storage for the encoded fragment.
 */
void encode(unsigned char *in, int n, int m, int fragSize, unsigned char *out);