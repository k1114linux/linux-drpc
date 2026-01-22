CC = gcc
CFLAGS = -std=c++17 -Wall -Wextra		  # -I./discord-rpc/include
LDFLAGS = -lstdc++ -ldiscord-rpc# -L./discord-rpc/lib

TARGET = ldrpc
SRC = ldrpc.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)
