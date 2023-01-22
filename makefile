TARGET := traffic_simulator

INCLUDE := include
DIST := dist
SRC := src

FLAGS := -I$(INCLUDE) -Wall -O3 -march=native
LIBS := -lSDL2 -lSDL2_image -lcollections -lm
CC := gcc

DIST_DIRS := $(patsubst $(SRC)/%,$(DIST)/%,$(shell find $(SRC) -type d -links 2))
OBJECTS := $(patsubst $(SRC)/%.c,$(DIST)/%.o,$(shell find $(SRC) -type f -iname *.c))

all: $(DIST_DIRS) $(DIST)/$(TARGET)

.PHONY: clean

$(DIST_DIRS) $(DIST):
	mkdir -p $@

$(DIST)/%.o: $(SRC)/%.c | $(INCLUDE)/*.h $(DIST_DIRS) $(DIST)
	$(CC) -c -o $@ $< $(FLAGS)

$(DIST)/$(TARGET): $(OBJECTS) | $(DIST_DIRS) $(DIST)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

clean:
	rm -rf $(DIST)

run:
	@$(DIST)/$(TARGET)
