CC = gcc
CFLAGS = -Wall -Wextra # -I./discord-rpc/include
LDFLAGS = -ldiscord-rpc# -L./discord-rpc/lib

TARGET = ldrpc
SRC = ldrpc.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)
