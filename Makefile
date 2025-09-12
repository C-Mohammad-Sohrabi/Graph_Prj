# =============================================================================
# Configuration
# =============================================================================
CC = gcc
CFLAGS = -g -Wall -Wextra -std=c11 -lm
SRCDIR = src
INCDIR = include
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
EXEC = $(BUILDDIR)/graph_program
INCLUDES = -I$(INCDIR)

# Files/dirs created at runtime
GENERATED_FILES = dot_file image $(BUILDDIR)
GENERATED_ARTIFACTS = dot_file/graph.dot image/graph.png dot_file/line_graph.dot image/line_graph.png

# =============================================================================
# Source Files (explicit listing to ensure all files are included)
# =============================================================================
SOURCES = $(SRCDIR)/main.c \
          $(SRCDIR)/clique.c \
          $(SRCDIR)/connectivity.c \
          $(SRCDIR)/euler_path.c \
          $(SRCDIR)/havel_hakimi.c \
          $(SRCDIR)/independent_set.c \
          $(SRCDIR)/line_graph.c \
          $(SRCDIR)/set_utils.c \
          $(SRCDIR)/vertex_cover.c \
          $(SRCDIR)/connectivity_number.c

OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
DEPS = $(OBJECTS:.o=.d)

# Header files for dependency tracking
HEADERS = $(INCDIR)/structs.h \
          $(INCDIR)/clique.h \
          $(INCDIR)/connectivity.h \
          $(INCDIR)/euler_path.h \
          $(INCDIR)/havel_hakimi.h \
          $(INCDIR)/independent_set.h \
          $(INCDIR)/line_graph.h \
          $(INCDIR)/set_utils.h \
          $(INCDIR)/vertex_cover.h \
          $(INCDIR)/connectivity_number.h

# =============================================================================
# Build Rules (Targets)
# =============================================================================
.PHONY: all clean install run debug help

all: $(EXEC)

$(EXEC): $(OBJECTS)
	@echo "==> Linking $@..."
	$(CC) $(OBJECTS) -o $@ $(CFLAGS)
	@echo "==> Build complete: $@"

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "==> Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Directory creation
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Development targets
install: all
	@echo "==> Installing to /usr/local/bin..."
	@sudo cp $(EXEC) /usr/local/bin/graph_analyzer
	@echo "==> Installed as 'graph_analyzer'"

run: $(EXEC)
	@echo "==> Running $(EXEC)..."
	@./$(EXEC)

debug: CFLAGS += -DDEBUG -O0
debug: $(EXEC)
	@echo "==> Debug build complete"

# Comprehensive clean
clean:
	@echo "==> Cleaning up..."
	@rm -rf $(BUILDDIR)
	@rm -rf dot_file image
	@rm -f $(GENERATED_ARTIFACTS)
	@rm -f core core.*
	@rm -f *.tmp *.log
	@echo "==> Clean complete"

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build the program (default)"
	@echo "  clean    - Remove all build artifacts and generated files"
	@echo "  run      - Build and run the program"
	@echo "  debug    - Build with debug flags"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  help     - Show this help message"

# =============================================================================
# Dependency Inclusion
# =============================================================================
-include $(DEPS)

# =============================================================================
# File Organization Check
# =============================================================================
check-structure:
	@echo "==> Checking project structure..."
	@echo "Source files:"
	@ls -la $(SRCDIR)/ 2>/dev/null || echo "  $(SRCDIR)/ directory not found"
	@echo "Header files:"
	@ls -la $(INCDIR)/ 2>/dev/null || echo "  $(INCDIR)/ directory not found"
	@echo "Build directory:"
	@ls -la $(BUILDDIR)/ 2>/dev/null || echo "  $(BUILDDIR)/ directory will be created"
	@echo "Object files:"
	@ls -la $(OBJDIR)/ 2>/dev/null || echo "  $(OBJDIR)/ directory will be created"