TARGET = sen
SRC = main.c

INSTALL = /usr/bin/install -c -D
BIN_DIR = /bin

CXX := gcc
CXXFLAGS := -O2 -Wall -Wextra --pedantic-errors

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

install: $(TARGET)
	@echo "Installing $(BIN_DIR)/$(TARGET) ..."
	$(INSTALL) $(TARGET) "$(BIN_DIR)/$(TARGET)"

clean:
	rm -f $(TARGET)
