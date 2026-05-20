CC ?= gcc
CFLAGS += -Wall -O2
CFLAGS += -I$(OECORE_TARGET_SYSROOT)/usr/include
LDFLAGS += -L$(OECORE_TARGET_SYSROOT)/usr/lib
LDLIBS += -lteec

TARGETS = hello_tee tee_rng_session

all: $(TARGETS)

hello_tee: hello.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

tee_rng_session: tee_rng_session.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(TARGETS)