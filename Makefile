CFLAGS	=	-Wall

all: clean build

clean:
	rm -rf ./build

build: build/runDaemon

build/runDaemon: build/runDaemon.o
	$(CC) $(CFLAGS) -o $@ $?

build/runDaemon.o: src/runDaemon.c
	@mkdir -p ./build
	$(CC) $(CFLAGS) -c -o $@ $?
