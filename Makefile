
CFLAGS := $(CFLAGS) \
		-Iinclude/ \
		-Ideps/recordlogs/include/ \
		-Ideps/signalhandling/include/ \
		-Wall

BASE_CODE_FILES := \
		src/closenonstdiofd.c			\
		src/changeprocessproperty.c		\
		src/signalprocessgroup.c		\
		src/processfork.c				\
		deps/signalhandling/src/signalhandling.c \
		deps/recordlogs/src/recordlogs_stderrprintf.c

all: test/runprintenv test/daemonizedsleep

test/runprintenv: test/runprintenv.c $(BASE_CODE_FILES)
	$(CC) $(CFLAGS) -o $@ $^

test/daemonizedsleep: test/daemonizedsleep.c $(BASE_CODE_FILES)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) test/runprintenv test/daemonizedsleep
