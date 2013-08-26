CXXFLAGS := -Wall -std=c++11 -I../glm
#CXXFLAGS += -s EXCEPTION_DEBUG=1

# Default to make all
all: debug

# Code objects and targets

OBJDIR := objects
$(OBJDIR):
	mkdir $(OBJDIR)

SOURCES := $(wildcard *.cpp)
OBJS := $(addprefix $(OBJDIR)/,$(subst .cpp,.bc,$(SOURCES)))
$(OBJS): | $(OBJDIR)

BUILDDIR := build

TARGET := $(BUILDDIR)/10sectaxi.js
ALLTARGETS := $(TARGET)
SIDETARGETS := $(BUILDDIR)/10sectaxi.js.map

# Data and related

DATADIR := data
PRELOAD_FILE := $(BUILDDIR)/10sectaxi.data
PRELOAD_SCRIPT := $(BUILDDIR)/preload.js
$(PRELOAD_FILE) $(PRELOAD_SCRIPT): $(DATADIR)/*
	python /opt/emscripten/tools/file_packager.py $(PRELOAD_FILE) --preload $(DATADIR) --js-output=$(PRELOAD_SCRIPT) \
	--pre-run

ALLTARGETS += $(PRELOAD_FILE) $(PRELOAD_SCRIPT)

$(OBJDIR)/%.bc: %.cpp %.h
	emcc $(CXXFLAGS) -o $@ $<

EXPORT_FUNCS := "['_main', '_malloc', '_app_toggleMusic']"
PAGE_FUNCS := _page_toggleMusic _page_isMusicEnabled
JS_LIB = $(BUILDDIR)/page-fns.js

$(TARGET): $(OBJS) | $(JS_LIB)
	emcc $(CXXFLAGS) -s EXPORTED_FUNCTIONS=$(EXPORT_FUNCS) --js-library $(JS_LIB) -o $@ $^

# Clean

.PHONY : clean
clean:
	rm -f $(OBJS)
	if [ -e $(OBJDIR) ]; then rmdir $(OBJDIR); fi
	rm -f $(ALLTARGETS) $(SIDETARGETS)

# Main build configs

main: $(ALLTARGETS)

debug: main
debug: CXXFLAGS += -g

release: main
	python release-fixer.py $(TARGET) $(PAGE_FUNCS)
release: CXXFLAGS += -O2 -s DISABLE_EXCEPTION_CATCHING=1
