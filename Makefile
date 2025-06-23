CC=gcc
CFLAGS=-Wall -g -pthread
LDFLAGS=-lreadline -lcrypto

SHELL_SRCS=main.c parser.c executor.c piping.c threading.c sync.c shared_memory.c vfs.c utils.c web.c tictactoe.c auth.c security.c
SHELL_OBJS=$(SHELL_SRCS:.c=.o)
TARGET=shell

all: useradd $(TARGET)

useradd: useradd.c
	$(CC) $(CFLAGS) -o useradd useradd.c $(LDFLAGS)

$(TARGET): $(SHELL_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SHELL_OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(SHELL_OBJS) useradd $(TARGET)