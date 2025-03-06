PROJECT_NAME = libpastel2d
PROJECT_VERSION = 0.1.0

LIB = $(PROJECT_NAME).a
LIB_CC = $(PROJECT_NAME)-cc.a

all: $(LIB) $(LIB_CC)

include contrib/pastel-base/mk/config.mk

CPPFLAGS += -Isrc -Icontrib/pastel-base/pl_log -Icontrib/pocketmod -Icontrib/stb $(shell pkg-config --cflags sdl3)

# library

OBJ = \
	src/graphics.o \
	src/pastel2d.o \
	src/res.o \
	src/scene.o \
	src/context.o \
	src/manip.o \
	src/private/textcache.o \
	src/audio.o

OBJ_CC = src/pastel2d-cc.o

$(LIB): $(OBJ)
	ar rcs $@ $^

$(LIB_CC): $(OBJ) $(OBJ_CC)
	ar rcs $@ $^

# examples

EMBED = \
	example/Born2bSportyFS.otf \
	example/example.png \
	example/example.tileset.lua \
	example/example-shadow.tileset.lua \
	example/nemesis.mod \
	example/OpenSans-Medium.ttf \
	example/shotgun.wav

example/example.o: $(EMBED:=.h)

example-c: $(LIB) example/example.o libluajit.a
	$(CC) $^ -o $@ $(shell pkg-config --libs sdl3)

example-cc: $(LIB_CC) example/example-cc.o libluajit.a
	$(CXX) $^ -o $@ $(shell pkg-config --libs sdl3)

.PHONY: clean
clean:
	rm -f $(LIB) $(OBJ) $(OBJ_CC) $(CLEANFILES) example-c example/*.o $(EMBED:=.h)
