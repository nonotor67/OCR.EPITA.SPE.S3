SHELL = /bin/sh

BUILDDIR = build/make

.SUFFIXES:

.PHONY: all
all:
	cmake -S . -B $(BUILDDIR) -G Ninja -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
