CC = gcc
CFLAGS = -pthread
TARGET_CLIENT = client
TARGET_SERVER = server
SOURCES_CLIENT = client.c
SOURCES_SERVER = server.c
SOURCES_LIB = factorial_lib.c
TARGET_LIB = libfactorial.a

all: $(TARGET_LIB) $(TARGET_CLIENT) $(TARGET_SERVER)

$(TARGET_LIB): $(SOURCES_LIB)
	$(CC) $(CFLAGS) -c $(SOURCES_LIB) -o factorial_lib.o
	ar rcs $(TARGET_LIB) factorial_lib.o

$(TARGET_CLIENT): $(SOURCES_CLIENT) $(TARGET_LIB)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT) $(SOURCES_CLIENT) -L. -lfactorial

$(TARGET_SERVER): $(SOURCES_SERVER) $(TARGET_LIB)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(SOURCES_SERVER) -L. -lfactorial

clean:
	rm -f $(TARGET_CLIENT) $(TARGET_SERVER) $(TARGET_LIB) *.o

.PHONY: all clean