#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <malloc.h>

#include "common.h"
#include "encoder.h"
#include "decoder.h"

#define MAX_FRAGS (256 / CHAR_BIT)

struct CodingRatio {
	short int antecedent;
	short int consequent;
};

static int parseCodingRatio(const char *arg, struct CodingRatio* cr);
static void createMissingFragsList(unsigned char *missingFrags, short int numLosses, short int n);
static int isFragInList(unsigned char *missingFrags, short int numLosses, short int frag);

int main(int argc, char *argv[]) {
	struct CodingRatio cr;
	unsigned char missingFrags[MAX_FRAGS]; // Bitmap of fragments to deliberately lose, for simulating fragment losses.
	short int numLosses;
	int inLen, inLenRounded;
	short int n, m, fragSize, i;
	int r;
	char *endptr;
	unsigned char *inBuf, *encoded, *decoded;
	unsigned char **a;
	FILE *in, *out;

	if (argc != 6) {
		fprintf(stderr, "Syntax error. %s <input file> <output file> <fragment size> <coding ratio> <losses>\n"
				"\tfragment size:\tthe size of each fragment, in bytes.\n"
				"\tcoding ratio:\tthe ratio between M and N, such as 1:2, 4:5 etc.\n"
				"\tlosses:\t\tthe number of frames to randomly lose, to simulate packet loss.\n"
				"Example: %s input.bin output.bin 32 4:5 10\n", argv[0], argv[0]);
		return EINVAL;
	}

	fragSize = (short int) strtol(argv[3], NULL, 10);
	if (fragSize == 0) {
		fprintf(stderr, "Invalid fragment size: %s\n", argv[3]);
		return EINVAL;
	}

	if (parseCodingRatio(argv[4], &cr)) {
		fprintf(stderr, "Invalid coding ratio: %s\n", argv[4]);
		return EINVAL;
	}

	numLosses = (short int) strtol(argv[5], &endptr, 10);
	if (numLosses == 0 && *endptr != '\0') {
		fprintf(stderr, "Invalid number of losses: %s\n", argv[5]);
		return EINVAL;
	}

	printf("Block size %d, CR %d/%d, in %s, out %s\n", fragSize, cr.antecedent, cr.consequent, argv[1], argv[2]);

	in = fopen(argv[1], "rb");
	if (in == NULL) {
		fprintf(stderr, "Failed to open input %s\n", argv[1]);
		return ENOENT;
	}

	fseek(in, 0, SEEK_END);
	inLen = ftell(in);
	rewind(in);

	inLenRounded = (inLen + fragSize - 1) / fragSize * fragSize;
	inBuf = malloc(inLenRounded);
	fread(inBuf, 1, inLen, in);
	fclose(in);
	memset(&inBuf[inLen], 0, inLenRounded - inLen);

	m = inLenRounded / fragSize;
	n = (m / cr.antecedent +  (m % cr.antecedent != 0)) * cr.consequent;

	printf("n/m: %d/%d\n", n, m);

	encoded = malloc(n * fragSize);
	memset(encoded, 0, n * fragSize);
	decoded = malloc(fragSize * m);
	memset(decoded, 0, fragSize * m);
	a = malloc(sizeof(unsigned char*) * m);
	for (i = 0; i < m; i++) {
		a[i] = malloc(m / CHAR_BIT + (m % CHAR_BIT != 0));
		memset(a[i], 0, m / CHAR_BIT + (m % CHAR_BIT != 0));
	}

	for (i = 0; i < n; i++) {
		encode(inBuf, i, m, fragSize, &encoded[i * fragSize]);
	}

	// Compute the list of frames that should be missing.
	createMissingFragsList(missingFrags, numLosses, n);

	// Main loop.
	for (i = 0; i < n; i++) {
		if (isFragInList(missingFrags, numLosses, i)) // Simulate message loss.
			continue;

		r = decode(&encoded[i * fragSize], i, m, fragSize, decoded, a);
		printf("%02d: ret %d, %d remaining\n", i, r, getMissing(a, m));
	}
	if (getMissing(a, m) == 0)
		printf("Message received successfully.\n");
	else
		printf("Unable to completely recover message!\n");

	// Regardless of outcome, write decoded output.
	out = fopen(argv[2], "wb");
	fwrite(decoded, 1, m * fragSize, out);
	fclose(out);

	free(inBuf);
	free(encoded);
	free(decoded);
	for (i = 0; i < m; i++)
		free(a[i]);
	free(a);

	return 0;
}

static int parseCodingRatio(const char *arg, struct CodingRatio* cr) {
	char *next;
	cr->antecedent = (short int) strtol(arg, &next, 10);
	if (cr->antecedent != 0 && next[0] != '\0' && next[1] != '\0') {
		cr->consequent = (short int)strtol(&next[1], NULL, 10);
		return cr->consequent != 0 ? 0 : EINVAL;
	}
	else {
		return EINVAL;
	}
}

static void createMissingFragsList(unsigned char *missingFrags, short int numLosses, short int n) {
	short int i, frag;
	memset(missingFrags, 0, MAX_FRAGS);
	for (i = 0; i < numLosses; i++) {
		do {
			frag = rand() % n;
		} while (isFragInList(missingFrags, numLosses, frag));
		missingFrags[frag / CHAR_BIT] |= (1 << (frag % CHAR_BIT));
	}
}

static int isFragInList(unsigned char *missingFrags, short int numLosses, short int frag) {
	return (missingFrags[frag / CHAR_BIT] & (1 << (frag % CHAR_BIT))) != 0;
}
