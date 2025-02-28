BUILDDIR    = ./bin/
SOURCEDIR   = ./src/

DEFINES     =

SDIRS       = $(wildcard $(SOURCEDIR)*/)
VPATH       = $(SOURCEDIR):$(SDIRS):$(foreach dir, $(SDIRS), $(wildcard $(dir)*/))

IFILES      = $(shell find $(SOURCEDIR) -name '*.cpp')

OFILES      = $(subst $(SOURCEDIR), $(BUILDDIR), $(addsuffix .o, $(notdir $(shell find $(SOURCEDIR)  -name '*.cpp'))))

CC          = clang++
LINKER      = $(CC)
ARCH        = 64

CCFLAGS     = -c -Wall -g -O2 -pthread -m$(ARCH) -std=c++11
LINKFLAGS   = -m$(ARCH) -pthread


TARGET = sbp

.PHONY: all clean

all: $(BUILDDIR) $(TARGET)

$(TARGET): $(foreach file, $(OFILES), $(BUILDDIR)$(file))
	$(LINKER) $^ $(LINKFLAGS) -o $(BUILDDIR)$@

$(BUILDDIR)%.cpp.o: %.cpp
	$(CC) $(foreach def, $(DEFINES), -D $(def)) $(CCFLAGS) $< -o $@

$(BUILDDIR):
	mkdir $@

clean:
	rm $(BUILDDIR)*
