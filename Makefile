PROJECT_NAME = libpastel2d
PROJECT_VERSION = 0.1.0

LIB = $(PROJECT_NAME).a
LIB_CC = $(PROJECT_NAME)-cc.a

all: $(LIB) $(LIB_CC)

include contrib/pastel-base/mk/config.mk

#
# config
#

CPPFLAGS += -Isrc -Icontrib/pastel-base/pl_log -Icontrib/pocketmod -Icontrib/stb $(shell pkg-config --cflags luajit sdl3)
LDFLAGS += $(shell pkg-config --libs luajit sdl3)

#
# library
#

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

#
# examples
#

EMBED = \
	example/Born2bSportyFS.otf \
	example/example.png \
	example/example.tileset.lua \
	example/example-shadow.tileset.lua \
	example/nemesis.mod \
	example/OpenSans-Medium.ttf \
	example/shotgun.wav

example/example.o: $(EMBED:=.h)
example/example-cc.o: $(EMBED:=.h)

example-c: example/example.o $(LIB)
	$(CC) -o $@ $^ $(LDFLAGS)

example-cc: example/example-cc.o $(LIB_CC)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(LIB) $(OBJ) $(OBJ_CC) $(CLEANFILES) example-c example-cc example/*.o $(EMBED:=.h)
