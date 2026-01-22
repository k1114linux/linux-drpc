CC      := gcc
CFLAGS  := -Wall -Wextra
LDFLAGS := -lstdc++ -ldiscord-rpc

TARGET  := ldrpc
SRC     := ldrpc.c

PREFIX  ?= /usr/local
BINDIR  := $(PREFIX)/bin

.PHONY: all build clean install uninstall

all: build

build: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

install: $(TARGET)
	@echo "Installing $(TARGET) to $(BINDIR)"
	install -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	@echo "Removing $(TARGET) from $(BINDIR)"
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

clean:
	rm -f $(TARGET)
