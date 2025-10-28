# Makefile for tempDB Client

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2 -g
TARGET = tempDB-client
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPS = $(SOURCES:.cpp=.d)

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

# Compile source files with dependency tracking
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean build files
clean:
	rm -f $(OBJECTS) $(DEPS) $(TARGET)
	rm -f $(SRCDIR)/*.o $(SRCDIR)/*.d

# Install target (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall target (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Development target with debug symbols
debug: CXXFLAGS += -DDEBUG -O0
debug: clean $(TARGET)

# Release target with optimizations
release: CXXFLAGS += -DNDEBUG -O3 -flto
release: clean $(TARGET)

# Test target (if you add tests later)
test:
	@echo "No tests defined yet"

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  clean     - Remove build files"
	@echo "  install   - Install to /usr/local/bin/"
	@echo "  uninstall - Remove from /usr/local/bin/"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  test      - Run tests (none defined)"
	@echo "  help      - Show this help message"

.PHONY: all clean install uninstall debug release test help
