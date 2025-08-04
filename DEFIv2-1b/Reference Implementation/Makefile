CC = /usr/bin/gcc
CFLAGS = -g -O3 -std=c99 -w
LDFLAGS = -static-libgcc -lssl -lcrypto

SOURCES = sign.c defiv2_keygen.c defiv2_siggen.c defiv2_sigver.c keccak.c rng.c rng_functions.c common_functions.c PQCgenKAT_sign.c
HEADERS = api.h parameters.h defiv2_keygen.h defiv2_siggen.h defiv2_sigver.h keccak.h rng.h rng_functions.h common_functions.h

PQCgenKAT_sign: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

.PHONY: clean

clean:
	-rm PQCgenKAT_sign