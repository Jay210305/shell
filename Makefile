CC = gcc
CFLAGS = -Wall -g -pthread
LDFLAGS = -lreadline -lcrypto

SOURCES = main.c parser.c executor.c piping.c threading.c sync.c shared_memory.c vfs.c utils.c web.c tictactoe.c auth.c security.c
OBJS = $(SOURCES:.c=.o)
TARGET = shell

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
