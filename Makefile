PROJECT_NAME = libpastel2d
PROJECT_VERSION = 0.1.0

LIB = $(PROJECT_NAME).a

all: $(LIB)

include contrib/pastel-base/mk/config.mk

CPPFLAGS += -Isrc -Icontrib/pastel-base/pl_log -Icontrib/pocketmod -Icontrib/stb `pkg-config --cflags sdl3`

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

$(LIB): $(OBJ)
	ar rcs $@ $^

.PHONY: clean
clean:
	rm -f $(LIB) $(OBJ) $(CLEANFILES)
