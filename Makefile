
CFLAGS := $(CFLAGS) \
		-Iinclude/ \
		-Ideps/recordlogs/include/ \
		-Ideps/signalhandling/include/ \
		-Wall

BASE_CODE_FILES := \
		src/closenonstdiofd.c src/processfork.c \
		deps/signalhandling/src/signalhandling.c \
		deps/recordlogs/src/recordlogs_stderrprintf.c

test/runprintenv: test/runprintenv.c $(BASE_CODE_FILES)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) test/runprintenv
