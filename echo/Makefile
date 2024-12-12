.PHONY: all debug release clean

Compiler = g++
RELEASE_FLAGS = -O2 -Wall -DNDEBUG 
DEBUG_FLAGS   = -g -O0 -Wall 
RELEASE_EXEC  = echo
DEBUG_EXEC    = echo-dbg
SOURCES       = echo.cpp
LIBRARY       = -lboost_program_options
all: debug release

debug: $(DEBUG_EXEC)

$(DEBUG_EXEC): $(SOURCES)
	$(Compiler) $(DEBUG_FLAGS) $(SOURCES) -o $(DEBUG_EXEC) $(LIBRARY)

release: $(RELEASE_EXEC)

$(RELEASE_EXEC): $(SOURCES)
	$(Compiler) $(RELEASE_FLAGS) $(SOURCES) -o $(RELEASE_EXEC) $(LIBRARY)

clean:
	rm -f $(RELEASE_EXEC) $(DEBUG_EXEC)
